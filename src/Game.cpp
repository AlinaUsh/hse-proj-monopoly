#include "Game.h"
#include "Field.h"
#include <utility>
#include "Manager.h"


Game::Game(const std::vector<std::pair<std::string_view, Token>>& players, Manager& manager)
        : board(players, *this), manager(manager) {}

PlayerReply Game::send(PlayerRequest request) {
    sync();
    PlayerReply rep = manager.sendRequest(std::move(request));
    if (rep && rep->type == RequestType::ACTION && rep->data.action == PlayerAction::FINISH_GAME) getBoard().terminate();
    return rep;
}


void Game::runGame() {
    using namespace Monopoly::Requests;
    std::size_t curPlayerNum = 0;
    while (!board.isFinished()) {
        board.setPlayerIndex(curPlayerNum);
        PlayerData& curPlayer = board.getPlayer(board.getPlayerToken(curPlayerNum));

        if (!curPlayer.alive) {
            curPlayerNum = (curPlayerNum + 1) % board.getPlayersNumber();
            continue;
        }
        PlayerReply reply = board.send(action(curPlayer.token, {PlayerAction::ROLL_DICE}));
        if (board.isFinished()) break;
        if (curPlayer.prisoner) {
            board.getField()[curPlayer.position]->onPlayerEntry(curPlayer.token);
            curPlayerNum = (curPlayerNum + 1) % board.getPlayersNumber();
            continue;
        }
        int firstTrow = rng.nextInt(1, 6), secondTrow = rng.nextInt(1, 6);
        curPlayer.lastTrow = firstTrow + secondTrow;

        send(message(curPlayer.token, MessageType::DICE,
                     std::to_string(firstTrow) + " " + std::to_string(secondTrow)));
        bool extraTurn = false;
        if (firstTrow == secondTrow) {
            curPlayer.doubleDice++;
            extraTurn = true;
        } else {
            curPlayer.doubleDice = 0;
        }
        if (curPlayer.doubleDice == 3) {
            extraTurn = false;
            curPlayer.toPrison();
        } else {
            curPlayer.newPosition(firstTrow + secondTrow);
        }
        board.getField()[curPlayer.position]->onPlayerEntry(curPlayer.token);
        if (!extraTurn) {
            curPlayerNum = (curPlayerNum + 1) % board.getPlayersNumber();
        }
        sync();
    }
    board.send(message(board.getWinner(), MessageType::INFO, "The game is finished!"));
    sync();
}

//TODO should be const, non-const only for debug
Board& Game::getBoard() {
    return board;
}

void Game::sync() {
    manager.sync(board);
}