#pragma once
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include "button.cpp"
#include "functions.cpp"

class DropdownMenu {
    public:
    std::vector<Button> items;
    int x, y, width, height, numItems, buttonHeight;
    bool dropped = false;
    Button* first = NULL;

    DropdownMenu(int x, int y, int width, int buttonHeight, int numItems, const sf::Color& color, const sf::Color& hoverColor) {
        this->x = x;
        this->y = y;
        this->width = width;
        this->buttonHeight = buttonHeight;
        this->numItems = numItems;
        this->height = (numItems + 1) * buttonHeight;
        first = new Button(x, y, width, this->height, std::string("Select function"), sf::Color::Black, hoverColor);
    }
    ~DropdownMenu() {
        delete first; 
    }

    void addItem(Button b) {
        items.push_back(b);
    }

    void makeSelected(double(*function)(double)) {
        for (int i = 0; i < items.size(); ++i) {
            Button* ptr = &items[i];
            if (items[i].func == function) {
                ptr->selected = !ptr->selected;
            } else {
                ptr->selected = false;
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        first->draw(window);
        if (dropped) {
            for (int i = 0; i < items.size(); ++i) {
                Button* ptr = &items[i];
                ptr->draw(window);
            }
        } 
    }

    void setDropped(bool b) {
        dropped = b;
    }



};