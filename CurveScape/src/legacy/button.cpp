#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>

class Button {
    public:
    int x, y, width, height;
    sf::Text text;
    sf::Color color;
    sf::Color hoverColor;
    sf::Color currentColor;
    sf::Font font;
    sf::RectangleShape rect;
    bool selected = false;
    double(*func)(double);

    Button(int x, int y, int width, int height, std::string text, sf::Color color, sf::Color hoverColor, double(*func)(double)) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->color = color;
        this->hoverColor = hoverColor;
        currentColor = color;
        this->func = func;
        rect.setSize(sf::Vector2f(width, height));
        rect.setPosition(sf::Vector2f(x, y));
        this->text.setFont(font);
        this->text.setString(text);
        this->text.setCharacterSize(25);

        if (!font.loadFromFile("SourceCodePro.ttf")) {
            std::cout << "Error loading font file!" << std::endl;
        }
    }

    Button(int x, int y, int width, int height, std::string text, sf::Color color, sf::Color hoverColor) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->height = height;
        this->color = color;
        this->hoverColor = hoverColor;
        currentColor = color;
        rect.setSize(sf::Vector2f(width, height));
        rect.setPosition(sf::Vector2f(x, y));
        this->text.setFont(font);
        this->text.setString(text);
        this->text.setCharacterSize(25);

        if (!font.loadFromFile("SourceCodePro.ttf")) {
            std::cout << "Error loading font file!" << std::endl;
        }
    }
    
    void draw(sf::RenderWindow& window) {
        if (selected) {
            rect.setFillColor(sf::Color::Magenta);
        } else {
            rect.setFillColor(currentColor);
        }
        window.draw(rect);
        if (text.getString().toAnsiString().length() > 0) {
            sf::FloatRect textRect = text.getLocalBounds();
            text.setPosition(sf::Vector2f(x + (width/2 - textRect.width/2.0f), y + 0.9 * (height/2 - textRect.height/2.0f)));
            window.draw(text);
        }
    }

    bool isOver(sf::Vector2f pos) {
        if (pos.x > x && pos.x < x + width) {
            if (pos.y > y && pos.y < y + height) {
                currentColor = hoverColor;
                return true;
            }
        }
        currentColor = color;
        return false;
    }

    void toString() {
        std::cout << "x, y, width, height, text, color = " << x << ", " << y << ", " << width << ", " << height << ", " << text.getString().toAnsiString() << ", " << unsigned(currentColor.r) << unsigned(currentColor.g) << unsigned(currentColor.b)  << std::endl;
    }
};