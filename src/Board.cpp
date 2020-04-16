#include "Board.h"
#include "Field.h"
#include "Game.h"

PlayerData::PlayerData(std::string_view name, Token token) : name(name), token(token) {}

int PlayerData::getMoney() {
    return money;
}

int PlayerData::addMoney(int newMoney) {
    money += newMoney;
    return money;
}

void PlayerData::setLoser() {
    alive = false;
    //TODO:
}

void PlayerData::newPosition(std::size_t throwSum) {
    std::size_t newPos = (position + throwSum) % Board::FIELD_SIZE;
    if (newPos < position) {
        (*this).addMoney(START_SUM);
    }
    position = (position + throwSum) % Board::FIELD_SIZE;
}

void PlayerData::toPrison() {
    position = PRISON_POS;
    daysLeftInPrison = 3;
    doubleDice = 0;
    prisoner = true;
}

void PlayerData::outOfPrison() {
    prisoner = false;
    daysLeftInPrison = 0;
}

Board::Board(const std::vector<std::pair<std::string_view, Token> > & playersList, Game& game) : field(), deck(), game(game) {
    players.reserve(playersList.size());
    for (auto [name, token] : playersList) {
        players.emplace_back(name, token);
    }
    numOfAlivePlayers = playersList.size();
    //TODO: Fill field with tiles
    field[0] = new Start(*this, 0, "Start");

    field[1] = new Street(*this, 1, "Zhitnaya street",
            60, Color::COL1, 50);

    field[2] = new PublicTreasury(*this, 2, "Public treasury");

    field[3] = new Street(*this, 3, "Nagatinskaya street",
            60, Color::COL1, 50);

    field[4] = new IncomeTax(*this, 4, "Income tax", 200);

    field[5] = new Railway(*this, 5, "Riga railway",
            200, Color::COL1);

    field[6] = new Street(*this, 6, "Varshavskoe highway",
            100, Color::COL2, 50);

    field[7] = new Chance(*this, 7, "Chance");

    field[8] = new Street(*this, 8, "Ogareva street",
            100, Color::COL2, 50);

    field[9] = new Street(*this, 9, "the 1st Park street",
            120, Color::COL2, 50);

    field[10] = new Prison(*this, 10, "Prison");

    field[11] = new Street(*this, 11, "Polyanka street",
            140, Color::COL3, 100);

    field[12] = new Utility(*this, 12, "Electric station",
            150, Color::COL1);

    field[13] = new Street(*this, 13, "Sretenka",
            140, Color::COL3, 100);

    field[14] = new Street(*this, 14, "Rostov embankment",
            160, Color::COL3, 100);

    field[15] = new Railway(*this, 15, "Kursk railway",
            200, Color::COL1);

    field[16] = new Street(*this, 16, "Ryazansky prospect",
            180, Color::COL4, 100);

    field[17] = new PublicTreasury(*this, 17, "Public treasury");

    field[18] = new Street(*this, 18, "Vavilov street",
            180, Color::COL4, 100);

    field[19] = new Street(*this, 19, "Rublevka",
            200, Color::COL4, 100);

    field[20] = new FreeParking(*this, 20, "Free parking");

    field[21] = new Street(*this, 21, "Tverskaya street",
            220, Color::COL5, 150);

    field[22] = new Chance(*this, 22, "Chance");

    field[23] = new Street(*this, 23, "Pushkinskaya street",
            220, Color::COL5, 150);

    field[24] = new Street(*this, 24, "Mayakovsky square",
            240, Color::COL5, 150);

    field[25] = new Railway(*this, 25, "Kazan railway",
            200, Color::COL1);

    field[26] = new Street(*this, 26, "Georgian Val street",
            260, Color::COL6, 150);

    field[27] = new Street(*this, 27, "Tchaikovsky street",
            260, Color::COL6, 150);

    field[28] = new Utility(*this, 28, "Water supply",
            150, Color::COL1);

    field[29] = new Street(*this, 29, "Smolensk square",
            280, Color::COL6, 150);

    field[30] = new GoToPrison(*this, 30, "Go to prison");

    field[31] = new Street(*this, 31, "Shchusev street",
            300, Color::COL7, 200);

    field[32] = new Street(*this, 32, "Gogolevsky boulevard",
            300, Color::COL7, 200);

    field[33] = new PublicTreasury(*this, 33, "Public treasury");

    field[34] = new Street(*this, 34, "Kutuzovsky prospect",
            320, Color::COL7, 200);

    field[35] = new Railway(*this, 35, "Leningrad railway",
            200, Color::COL1);

    field[36] = new Chance(*this, 36, "Chance");

    field[37] = new Street(*this, 37, "Malaya Bronnaya",
            350, Color::COL8, 200);

    field[38] = new IncomeTax(*this, 38, "Income tax", 100);

    field[39] = new Street(*this, 39, "Arbat street",
            400, Color::COL8, 200);
}

PlayerData& Board::getPlayer(Token token) {
    for (auto &player : players) {
        if (player.token == token) {
            return player;
        }
    }
    //TODO: что вернуть если не нашли.
    throw 1;
}

const PlayerData &Board::getPlayer(Token token) const {
    for (auto &player : players) {
        if (player.token == token) {
            return player;
        }
    }
    //TODO: что вернуть если не нашли.
    throw 1;
}

Token Board::getPlayerToken(std::size_t index) const {
    return players[index].token;
}

PlayerReply Board::sendRequest(Token token, PlayerRequest request) const {
    return game.sendRequest(token, request);
}

void Board::sendMessage(Token token, PlayerMessage mes) const {
    game.sendMessage(token, mes);
}

std::size_t Board::getPlayersNumber() const {
    return players.size();
}

const std::vector<PlayerData> &Board::getPlayers() const {
    return players;
}

void Board::decrNumOfOlayers() {
    numOfAlivePlayers--;
}

bool Board::isFinished() const {
    return (numOfAlivePlayers <= 1);
}

int Board::getCurNumOfPlayers() const {
    return numOfAlivePlayers;
}

Token Board::getWinner() const {
    for (std::size_t i = 0; i < (*this).getPlayersNumber(); i++) {
        if (players[i].alive) {
            return players[i].token;
        }
    }
    //TODO: что вернуть если не нашли.
    throw 1;
}
