#include <SFML/Graphics.hpp>
#include <nfd.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <array>
#include "constants.h"
#include "imgui.h"

#include "graph.h"
#include "fileOperations.h"
#include "events/eventManager.h"
#include "events/panelToggledEvent.h"
#include "stateManager.h"
#include "plotutil.h"
#include "LineShape.hpp"

#define LOG(x) std::cout << (x) << std::endl;
#define M_PI 3.14159265358979323846

float rtd(float r) {
    return r * 180 / M_PI;
}

float dtr(float r) {
    return r * M_PI / 180;
}

sf::Vector2f cartesianToSFML(const sf::Vector2f& original, const sf::Vector2f& origin) {
    sf::Vector2f temp(original.x + origin.x, origin.y - original.y);
    return temp;
}

sf::Vector2f SFMLToCartesian(const sf::Vector2f& original, const sf::Vector2f& origin) {
    sf::Vector2f temp(original.x - origin.x, origin.y - original.y);
    return temp;
}

sf::Vector2f multVec2(sf::Vector2f original, float scale) {
    original.x *= scale;
    original.y *= scale;
    return original;
}

class GridLine {
public:
    std::array<sf::Vertex, 2> line;
    bool major;

    GridLine(const std::array<sf::Vertex, 2>& li, const bool& ma) {
        line = li;
        major = ma;
        if (major) {
            //line[0].color = sf::Color::Red;
            //line[1].color = sf::Color::Red;
        }
        else {
            sf::Color tempColor = line[0].color;
            line[0].color = sf::Color(tempColor.r, tempColor.g, tempColor.b, 100);
            line[1].color = sf::Color(tempColor.r, tempColor.g, tempColor.b, 100);
        }
    }

    void draw(sf::RenderWindow& window) {
        sf::Vertex temp[] = { line[0], line[1] };
        window.draw(temp, 2, sf::Lines);
    }
};

class GridLabel {
public:
    sf::Text text;
    sf::RectangleShape back;

    GridLabel(const sf::Text& t, const sf::RectangleShape& b) {
        text = t;
        back = b;
    }

    void draw(sf::RenderWindow& window) {
        window.draw(back);
        window.draw(text);
    }
};

double wonkyRound(float num) {
    std::vector<double> benchmarks;
    for (int i = -5; i < 6; ++i) {
        benchmarks.push_back(2 * pow(10, i));
        benchmarks.push_back(5 * pow(10, i));
        benchmarks.push_back(10 * pow(10, i));
    }
    double minDiff = 100000;
    double res = benchmarks[0];
    for (double b : benchmarks) {
        if (abs(num - b) < minDiff) {
            minDiff = abs(num - b);
            res = b;
        }
    }
    return res;
}

int countDigits(int num) {
    int count = 0;
    while (num != 0) {
        num /= 10;
        count++;
    }
    return count;
}

inline float myRound(float val)
{
    if (val < 0) return ceil(val - 0.5);
    return floor(val + 0.5);
}

float roundDecimal(float val, int places) {
    int scale = (int) pow(10, places);
    return myRound(val * scale) / scale;
}


