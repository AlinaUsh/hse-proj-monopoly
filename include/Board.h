#ifndef BOARD_H
#define BOARD_H

#include "MonopolyFwd.h"
#include "Cards.h"
#include "PlayerRequests.h"
#include <string_view>
#include <array>
#include <vector>

static constexpr int PRISON_POS = 10;

struct PlayerData {
    PlayerData(std::string name, Token token);
    PlayerData(const PlayerData&) = delete;
    void operator=(PlayerData) = delete;
    PlayerData(PlayerData&&) noexcept = default;
    PlayerData& operator=(PlayerData&&) noexcept = default;
    std::string name;
    Token token;
    uint32_t position = 0;
    uint32_t money = 1500; //TODO: Negative money?
    uint32_t doubleDice = 0;
    uint32_t daysLeftInPrison = 0;
    uint32_t numberOfRailways = 0;
    uint32_t numberOfUtilities = 0;
    uint32_t lastTrow = 0;
    bool prisoner = false;
    bool alive = true;
    std::vector<std::unique_ptr<Card>> cards;

    void newPosition(std::size_t throwSum);
    void toPrison();
    void outOfPrison();
};

class Board {
public:
    Board(const std::vector<std::pair<std::string_view, Token>>& players, Game& game);
    static constexpr int FIELD_SIZE = 40;
    std::array<FieldTile *, FIELD_SIZE> field; //TODO: raw pointer?
    CardPool deck;

    PlayerData& getPlayer(Token token);
    const PlayerData& getPlayer(Token token) const;
    Token getPlayerToken(std::size_t index) const;
    const std::vector<PlayerData>& getPlayers() const;
    PlayerReply sendRequest(Token token, PlayerRequest request) const;
    void sendMessage(Token token, PlayerMessage mes) const;
    std::size_t getPlayersNumber() const;

private:
    std::vector<PlayerData> players;
    Game& game;
};

#endif //BOARD_H
