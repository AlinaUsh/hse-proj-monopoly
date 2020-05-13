#include "view/BoardModel.h"
#include "Field.h"
void PlayerModel::update(const PlayerData& player) {
    name = player.name;
    token = player.token;
    position = player.position;
    money = player.money;
    prisoner = player.prisoner;
    alive = player.alive;
}

void FieldTileModel::update(const FieldTile& tile) {
    position = tile.position;
    name = tile.name;
    if(auto ownable = dynamic_cast<const OwnableTile*>(&tile); ownable) {
        cost = ownable->cost;
        costOfParking = ownable->costOfParking;
        owner = ownable->owner;
        if(auto street = dynamic_cast<const Street*>(ownable); street) {
            numberOfHouses = street->numberOfHouses;
            costPerHouse = street->costPerHouse;
            color = street->color;
           // tax = street->tax;
           // isStreet = true;
        }
    } else if(auto prison = dynamic_cast<const Prison*>(&tile); prison) {
        costOfParking = prison->tax;
    } else if(auto incometax = dynamic_cast<const IncomeTax*>(&tile); incometax) {
        costOfParking = incometax->getTax();
    }
}

void BoardModel::update(const Board& board) {
    for (size_t i = 0; i < FIELD_SIZE; ++i) {
        field[i].update(*board.field[i]);
    }
    players.resize(board.getPlayers().size());
    for (size_t i = 0; i < players.size(); ++i) {
        players[i].update(board.getPlayers()[i]);
    }
    curPlayer = board.getPlayerToken(board.getPlayerIndex());
}
