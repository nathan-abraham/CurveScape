#include <ctype.h>
#include <iostream>
#include <string.h>

#include "graph.h"
#include "constants.h"
#include "plotutil.h"
#include "tinyexpr/tinyexpr.h"
#include "thickLine.h"
#include "eval.h"

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
}

Graph::Graph() {}

Graph::~Graph() {
    te_free(this->evaluated);
}

double Graph::findDistance(const sf::Vector2f& a, const sf::Vector2f& b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void Graph::draw(sf::RenderWindow& window) {
    ThickLine tempLine(pointColor);

    for (int i = 0; i < this->points.size() - 1; ++i) {
        if (std::isinf(points[i].x)) {
            continue;
        }
        tempLine.setCoor(points[i], points[i + 1]);
        tempLine.draw(window);
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

    //std::string temp = expression;

    //for (int i = 1; i < temp.size(); i++) {
    //	if ((isdigit(temp[i-1]) || temp[i-1] == 'x') && temp[i] == 'x') {
    //		temp.insert(i, 1, '*');
    //	}
    //}

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

    for (int i = sfCurrentCenter.x - currentSize.x / 2; i < sfCurrentCenter.x + currentSize.x / 2; i += Graph::pointIncr) {
        this->x_coor = i / gap;

        double y = eval_expr() * gap;
        if (std::isinf(y)) {
            //std::cout << "inf" << std::endl;
            this->points[idx] = sf::Vector2f(std::numeric_limits<float>::infinity(), 0.0f);
            idx++;
            continue;
        }
        position.x = i;
        position.y = y;
        if (idx < Graph::numPoints) {
            this->points[idx] = cartesianToSFML(position, origin);
            idx++;
        }
    }

    //  for (int i = -numPoints / 2; i < numPoints / 2; i += 1) {
    //      this->x_coor = i / gap;

          //double y = eval_expr() * gap;
          //if (y == NAN) {
          //	continue;
          //}
          //position.x = i;
          //position.y = y;
    //      points[idx] = cartesianToSFML(position, origin);
    //      idx++;
    //  }
}

