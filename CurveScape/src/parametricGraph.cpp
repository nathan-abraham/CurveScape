#include <SFML/Graphics.hpp>
#include <imgui.h>

#include "tinyexpr/tinyexpr.h"
#include "parametricGraph.h"
#include "graph.h"
#include "LineShape.hpp"
#include "plotutil.h"
#include "constants.h"
#include "eval.h"

#define IMGUI_TOOLTIP 0

sf::RenderWindow* ParametricGraph::window = NULL;

ParametricGraph::ParametricGraph(const char* x_expr, const char* y_expr, const char* x_name, const char* y_name, bool o, const sf::Color& pColor) {
    strcpy_s(this->x_expression, 45, x_expr);
    strcpy_s(this->y_expression, 45, y_expr);

    this->x_name = x_name;
    this->y_name = y_name;
    strcpy_s(this->x_checkName, 10, "on##");
    strcat_s(this->x_checkName, 45, x_name);

    this->on = o;
    this->pointColor = pColor;
    this->t_coor = 0;
    this->parseExprs();
    this->points = std::vector<sf::Vector2f>(Graph::numPoints);
    updatePoints();
}

ParametricGraph::ParametricGraph() {}

ParametricGraph::~ParametricGraph() {
    te_free(this->x_evaluated);
    te_free(this->y_evaluated);
}

void ParametricGraph::draw(sf::RenderWindow& window) {
    sf::LineShape tempLine;
    tempLine.setFillColor(pointColor);
    tempLine.setThickness(4.0f);
    for (int i = 0; i < this->points.size() - 1; ++i) {
        if (std::isinf(points[i].x)) {
            continue;
        }
        tempLine.init(points[i], points[i + 1]);
        window.draw(tempLine);

        if (IMGUI_TOOLTIP) {
            if (tempLine.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
                sf::Vector2f temp(SFMLToCartesian(points[i], origin));
                std::string toolTip = std::to_string(temp.x / Graph::scaleFactor / (winWidth / ROWS)) + std::string(", ") +
                    std::to_string(temp.y / Graph::scaleFactor / (winWidth / ROWS));
                ImGui::SetTooltip(toolTip.c_str());
            }
        }
    }
}

void ParametricGraph::updatePoints() {
    this->points.resize(Graph::numPoints);
    if (this->x_evaluated && this->y_evaluated) {
        plot(winWidth, ROWS, origin);
    }
}

bool ParametricGraph::shouldGraph() {
    return this->on && this->x_evaluated && this->y_evaluated;
}

void ParametricGraph::parseExprs() {
    te_variable vars[] = {
        {"t", &this->t_coor}
    };

    this->x_evaluated = te_compile(clean(x_expression).c_str(), vars, 1, &this->x_parseError);
    this->y_evaluated = te_compile(clean(y_expression).c_str(), vars, 1, &this->y_parseError);
}

sf::Vector2f ParametricGraph::eval_exprs() {
    float x_result = NAN;
    float y_result = NAN;
    if (this->x_evaluated && this->y_evaluated) {
        x_result =  te_eval(this->x_evaluated);
        y_result =  te_eval(this->y_evaluated);
    }
    return sf::Vector2f(x_result, y_result);
}

void ParametricGraph::plot(int width, int rows, const sf::Vector2f origin) {
    double gap = width / rows * Graph::scaleFactor;
    int idx = 0;

    sf::Vector2f sfCurrentCenter = SFMLToCartesian(window->getView().getCenter(), origin);
    sf::Vector2f sfCurrentSize = SFMLToCartesian(window->getView().getSize(), origin);
    sf::Vector2f currentSize = window->getView().getSize();

    sf::CircleShape point(5, 4);
    point.setFillColor(pointColor);

    sf::Vector2f position;
    int radius = 3;

    float maxT = 10;
    float parametricIncr = maxT / Graph::numPoints;

    for (float i = 0; i < maxT; i += parametricIncr) {
        this->t_coor = i;
        sf::Vector2f result = eval_exprs();
        double x = result.x * gap;
        double y = result.y * gap;
        if (std::isinf(y) || std::isinf(x)) {
            this->points[idx] = sf::Vector2f(std::numeric_limits<float>::infinity(), 0.0f);
            idx++;
            continue;
        }
        position.x = x;
        position.y = y;
        if (idx < Graph::numPoints) {
            this->points[idx] = cartesianToSFML(position, origin);
            idx++;
        }
    }
}

