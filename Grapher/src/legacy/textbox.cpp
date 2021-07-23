#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

class TextBox {
    public:
    int x, y, width, height, blinkCount = 0;
    bool focused = false, blinking = false;
    sf::Color color;
    sf::String input;
    sf::Text text;
    sf::Font font;
    sf::Clock clock;
    sf::RectangleShape cursor;

    TextBox(int x, int y, int width, int height, const sf::Color& color) {
        this->x = x;
        this-> y = y;
        this->width = width;
        this->height = height;
        this->color = color;
        text.setString("");
        text.setFont(font);

        if (!font.loadFromFile("SourceCodePro.ttf")) {
            std::cout << "Error loading font file!" << std::endl;
        }
    }

    void captureInput(sf::Event event) {
        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode != '\b') {
                input += event.text.unicode;
            } else {
                if (input.getSize() > 0)
                    input.erase(input.getSize() - 1);
            }
            text.setString(input);
        }
    }

    bool isOver(sf::Vector2f pos) {
        if (pos.x > x && pos.x < x + width) {
            if (pos.y > y && pos.y < y + height) {
                return true;
            }
        }
        return false;
    }

    void draw(sf::RenderWindow& window) {
        sf::RectangleShape rect;
        rect.setPosition(sf::Vector2f(x, y));
        rect.setSize(sf::Vector2f(width, height));
        rect.setFillColor(color);
        window.draw(rect);
        
        sf::FloatRect globalRect = text.getGlobalBounds();
        if (focused) {
            blinkCount++;
            if (text.getString().toAnsiString().length() < 1) {
                sf::Vector2f dfPosition(x + width / 2, y + height / 2 - height / 10);
                sf::Vector2f dfSize(5, 25);
                cursor.setPosition(dfPosition);
                cursor.setSize(dfSize);
            } else { 
                cursor.setPosition(sf::Vector2f(globalRect.left + globalRect.width + 10, globalRect.top));
                cursor.setSize(sf::Vector2f(5, globalRect.height));
            }
            float time = clock.getElapsedTime().asSeconds();
            if (abs(time - 0.7) <= 0.1) {
                blinking = !blinking;
                clock.restart();
            }

            cursor.setFillColor(blinking ? sf::Color::White : sf::Color::Green);
            window.draw(cursor);
        } else {
            clock.restart();
        }

        if (text.getString().toAnsiString().length() > 0) {
            sf::FloatRect textRect = text.getLocalBounds();
            text.setPosition(sf::Vector2f(x + (width/2 - textRect.width/2.0f), y + 0.8 * (height/2 - textRect.height/2.0f)));
            window.draw(text);
        }         
    }
};