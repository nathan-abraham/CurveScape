#include <ctype.h>
#include <iostream>
#include <string.h>

#include "graph.h"
#include "constants.h"
#include "plotutil.h"
#include "tinyexpr/tinyexpr.h"
#include "thickLine.h"
#include "eval.h"
#include "LineShape.hpp"

#define M_PI 3.14159265358979323846
#define LOG(x) std::cout << (x) << std::endl;

float Graph::scaleFactor = 2.0f;
sf::RenderWindow* Graph::window = NULL;
int Graph::numPoints;
int Graph::pointIncr;

Graph::Graph(const char* expr, const char* name, bool o, const sf::Color& pColor) {
    strcpy_s(this->expression, 45, expr);
    this->name = name;
    strcpy_s(this->checkName, 10, "on##");
    strcat_s(this->checkName, 45, name);
    this->on = o;
    this->pointColor = pColor;
    this->x_coor = 0;
    this->parseExpr();
    this->points = std::vector<sf::Vector2f>(Graph::numPoints);
    updatePoints();
    polar = false;
}

Graph::Graph(const char* expr, const char* name, bool o, const sf::Color& pColor, const bool& polar) {
    strcpy_s(this->expression, 45, expr);
    this->name = name;
    strcpy_s(this->checkName, 10, "on##");
    strcat_s(this->checkName, 45, name);
    this->on = o;
    this->pointColor = pColor;
    this->x_coor = 0;
    this->polar = polar;
    this->parseExpr();
    this->points = std::vector<sf::Vector2f>(Graph::numPoints);
    updatePoints();
}

Graph::Graph() {}

Graph::~Graph() {
    te_free(this->evaluated);
}

double Graph::findDistance(const sf::Vector2f& a, const sf::Vector2f& b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void Graph::draw(sf::RenderWindow& window) {
    //ThickLine tempLine(pointColor);

    for (int i = 0; i < this->points.size() - 1; ++i) {
        if (std::isinf(points[i].x)) {
            continue;
        }
        sf::LineShape tempLine(points[i], points[i + 1]);
        tempLine.setFillColor(pointColor);
        tempLine.setThickness(4.0f);
        window.draw(tempLine);
    }
}

void Graph::updatePoints() {
    this->points.resize(Graph::numPoints);
    if (this->evaluated) {
        this->on = true;
        plot(winWidth, ROWS, origin);
    }
    else {
        this->on = false;
    }
}

void Graph::parseExpr() {
    te_variable vars[] = {
        {"x", &this->x_coor}
    };

    this->evaluated = te_compile(clean(expression).c_str(), vars, 1, &this->parseError);
}

double Graph::eval_expr() {
    if (this->evaluated) {
        return te_eval(this->evaluated);
    }
    return NAN;
}

void Graph::plot(int width, int rows, const sf::Vector2f origin) {
    double gap = width / rows * Graph::scaleFactor;
    int idx = 0;

    sf::Vector2f sfCurrentCenter = SFMLToCartesian(window->getView().getCenter(), origin);
    sf::Vector2f sfCurrentSize = SFMLToCartesian(window->getView().getSize(), origin);
    sf::Vector2f currentSize = window->getView().getSize();

    sf::CircleShape point(5, 4);
    point.setFillColor(pointColor);

    sf::Vector2f position;
    int radius = 3;

    float polar_x = 0;
    float polarIncr = 6 * M_PI / Graph::numPoints;

    for (int i = sfCurrentCenter.x - currentSize.x / 2; i < sfCurrentCenter.x + currentSize.x / 2; i += Graph::pointIncr) {
        if (polar) {
            this->x_coor = polar_x;
            double y = eval_expr() * gap;
            position.x = y * cos(x_coor);
            position.y = y * sin(x_coor);
        }
        else {
            this->x_coor = i / gap;
            double y = eval_expr() * gap;
            if (std::isinf(y)) {
                this->points[idx] = sf::Vector2f(std::numeric_limits<float>::infinity(), 0.0f);
                idx++;
                continue;
            }
            position.x = i;
            position.y = y;
        }
        if (idx < Graph::numPoints) {
            this->points[idx] = cartesianToSFML(position, origin);
            idx++;
        }
        polar_x += polarIncr;
    }
}

