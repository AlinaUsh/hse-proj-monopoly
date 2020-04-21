#include "view/SFMLView.h"
#include "Manager.h"
#include "Field.h"
#include <view/VectorOps.h>
#include <iostream>
#include <cassert>

using Vec::transpose;

SFMLView::SFMLView(Manager &manager) : manager(manager) {
    window.create(sf::VideoMode(800, 600), "Monopoly");
    window.setFramerateLimit(60);
    mainFont.loadFromFile("Ubuntu-R.ttf");
    //
    Button btn2("1", { 30, 30 }, 30, sf::Color::Green, sf::Color::Black);
    btn1 = std::move(btn2);
    btn1.setFont(mainFont);
    btn1.setPosition({ 10, 30 });
    //
    events.addHandler<sf::Event::Closed>([this](){ window.close(); });
    events.addHandler<sf::Event::Resized>([this](auto e) { onResize(e); });
  //  events.addHandler<sf::Event::MouseButtonPressed>([this](sf::Event::MouseButtonEvent e){ tmp++; });
    events.addHandler<sf::Event::MouseButtonPressed>([this](sf::Event::MouseButtonEvent e){
        if (this->btn1.isMouseOver(window)) {
            std::cout << "Pressed " << "\n";
        }
    });
    btn1.drawTo(window);

    onResize({window.getSize().x, window.getSize().y});

    //TODO: temp
    manager.addPlayer(std::make_unique<LocalPlayer>(Token::DOG, "Player 1", *this));
    manager.addPlayer(std::make_unique<LocalPlayer>(Token::HAT, "Player 2", *this));
    manager.addPlayer(std::make_unique<LocalPlayer>(Token::BOOT, "Player 3", *this));
    manager.addPlayer(std::make_unique<LocalPlayer>(Token::CAR, "Player 4", *this));
    manager.addPlayer(std::make_unique<LocalPlayer>(Token::CAT, "Player 5", *this));
    manager.addPlayer(std::make_unique<LocalPlayer>(Token::SHIP, "Player 6", *this));
    manager.startGame();
}

SFMLView::~SFMLView() {
    windowMutex.lock();
    shouldClose = true;
    windowMutex.unlock();
}


void SFMLView::mainLoop() {
    while (window.isOpen()) {
        {
            std::lock_guard m(windowMutex);
            if(shouldClose) {
                window.close();
                break;
            }
        }
        sf::Event e{};
        while (window.pollEvent(e)) {
            events.handleEvent(e);
        }
        window.clear();
        draw();
        btn1.drawTo(window);
        window.display();
    }
}

bool doesFit(const sf::Text& t, float width) {
    return t.getLocalBounds().width < width;
}

void SFMLView::drawField(const Board& board) {
    for(const auto& fieldTile : board.field) {
        int i = fieldTile->position;
        const auto& viewTile = shapes.fieldRects[i];
        bool isSide = (i / 10) % 2 == 0;
        auto [x, y] = viewTile.getPosition();
        auto [w, h] = isSide ? viewTile.getSize() : transpose(viewTile.getSize());

        window.draw(viewTile);
        std::string s = fieldTile->name;
        sf::Text name(fieldTile->name, mainFont);
        int fsize = 11;
        name.setCharacterSize(fsize);
        while(!doesFit(name, h * 0.8f)) {
            auto lastSpace = s.rfind(' ');
            if(lastSpace == std::string::npos) break;
            s.replace(lastSpace, 1, "\n");
            name.setString(s);
        }
        while(doesFit(name, h * 0.7f)) {
            name.setCharacterSize(++fsize);
        }
        auto [nx, ny, nw, nh] = name.getLocalBounds();
        const float align = ((h - nw) / 2);
        const float shift = (w / 4);
        if(i % 10 == 0) {
            name.setOrigin(nx + nw / 2, ny + nh / 2);
            name.setPosition(x + h / 2, y + w / 2);
            name.rotate(isSide ? 45 : -45);
        } else {
            switch(i / 10) {
                case 0: name.rotate(+90); name.setPosition(x + w - shift, y + align); break;
                case 1: name.setPosition(x + align, y + h - shift - nh / 2); break;
                case 2: name.rotate(-90); name.setPosition(x + shift, y + h - align); break;
                case 3: name.setPosition(x + align, y + shift); break;
            }
        }

        window.draw(name);
    }
}

void moveTo(sf::Shape& s, const sf::RectangleShape& to) {
    auto [x, y] = to.getPosition();
    auto [w, h] = to.getSize();
    s.move(x + w / 2, y + h / 2);
}

