#pragma once
#include "graph.h"
#include "events/eventManager.h"

sf::Vector2f cartesianToSFML(const sf::Vector2f& original, const sf::Vector2f& origin);
sf::Vector2f SFMLToCartesian(const sf::Vector2f& original, const sf::Vector2f& origin);

sf::Vector2f multVec2(sf::Vector2f original, float scale);

std::vector<sf::CircleShape> plot(int width, int height, int rows, sf::Vector2f origin, int numPoints, sf::Color pointColor, double(*function)(double), double scaleFactor);

void drawGrid(sf::RenderWindow& window, const int& width, const int& height, const int& rows, const sf::Color& lineColor, const float& scaleFactor,
    const sf::Font& font, const sf::Color& bgColor);

void drawGrid2(sf::RenderWindow& window, const int& width, const int& height, const int& rows, const sf::Color& lineColor, const float& scaleFactor,
    const sf::Font& font, const sf::Color& bgColor);

void zoomViewAt(const sf::Vector2i& pixel, sf::RenderWindow& window, sf::View& view, float& scalef, const float& incr);

void zoomViewAt(sf::Vector2i pixel, sf::RenderWindow& window, sf::View& view, float zoom);

void take_screenshot(const sf::RenderWindow& window, const std::string& filename);


void mainMenuBar(EventManager& em, std::vector<Graph*>& graphs, std::string& current, std::string& imageFilename, bool& main, bool& panel, 
    bool& calc, bool& polarPanel, bool& screen, int& frame, bool& saveAsFlag, bool& saveFlag, bool& openFlag);

void menuFile(EventManager& em, std::vector<Graph*>& graphs, std::string& current, std::string& imageFilename, bool& main, bool& panel,
    bool& calc, bool& polarPanel, bool& screen, int& frame, bool& saveAsFlag, bool& saveFlag, bool& openFlag);
