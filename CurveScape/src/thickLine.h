#pragma once
#include <SFML/Graphics.hpp>
#include "plotutil.h"
#include "constants.h"

class ThickLine {
private:
    sf::Vector2f p1;
    sf::Vector2f p2;
    sf::RectangleShape line;

    sf::Vector2f linePos;
    sf::FloatRect lineRect;

    static const float vertOffset;

    bool isOver(sf::RenderWindow& window, const sf::Vector2i& pos);

public:
    ThickLine(const sf::Vector2f&, const sf::Vector2f&);
    ThickLine(const sf::Color&);
    void draw(sf::RenderWindow& window);
    void calc();
    void setCoor(const sf::Vector2f&, const sf::Vector2f&);
};