void drawHorizontal(sf::RenderWindow& window, const float& endVal, const int& width, const float& scaleFactor, const sf::Vector2f& sfCurrentCenter,
    const sf::Vector2f& sfCurrentSize, const sf::Color& lineColor, const sf::Font& font, const bool& addFlag) {

    int major_size = width / 12;
    std::vector<GridLine> majorLines;
    std::vector<GridLabel> majorLabels;
    int idx = 0;
    int major_idx = 0;
    int cur_major_size = major_size;

    double offset = wonkyRound(scaleFactor);
    double incr = (float) gap / offset;

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(12);
    text.setFillColor(lineColor);

    if (addFlag) {
        for (float i = 0; i < endVal; i += incr) {
            bool major = false;
            int quant = i * scaleFactor;

            if (i == 0) {
                major = true;
            }
            if (major_idx == 0 && abs(quant) > major_size - 10) {
                major = true;
                major_idx = idx;
                cur_major_size = abs(quant);
            }
            else if (major_idx && idx % major_idx == 0) {
                major = true;
            }

            if (major) {
                std::array<sf::Vertex, 2> line = {
                    sf::Vertex(cartesianToSFML(sf::Vector2f(i * scaleFactor, sfCurrentCenter.y - sfCurrentSize.y), origin), lineColor),
                    sf::Vertex(cartesianToSFML(sf::Vector2f(i * scaleFactor, sfCurrentCenter.y + sfCurrentSize.y), origin), lineColor),
                };
                majorLines.push_back(GridLine(line, major));
            }

            if (i != 0 and major) {
                float labelNum = roundDecimal((float)i / gap, countDigits(offset));
                if (labelNum - floor(labelNum) < 0.001) {
                    text.setString(std::to_string((int)labelNum));
                }
                else {
                    std::stringstream format;
                    format << std::fixed << std::setprecision(countDigits(offset)) << labelNum;
                    text.setString(format.str());
                }
                sf::FloatRect textRect = text.getGlobalBounds();
                text.setPosition(cartesianToSFML(sf::Vector2f(i * scaleFactor - textRect.width / 2.0f, -3), origin));
                sf::RectangleShape back(sf::Vector2f(textRect.width, textRect.height + 5));
                back.setPosition(text.getPosition());
                back.setFillColor(bgColor);

                majorLabels.push_back(GridLabel(text, back));
            }
            idx++;
        }
    }
    else {
        for (float i = 0; i > endVal; i -= incr) {
            bool major = false;
            int quant = i * scaleFactor;

            if (i == 0) {
                major = true;
            }
            if (major_idx == 0 && abs(quant) > major_size - 10) {
                major = true;
                major_idx = idx;
                cur_major_size = abs(quant);
            }
            else if (major_idx && idx % major_idx == 0) {
                major = true;
            }

            if (major) {
                std::array<sf::Vertex, 2> line = {
                    sf::Vertex(cartesianToSFML(sf::Vector2f(i * scaleFactor, sfCurrentCenter.y - sfCurrentSize.y), origin), lineColor),
                    sf::Vertex(cartesianToSFML(sf::Vector2f(i * scaleFactor, sfCurrentCenter.y + sfCurrentSize.y), origin), lineColor),
                };
                majorLines.push_back(GridLine(line, major));
            }

            if (i != 0 and major) {
                float labelNum = roundDecimal((float)i / gap, countDigits(offset));
                if (labelNum - floor(labelNum) < 0.001) {
                    text.setString(std::to_string((int)labelNum));
                }
                else {
                    std::stringstream format;
                    format << std::fixed << std::setprecision(countDigits(offset)) << labelNum;
                    text.setString(format.str());
                }
                sf::FloatRect textRect = text.getGlobalBounds();
                text.setPosition(cartesianToSFML(sf::Vector2f(i * scaleFactor - textRect.width / 2.0f, -3), origin));
                sf::RectangleShape back(sf::Vector2f(textRect.width, textRect.height + 5));
                back.setPosition(text.getPosition());
                back.setFillColor(bgColor);

                majorLabels.push_back(GridLabel(text, back));
            }
            idx++;
        }
    }

    std::vector<GridLine> minorLines;

    for (int i = 1; i < majorLines.size(); ++i) {
        float startX = majorLines[i-1].line[0].position.x;
        float endX = majorLines[i].line[0].position.x;
        float minorGap = (endX - startX) / 5;
        for (int j = 1; j < 5; ++j) {
            std::array<sf::Vertex, 2> tempMinorLine = {
                sf::Vertex(sf::Vector2f(startX + j * minorGap, origin.y - sfCurrentCenter.y + sfCurrentSize.y), lineColor),
                sf::Vertex(sf::Vector2f(startX + j * minorGap, origin.y - sfCurrentCenter.y - sfCurrentSize.y), lineColor),
            };
            minorLines.push_back(GridLine(tempMinorLine, false));
        }
        if (i == majorLines.size() - 1) {
            startX = endX;
            for (int j = 1; j < 5; ++j) {
                std::array<sf::Vertex, 2> tempMinorLine = {
                    sf::Vertex(sf::Vector2f(startX + j * minorGap, origin.y - sfCurrentCenter.y + sfCurrentSize.y), lineColor),
                    sf::Vertex(sf::Vector2f(startX + j * minorGap, origin.y - sfCurrentCenter.y - sfCurrentSize.y), lineColor),
                };
                minorLines.push_back(GridLine(tempMinorLine, false));
            }
        }
    }

    for (int i = 0; i < majorLines.size(); ++i) {
        majorLines[i].draw(window);
    }

    for (int i = 0; i < minorLines.size(); ++i) {
        minorLines[i].draw(window);
    }

    for (int i = 0; i < majorLabels.size(); ++i) {
        majorLabels[i].draw(window);
    }
}