void SFMLView::drawPlayers(const Board& board) {
    for(const auto& player : board.getPlayers()) {
        int i = tmp % 40;
        int row = i / 10;
        float k = 6;
        bool isSide = row % 2 == 0;
        auto& tile = shapes.fieldRects[i];
        auto [w, h] = isSide ? tile.getSize() : transpose(tile.getSize());
        sf::CircleShape p(w / 20);
        p.setOrigin(p.getRadius(), p.getRadius());
        switch(player.token) {

            case Token::DOG:
                p.setFillColor(sf::Color::White);
                break;
            case Token::HAT:
                p.setFillColor(sf::Color::Red);
                break;
            case Token::BOOT:
                p.setFillColor(sf::Color::Blue);
                break;
            case Token::CAT:
                p.setFillColor(sf::Color::Cyan);
                break;
            case Token::CAR:
                p.setFillColor(sf::Color::Magenta);
                break;
            case Token::SHIP:
                p.setFillColor(sf::Color::Green);
                break;
            case Token::FREE_FIELD:
                assert(false);
        }
        if(i % 10 == 0) {
            float a = w / k, b = a / 2, c = h / k, d = c / 2;
            switch (player.token) {
                case Token::DOG:
                    p.move(-a + b, c + d);
                    break;
                case Token::HAT:
                    p.move(a + b, -c + d);
                    break;
                case Token::BOOT:
                    p.move(a - b, -c - d);
                    break;
                case Token::CAT:
                    p.move(-a - b, c - d);
                    break;
                case Token::CAR:
                    p.move(a + b, -c - d);
                    break;
                case Token::SHIP:
                    p.move(-a - b, c + d);
                    break;
                case Token::FREE_FIELD:
                    assert(false);
            }
            auto [px, py] = p.getPosition();
            switch(row) {
                case 0: p.setPosition(px, py); break;
                case 1: p.setPosition(-px, py); break;
                case 2: p.setPosition(-px, -py); break;
                case 3: p.setPosition(px, -py); break;
            }
        } else {
            p.setPosition(-w / k, 0);
            switch (player.token) {
                case Token::DOG:
                    p.move(0, h / (2 * k));
                    break;
                case Token::HAT:
                    p.move(0, -h / (2 * k));
                    break;
                case Token::BOOT:
                    p.move(-w / k, -h / (2 * k));
                    break;
                case Token::CAT:
                    p.move(-w / k, h / (2 * k));
                    break;
                case Token::CAR:
                    p.move(-w / (2 * k), -h / k);
                    break;
                case Token::SHIP:
                    p.move(-w / (2 * k), h / k);
                    break;
                case Token::FREE_FIELD:
                    assert(false);
            }
            auto [px, py] = p.getPosition();
            switch(row) {
                case 0: p.setPosition(px, py); break;
                case 1: p.setPosition(py, px); break;
                case 2: p.setPosition(-px, -py); break;
                case 3: p.setPosition(-py, -px); break;
            }
        }
        moveTo(p, tile);
        window.draw(p);
    }
}

void SFMLView::draw() {
    const Board& board = manager.getBoard();
    drawField(board);
    drawPlayers(board);
}

void SFMLView::redraw(const Board &board) {
    //NO-OP
}

PlayerReply SFMLView::processRequest(Player &p, PlayerRequest req) {
    std::unique_lock m(requestMutex);
    assert(!curRequest.has_value());
    curRequest = std::move(req);
    while (!requestReply) {
        requestCond.wait(m);
    }


    auto rep = std::move(requestReply);
    return rep;
}

void SFMLView::processMessage(Player &p, PlayerMessage mes) {
    std::lock_guard m(requestMutex);
    curMessage = mes.message;
}

void SFMLView::onResize(sf::Event::SizeEvent e) {
    window.setView(sf::View(sf::FloatRect(0, 0, e.width, e.height)));
    const size_t side = (40 * e.width) / 800;
    const size_t longSide = (50 * e.width) / 800;
    const auto ss = static_cast<float>(side);
    const auto ls = static_cast<float>(longSide);
    const size_t longSideDiff = longSide - side;
    const size_t rowLen = 10;
    const size_t totalSide = side * rowLen + longSideDiff;
    const size_t totalLen = totalSide + longSide;
    const size_t left = (e.width - totalLen) / 2;
    const size_t bot = e.height - (e.height - totalLen) / 2 - longSide;
    const size_t right = left + totalSide;
    const size_t top = bot - totalSide;

    for (size_t i = 0; i < Board::FIELD_SIZE; i++) {
        auto &tile = shapes.fieldRects[i];
        tile.setFillColor(sf::Color::Transparent);
        tile.setOutlineColor(sf::Color::White);
        tile.setOutlineThickness(1);

        const size_t k = i % rowLen;
        if(k == 0) {
            tile.setSize({ls, ls});
            switch(i / rowLen) {
                case 0: tile.setPosition(left, bot); break;
                case 1: tile.setPosition(left, top); break;
                case 2: tile.setPosition(right, top); break;
                case 3: tile.setPosition(right, bot); break;
            }
        } else {
            const auto shift = static_cast<float>(side * k);
            switch(i / rowLen) {
                case 0: tile.setPosition(left, bot - shift); break;
                case 1: tile.setPosition(left + shift + longSideDiff, top); break;
                case 2: tile.setPosition(right, top + shift + longSideDiff); break;
                case 3: tile.setPosition(right - shift, bot); break;
            }
            if((i / rowLen) % 2 == 0) {
                tile.setSize({ls, ss});
            } else {
                tile.setSize({ss, ls});
            }
        }

    }
}
