#include "Field.h"
#include "Board.h"

FieldTile::FieldTile(Board& board, int position, std::string name) :
                        board(board), position(position), name(std::move(name)){}

void FieldTile::onPlayerPass(Token) {
    return;
}

void FieldTile::onPlayerEntry(Token) {
    return;
}

void Start::onPlayerEntry(Token token) {
    PlayerData& player = board.getPlayer(token);
    player.money += 200;
}

void Start::onPlayerPass(Token token) {
    PlayerData& player = board.getPlayer(token);
    player.money += 200;
}

void Prison::onPlayerEntry(Token token) {
    if (!board.getPlayer(token).prisoner) {
        return;
    }
    if (board.sendRequest(token, PlayerRequest("Would you like to pay 50000 to left prison?")).answer) {
        board.getPlayer(token).money -= PRISON_FINE;
    }
    //ask to pay fine 50
    //ask to use special card
    return;
}