void drawVertical(sf::RenderWindow& window, const float& endVal, const int& width, const float& scaleFactor, const sf::Vector2f& sfCurrentCenter,
    const sf::Vector2f& sfCurrentSize, const sf::Color& lineColor, const sf::Font& font, const bool& addFlag) {

    int major_size = width / 12;
    std::vector<GridLine> majorLines;
    std::vector<GridLabel> majorLabels;
    int idx = 0;
    int major_idx = 0;
    int cur_major_size = major_size;

    double incr = gap;
    double offset = wonkyRound(scaleFactor);
    incr = (float) gap / offset;

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(12);
    text.setFillColor(lineColor);

    if (addFlag) {
        for (float i = 0; i < endVal; i += incr) {
            bool major = false;
            int quant = i * scaleFactor;

            if (i == 0) {
                major = true;
            }
            if (major_idx == 0 && abs(quant) > major_size - 10) {
                major = true;
                major_idx = idx;
                cur_major_size = abs(quant);
            }
            else if (major_idx && idx % major_idx == 0) {
                major = true;
            }

            if (major) {
                std::array<sf::Vertex, 2> line = {
                    sf::Vertex(cartesianToSFML(sf::Vector2f(sfCurrentCenter.x + sfCurrentSize.x, i * scaleFactor), origin), lineColor),
                    sf::Vertex(cartesianToSFML(sf::Vector2f(sfCurrentCenter.x - sfCurrentSize.x, i * scaleFactor), origin), lineColor),
                };
                majorLines.push_back(GridLine(line, major));
            }

            if (i != 0 and major) {
                float labelNum = roundDecimal((float)i / gap, countDigits(offset));
                if (labelNum - floor(labelNum) < 0.001) {
                    text.setString(std::to_string((int)labelNum));
                }
                else {
                    std::stringstream format;
                    format << std::fixed << std::setprecision(countDigits(offset)) << labelNum;
                    text.setString(format.str());
                }
                sf::FloatRect textRect = text.getGlobalBounds();
                text.setPosition(cartesianToSFML(sf::Vector2f(-textRect.width - 6, i * scaleFactor + textRect.height / 1.25f), origin));
                sf::RectangleShape back(sf::Vector2f(textRect.width, textRect.height + 5));
                back.setPosition(text.getPosition());
                back.setFillColor(bgColor);

                majorLabels.push_back(GridLabel(text, back));
            }
            idx++;
        }
    }
    else {
        for (float i = 0; i > endVal; i -= incr) {
            bool major = false;
            int quant = i * scaleFactor;

            if (i == 0) {
                major = true;
            }
            if (major_idx == 0 && abs(quant) > major_size - 10) {
                major = true;
                major_idx = idx;
                cur_major_size = abs(quant);
            }
            else if (major_idx && idx % major_idx == 0) {
                major = true;
            }

            if (major) {
                std::array<sf::Vertex, 2> line = {
                    sf::Vertex(cartesianToSFML(sf::Vector2f(sfCurrentCenter.x + sfCurrentSize.x, i * scaleFactor), origin), lineColor),
                    sf::Vertex(cartesianToSFML(sf::Vector2f(sfCurrentCenter.x - sfCurrentSize.x, i * scaleFactor), origin), lineColor),
                };
                majorLines.push_back(GridLine(line, major));
            }

            if (i != 0 and major) {
                float labelNum = roundDecimal((float)i / gap, countDigits(offset));
                if (labelNum - floor(labelNum) < 0.001) {
                    text.setString(std::to_string((int)labelNum));
                }
                else {
                    std::stringstream format;
                    format << std::fixed << std::setprecision(countDigits(offset)) << labelNum;
                    text.setString(format.str());
                }
                sf::FloatRect textRect = text.getGlobalBounds();
                text.setPosition(cartesianToSFML(sf::Vector2f(-textRect.width - 6, i * scaleFactor + textRect.height / 1.25f), origin));
                sf::RectangleShape back(sf::Vector2f(textRect.width, textRect.height + 5));
                back.setPosition(text.getPosition());
                back.setFillColor(bgColor);

                majorLabels.push_back(GridLabel(text, back));
            }
            idx++;
        }
    }

    std::vector<GridLine> minorLines;

    for (int i = 1; i < majorLines.size(); ++i) {
        float startY = majorLines[i-1].line[0].position.y;
        float endY = majorLines[i].line[0].position.y;
        float minorGap = (endY - startY) / 5;
        for (int j = 1; j < 5; ++j) {
            std::array<sf::Vertex, 2> tempMinorLine = {
                sf::Vertex(sf::Vector2f(origin.x + sfCurrentCenter.x + sfCurrentSize.x, startY + j * minorGap), lineColor),
                sf::Vertex(sf::Vector2f(origin.x + sfCurrentCenter.x - sfCurrentSize.x, startY + j * minorGap), lineColor),
            };
            minorLines.push_back(GridLine(tempMinorLine, false));
        }
        if (i == majorLines.size() - 1) {
            startY = endY;
            for (int j = 1; j < 5; ++j) {
                std::array<sf::Vertex, 2> tempMinorLine = {
                    sf::Vertex(sf::Vector2f(origin.x + sfCurrentCenter.x + sfCurrentSize.x, startY + j * minorGap), lineColor),
                    sf::Vertex(sf::Vector2f(origin.x + sfCurrentCenter.x - sfCurrentSize.x, startY + j * minorGap), lineColor),
                };
                minorLines.push_back(GridLine(tempMinorLine, false));
            }
        }
    }

    for (int i = 0; i < majorLines.size(); ++i) {
        majorLines[i].draw(window);
    }

    for (int i = 0; i < minorLines.size(); ++i) {
        minorLines[i].draw(window);
    }

    for (int i = 0; i < majorLabels.size(); ++i) {
        majorLabels[i].draw(window);
    }
}

