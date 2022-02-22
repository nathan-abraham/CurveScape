#pragma once

class ParametricGraph {
private:
    sf::Color pointColor;
    double t_coor;
    te_expr* x_evaluated;
    te_expr* y_evaluated;
    sf::Vector2f eval_exprs();
    int x_parseError;
    int y_parseError;

public:
    static sf::RenderWindow* window;
    static int numPoints;
    static int pointIncr;

    std::vector<sf::Vector2f> points;
    bool on;
    ParametricGraph(const char* x_expr, const char* y_expr, const char* x_name, const char* y_name, bool o, const sf::Color& pColor);
    ParametricGraph();
    ~ParametricGraph();
    void draw(sf::RenderWindow& window);
    void plot(int width, int rows, sf::Vector2f origin);
    void updatePoints();
    void parseExprs();
    bool shouldGraph();
    char x_expression[50];
    char y_expression[50];
    const char* x_name;
    const char* y_name;
    char x_checkName[60];
    char y_checkName[60];
};
