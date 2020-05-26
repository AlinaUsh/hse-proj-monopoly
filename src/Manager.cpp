#include "Manager.h"
#include "Game.h"
#include "view/SFMLView.h"
#include <string_view>
#include <cassert>

#include <sockpp/tcp_connector.h>

void Manager::addPlayer(std::unique_ptr<Player> player) {
    std::lock_guard g(playerMutex);
    players.push_back(std::move(player));
}

PlayerReply Manager::sendRequest(PlayerRequest request) {
    std::lock_guard g(playerMutex);
    Token token = request->player;
    for (auto& p : players) {
        if (p->token == token) {
            return p->send(std::move(request));
        }
    }
    throw 1; //TODO: сделать PlayerNotFoundException или что-нибудь такое
}


void Manager::createGame() {
    std::lock_guard g(playerMutex);
    assert(!game);
    std::vector<std::pair<std::string_view, Token>> gameData;
    gameData.reserve(players.size());
    for (const auto& p : players) {
        gameData.emplace_back(std::string_view(p->name), p->token);
    }
    game = std::make_shared<LocalGame>(gameData, *this);
}

void Manager::sync(const BoardModel& board) {
    std::lock_guard g(playerMutex);
    for (auto& p : players) {
        p->sync(board);
    }
}

void Manager::startGame() {
    gameThread = std::thread([this]() {
        createGame();
        sync(game->getBoard());
        game->run();
    });
}

void Manager::createView() {
    view = std::make_shared<SFMLView>(*this);
}

void Manager::run() {
    createServer();
    createView();
    view->mainLoop();
}

bool Manager::isGameStarted() {
    return bool(game);
}

Manager::~Manager() {
    view->dispose();
    if (gameThread.joinable()) {
        gameThread.join();
    }
    if (serverThread.joinable()) {
        serverThread.join();
    }
}

void Manager::startRemoteGame(const sockpp::inet_address& addr) {
    {
        std::lock_guard g(serverMutex);
        server.reset();
    }
    gameThread = std::thread([this, addr]() {
        auto conn = std::make_unique<sockpp::tcp_connector>(addr);
        {
            std::lock_guard g(playerMutex);
            assert(players.size() == 1);
            game = std::make_shared<NetworkGame>(
                    Monopoly::Network::Client(addr, std::move(conn), [this]() { game->terminate(); }),
                    *players[0]
            );
        }
        sync(game->getBoard());
        game->run();
    });
}

void Manager::createServer() {
    serverThread = std::thread([this]() {
        {
            std::lock_guard g(serverMutex);
            server = std::make_shared<Monopoly::Network::MonopolyServer>(*this);
        }
        serverMutex.lock();
        if(!server) return;
        auto& serv = *server;
        serverMutex.unlock();
        serv.mainLoop();
    });
}