void drawGrid(sf::RenderWindow& window, const int& width, const int& height, const int& rows, const sf::Color& lineColor, const float& scaleFactor,
    const sf::Font& font, const sf::Color& bgColor) {

    const int gap = width / rows;

    sf::Vector2f sfCurrentCenter = SFMLToCartesian(window.getView().getCenter(), origin);
    sf::Vector2f sfCurrentSize = SFMLToCartesian(window.getView().getSize(), origin);

    sf::Vector2f currentCenter = window.getView().getCenter();
    sf::Vector2f currentSize = window.getView().getSize();

    sf::RectangleShape thickLine(sf::Vector2f(currentSize.x, height / 160));
    thickLine.setPosition(currentCenter.x - currentSize.x / 2, height / 2 - height / 320);
    thickLine.setFillColor(lineColor);
    window.draw(thickLine);

    thickLine = sf::RectangleShape(sf::Vector2f(width / 160, currentSize.y));
    thickLine.setPosition(width / 2 - width / 320, currentCenter.y - currentSize.y / 2);
    thickLine.setFillColor(lineColor);
    window.draw(thickLine);

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(12);
    text.setFillColor(lineColor);

    text.setString("0");
    text.setPosition(cartesianToSFML(sf::Vector2f(-15, -3), origin));
    window.draw(text);

    drawHorizontal(window, (sfCurrentCenter.x + sfCurrentSize.x) / scaleFactor, width, scaleFactor, sfCurrentCenter, sfCurrentSize,
        lineColor, font, true);
    drawHorizontal(window, (sfCurrentCenter.x - sfCurrentSize.x) / scaleFactor, width, scaleFactor, sfCurrentCenter, sfCurrentSize,
        lineColor, font, false);

    drawVertical(window, (sfCurrentCenter.y - sfCurrentSize.y) / scaleFactor, width, scaleFactor, sfCurrentCenter, sfCurrentSize,
        lineColor, font, true);

    drawVertical(window, (sfCurrentCenter.y + sfCurrentSize.y) / scaleFactor, width, scaleFactor, sfCurrentCenter, sfCurrentSize,
        lineColor, font, false);
}

