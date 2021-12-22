#pragma once

class GraphicsManager {
public:
    sf::Clock deltaClock;
    sf::View view;
    sf::Font font;
    sf::Image icon;

    GraphicsManager(sf::RenderWindow&);
    ~GraphicsManager();
    void setImGuiStyles();
};
