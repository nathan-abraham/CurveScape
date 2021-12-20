#pragma once

#include <SFML/Graphics.hpp>
#include "tinyexpr\tinyexpr.h"

class Graph {
private:
    sf::Color pointColor;
    double x_coor;
    te_expr* evaluated;
    double eval_expr();
    int parseError;
    double findDistance(const sf::Vector2f&, const sf::Vector2f&);
    bool polar;

public:
    static float scaleFactor;
    static sf::RenderWindow* window;
    static int numPoints;
    static int pointIncr;

    std::vector<sf::Vector2f> points;
    bool on;
    Graph(const char* expr, const char* name, bool o, const sf::Color& pColor);
    Graph(const char* expr, const char* name, bool o, const sf::Color& pColor, const bool& polar);
    Graph();
    ~Graph();
    void draw(sf::RenderWindow& window);
    void plot(int width, int rows, sf::Vector2f origin);
    void updatePoints();
    void parseExpr();
    char expression[50];
    const char* name;
    char checkName[60];
};