void drawPolarGrid(sf::RenderWindow& window, const int& width, const int& height, const int& rows, const sf::Color& lineColor, const float& scaleFactor,
    const sf::Font& font, const sf::Color& bgColor) {

    const int gap = width / rows;

    sf::Vector2f sfCurrentCenter = SFMLToCartesian(window.getView().getCenter(), origin);
    sf::Vector2f sfCurrentSize = SFMLToCartesian(window.getView().getSize(), origin);

    sf::Vector2f currentCenter = window.getView().getCenter();
    sf::Vector2f currentSize = window.getView().getSize();

    sf::RectangleShape thickLine(sf::Vector2f(currentSize.x, height / 160));
    thickLine.setPosition(currentCenter.x - currentSize.x / 2, height / 2 - height / 320);
    thickLine.setFillColor(lineColor);
    window.draw(thickLine);

    thickLine = sf::RectangleShape(sf::Vector2f(width / 160, currentSize.y));
    thickLine.setPosition(width / 2 - width / 320, currentCenter.y - currentSize.y / 2);
    thickLine.setFillColor(lineColor);
    window.draw(thickLine);

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(12);
    text.setFillColor(lineColor);

    text.setString("0");
    text.setPosition(cartesianToSFML(sf::Vector2f(-15, -3), origin));
    window.draw(text);

    double incr = gap;
    double offset = wonkyRound(scaleFactor);
    incr = (float) gap / offset;

    constexpr float lineThickness = 2;
    sf::CircleShape tempCircle;
    tempCircle.setOutlineThickness(lineThickness);
    tempCircle.setOutlineColor(lineColor);
    tempCircle.setFillColor(sf::Color(0, 0, 0, 0));
    tempCircle.setPointCount(100);

    float endVal = std::max((sfCurrentCenter.x + sfCurrentSize.x) / scaleFactor, abs((sfCurrentCenter.x - sfCurrentSize.x) / scaleFactor));
    endVal = std::max(endVal, abs((sfCurrentCenter.y - sfCurrentSize.y) / scaleFactor));
    endVal = 2 * std::max(endVal, abs((sfCurrentCenter.y + sfCurrentSize.y) / scaleFactor));

    for (float i = 0; i < endVal; i += incr) {
        float quant = i * scaleFactor;

        float radius = i * scaleFactor - lineThickness / 2;
        tempCircle.setRadius(radius);
        tempCircle.setPosition(sf::Vector2f(origin.x - radius, origin.y - radius));
        window.draw(tempCircle);

        if (i != 0) {
            float labelNum = roundDecimal((float)i / gap, countDigits(offset));
            if (labelNum - floor(labelNum) < 0.001) {
                text.setString(std::to_string((int)labelNum));
            }
            else {
                std::stringstream format;
                format << std::fixed << std::setprecision(countDigits(offset)) << labelNum;
                text.setString(format.str());
            }
            sf::FloatRect textRect = text.getGlobalBounds();
            text.setPosition(cartesianToSFML(sf::Vector2f(i * scaleFactor - textRect.width / 2.0f, -3), origin));
            sf::RectangleShape back(sf::Vector2f(textRect.width, textRect.height + 5));
            back.setPosition(text.getPosition());
            back.setFillColor(bgColor);

            window.draw(back);
            window.draw(text);
        }
    }

    std::vector<std::array<sf::Vertex, 2>> diagLines;
    std::array<float, 8> degrees = { 30, 60, 120, 150, 210, 240, 300, 330 };

    float rightDist = (sfCurrentCenter.x + sfCurrentSize.x) / scaleFactor;
    float leftDist = (sfCurrentCenter.x - sfCurrentSize.x) / scaleFactor;
    float upDist = (sfCurrentCenter.y + sfCurrentSize.y) / scaleFactor;
    float downDist = (sfCurrentCenter.y - sfCurrentSize.y) / scaleFactor;

    for (float degree : degrees) {
        sf::LineShape line(
            cartesianToSFML(sf::Vector2f(rightDist * scaleFactor, scaleFactor * rightDist * tan(dtr(degree))), origin),
            cartesianToSFML(sf::Vector2f(leftDist * scaleFactor, scaleFactor * leftDist * tan(dtr(degree))), origin)
        );
        line.setThickness(lineThickness);
        line.setFillColor(lineColor);
        window.draw(line);
    }
}

void zoomViewAt(const sf::Vector2i& pixel, sf::RenderWindow& window, sf::View& view, float& scalef, const float& incr)
{
    const sf::Vector2f beforeCoord(SFMLToCartesian(window.mapPixelToCoords(pixel), origin));
    scalef *= (1 + incr);
    const sf::Vector2f afterCoord(beforeCoord * scalef / (scalef / (1 + incr)));

    const sf::Vector2f offsetCoord(afterCoord - beforeCoord);
    view.move(offsetCoord.x, -offsetCoord.y);
    window.setView(view);
}

void zoomViewAt(sf::Vector2i pixel, sf::RenderWindow& window, sf::View& view, float zoom)
{
    const sf::Vector2f beforeCoord{ window.mapPixelToCoords(pixel) };
    view.zoom(zoom);
    window.setView(view);
    const sf::Vector2f afterCoord{ window.mapPixelToCoords(pixel) };
    const sf::Vector2f offsetCoords{ beforeCoord - afterCoord };
    view.move(offsetCoords);
    window.setView(view);
}

void take_screenshot(const sf::RenderWindow& window, const std::string& filename)
{
    sf::Texture texture;
    texture.create(window.getSize().x, window.getSize().y);
    texture.update(window);
    if (texture.copyToImage().saveToFile(filename))
    {
        std::cout << "Screenshot saved to " << filename << std::endl;
    }
    else
    {
        std::cout << "Error saving image" << std::endl;
    }
}

void mainMenuBar(EventManager& em, std::vector<Graph*>& graphs, std::string& current, std::string& imageFilename, bool& main, bool& panel,
    bool& calc, bool& polarPanel, bool& screen, int& frame, bool& saveAsFlag, bool& saveFlag, bool& openFlag)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            menuFile(em, graphs, current, imageFilename, main, panel, calc, polarPanel, screen, frame, saveAsFlag, saveFlag, openFlag);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z", false, !em.undo.empty())) {
                em.undo_event();
            }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, !em.redo.empty())) {
                em.redo_event();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Panels")) {
            if (ImGui::MenuItem("Graphing Panel", NULL, panel)) {
                panel = !panel;
                polarPanel = false;
                em.addEvent(new PanelToggledEvent(&panel));
            }
            if (ImGui::MenuItem("Calculation Panel", NULL, calc)) {
                calc = !calc;
                em.addEvent(new PanelToggledEvent(&calc));
            }
            if (ImGui::MenuItem("Polar Graphing Panel", NULL, polarPanel)) {
                polarPanel = !polarPanel;
                panel = false;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void mainMenuBar(EventManager& em, std::vector<Graph*>& graphs, StateManager& sm)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            menuFile(em, graphs, sm);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z", false, !em.undo.empty())) {
                em.undo_event();
            }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, !em.redo.empty())) {
                em.redo_event();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Panels")) {
            if (ImGui::MenuItem("Graphing Panel", NULL, sm.panelActive)) {
                sm.panelActive = !sm.panelActive;
                sm.polarPanelActive = false;
                em.addEvent(new PanelToggledEvent(&sm.panelActive));
            }
            if (ImGui::MenuItem("Calculation Panel", NULL, sm.calcActive)) {
                sm.calcActive = !sm.calcActive;
                em.addEvent(new PanelToggledEvent(&sm.calcActive));
            }
            if (ImGui::MenuItem("Polar Graphing Panel", NULL, sm.polarPanelActive)) {
                sm.polarPanelActive = !sm.polarPanelActive;
                sm.panelActive = false;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void menuFile(EventManager& em, std::vector<Graph*>& graphs, std::string& current, std::string& imageFilename, bool& main, bool& panel,
    bool& calc, bool& polarPanel, bool& screen, int& frame, bool& saveAsFlag, bool& saveFlag, bool& openFlag)
{
    if (ImGui::MenuItem("Open", "Ctrl+O")) {
        nfdchar_t* rawFilename;
        nfdresult_t result = NFD_OpenDialog("graph", NULL, &rawFilename);

        if (result == NFD_OKAY) {
            loadGraphs(graphs, rawFilename);
            current = rawFilename;
        }
        else {
            LOG(NFD_GetError());
        }
    }
    if (ImGui::BeginMenu("Open Recent"))
    {
        ImGui::MenuItem("fish_hat.c");
        ImGui::MenuItem("fish_hat.inl");
        ImGui::MenuItem("fish_hat.h");
        if (ImGui::BeginMenu("More.."))
        {
            ImGui::MenuItem("Hello");
            ImGui::MenuItem("Sailor");
            if (ImGui::BeginMenu("Recurse.."))
            {
                menuFile(em, graphs, current, imageFilename, main, panel, calc, polarPanel, screen, frame, saveAsFlag, saveFlag, openFlag);
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Save", "Ctrl+S")) {
        if (current == "" || current.find(".png") != std::string::npos
            || current.find(".jpg") != std::string::npos) {
            saveAsFlag = true;
        }
        else {
            saveAsFlag = false;
            saveGraphs(graphs, current.c_str());
        }
    }
    if (ImGui::MenuItem("Save As...") || saveAsFlag) {
        nfdchar_t* rawFilename;
        nfdresult_t result = NFD_SaveDialog("png;jpg;graph", NULL, &rawFilename);

        if (result == NFD_OKAY) {
            current = rawFilename;
            if (current.find(".graph") != std::string::npos) {
                saveGraphs(graphs, rawFilename);
            }
            else {
                imageFilename = rawFilename;

                if (imageFilename.substr(imageFilename.size() - 4).find(".") == std::string::npos) {
                    LOG("added extension")
                        imageFilename += ".png";
                }
                main = false;
                panel = false;
                calc = false;
                screen = true;
                frame = 0;
            }
            saveAsFlag = false;
        }
        else if (result == NFD_CANCEL) {
            saveAsFlag = false;
            LOG("canceled saving")
        }
        else {
            LOG(NFD_GetError());
        }

    }

    ImGui::Separator();
    if (ImGui::BeginMenu("Options"))
    {
        static bool enabled = true;
        ImGui::MenuItem("Enabled", "", &enabled);
        ImGui::BeginChild("child", ImVec2(0, 60), true);
        for (int i = 0; i < 10; i++)
            ImGui::Text("Scrolling Text %d", i);
        ImGui::EndChild();
        static float f = 0.5f;
        static int n = 0;
        ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
        ImGui::InputFloat("Input", &f, 0.1f);
        ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Colors"))
    {
        float sz = ImGui::GetTextLineHeight();
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
            ImGui::Dummy(ImVec2(sz, sz));
            ImGui::SameLine();
            ImGui::MenuItem(name);
        }
        ImGui::EndMenu();
    }

    // Here we demonstrate appending again to the "Options" menu (which we already created above)
    // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
    // In a real code-base using it would make senses to use this feature from very different code locations.
    if (ImGui::BeginMenu("Options")) // <-- Append!
    {
        static bool b = true;
        ImGui::Checkbox("SomeOption", &b);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (ImGui::MenuItem("Checked", NULL, true)) {}
    if (ImGui::MenuItem("Quit", "Alt+F4")) {
        exit(EXIT_SUCCESS);
    }
}

void menuFile(EventManager& em, std::vector<Graph*>& graphs, StateManager& sm)
{
    if (ImGui::MenuItem("Open", "Ctrl+O")) {
        nfdchar_t* rawFilename;
        nfdresult_t result = NFD_OpenDialog("graph", NULL, &rawFilename);

        if (result == NFD_OKAY) {
            loadGraphs(graphs, rawFilename);
            sm.currentFileOpen = rawFilename;
        }
        else {
            LOG(NFD_GetError());
        }
    }
    if (ImGui::BeginMenu("Open Recent"))
    {
        ImGui::MenuItem("fish_hat.c");
        ImGui::MenuItem("fish_hat.inl");
        ImGui::MenuItem("fish_hat.h");
        if (ImGui::BeginMenu("More.."))
        {
            ImGui::MenuItem("Hello");
            ImGui::MenuItem("Sailor");
            if (ImGui::BeginMenu("Recurse.."))
            {
                menuFile(em, graphs, sm);
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Save", "Ctrl+S")) {
        if (sm.currentFileOpen == "" || sm.currentFileOpen.find(".png") != std::string::npos
            || sm.currentFileOpen.find(".jpg") != std::string::npos) {
            sm.saveAsFlag = true;
        }
        else {
            sm.saveAsFlag = false;
            saveGraphs(graphs, sm.currentFileOpen.c_str());
        }
    }
    if (ImGui::MenuItem("Save As...") || sm.saveAsFlag) {
        nfdchar_t* rawFilename;
        nfdresult_t result = NFD_SaveDialog("png;jpg;graph", NULL, &rawFilename);

        if (result == NFD_OKAY) {
            sm.currentFileOpen = rawFilename;
            if (sm.currentFileOpen.find(".graph") != std::string::npos) {
                saveGraphs(graphs, rawFilename);
            }
            else {
                sm.imageFilename = rawFilename;

                if (sm.imageFilename.substr(sm.imageFilename.size() - 4).find(".") == std::string::npos) {
                    LOG("added extension")
                    sm.imageFilename += ".png";
                }
                sm.mainActive = false;
                sm.panelActive = false;
                sm.calcActive = false;
                sm.screenshotFlag = true;
                sm.frameDelay = 0;
            }
            sm.saveAsFlag = false;
        }
        else if (result == NFD_CANCEL) {
            sm.saveAsFlag = false;
            LOG("canceled saving")
        }
        else {
            LOG(NFD_GetError());
        }

    }

    ImGui::Separator();
    if (ImGui::BeginMenu("Options"))
    {
        static bool enabled = true;
        ImGui::MenuItem("Enabled", "", &enabled);
        ImGui::BeginChild("child", ImVec2(0, 60), true);
        for (int i = 0; i < 10; i++)
            ImGui::Text("Scrolling Text %d", i);
        ImGui::EndChild();
        static float f = 0.5f;
        static int n = 0;
        ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
        ImGui::InputFloat("Input", &f, 0.1f);
        ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Colors"))
    {
        float sz = ImGui::GetTextLineHeight();
        for (int i = 0; i < ImGuiCol_COUNT; i++)
        {
            const char* name = ImGui::GetStyleColorName((ImGuiCol)i);
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
            ImGui::Dummy(ImVec2(sz, sz));
            ImGui::SameLine();
            ImGui::MenuItem(name);
        }
        ImGui::EndMenu();
    }

    // Here we demonstrate appending again to the "Options" menu (which we already created above)
    // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
    // In a real code-base using it would make senses to use this feature from very different code locations.
    if (ImGui::BeginMenu("Options")) // <-- Append!
    {
        static bool b = true;
        ImGui::Checkbox("SomeOption", &b);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Disabled", false)) // Disabled
    {
        IM_ASSERT(0);
    }
    if (ImGui::MenuItem("Checked", NULL, true)) {}
    if (ImGui::MenuItem("Quit", "Alt+F4")) {
        exit(EXIT_SUCCESS);
    }
}
