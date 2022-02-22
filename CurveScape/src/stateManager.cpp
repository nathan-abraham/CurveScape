#include <future>
#include <string>
#include <string.h>

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "stateManager.h"
#include "graphicsManager.h"
#include "graph.h"
#include "plotutil.h"
#include "constants.h"

StateManager::StateManager() {
    currentFuncIndex = 0;
    currentCalcIndex = 0;
    frameDelay = 0;

    mainActive = true;
    panelActive = true;
    calcActive = false;
    polarPanelActive = false;
    imGuiFocused = false;
    screenshotFlag = false;

    saveAsFlag = false;
    saveFlag = false;

    strcpy_s(comboPreview, 30, "Select a function");
    strcpy_s(calcPreview, 30, "Select a calculation");

    currentFileOpen = "";
    x_coor = 0;
    intLowerBound = 0;
    intUpperBound = 0;
    maxLowerBound = 0;
    maxUpperBound = 0;

    integralResult = 0;
    derivativeResult = 0;
    maxResult = 0;

    integralLoading = false;
    integralCalculated = false;
    derivativeCalculated = false;
    maxCalculated = false;

    strcpy_s(options[0], 10, "nDeriv");
    strcpy_s(options[1], 10, "fnInt");
    strcpy_s(options[2], 10, "max");
    numOptions = 3;

    moving = false;
    zoomIncrement = 0.075f;
    zoomOffset = 0.001f;
}

void StateManager::handleEvents(sf::RenderWindow& window, GraphicsManager& gm, sf::Event& event, std::vector<Graph*>& graphs, std::vector<Graph*>& polarGraphs, ImGuiIO& IO) {
    ImGui::SFML::ProcessEvent(event);

    if (event.type == sf::Event::Closed)
        window.close();

    if (event.type == sf::Event::MouseButtonPressed && !this->imGuiFocused)
    {
        if (event.mouseButton.button == 0)
        {
            this->moving = true;
            this->oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        }
    }

    else if (event.type == sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button == 0)
        {
            this->moving = false;
        }
    }

    if (event.type == sf::Event::MouseMoved && !this->imGuiFocused)
    {
        if (this->moving)
        {
            const sf::Vector2f newPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            const sf::Vector2f deltaPos = this->oldPos - newPos;
            gm.view.setCenter(gm.view.getCenter() + deltaPos);
            window.setView(gm.view);
            this->oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));

            for (Graph *graph : graphs)
            {
                graph->updatePoints();
            }
        }
    }

    else if (event.type == sf::Event::Resized)
    {
        sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
        gm.view.reset(visibleArea);
        window.setView(sf::View(visibleArea));
        Graph::numPoints = (int)event.size.width;
        for (Graph *graph : graphs)
        {
            graph->updatePoints();
        }
    }

    if (event.type == sf::Event::MouseWheelScrolled && !IO.WantCaptureMouse)
    {
        if (event.mouseWheelScroll.delta > 0 && Graph::scaleFactor * (1 + this->zoomIncrement) <= SCALE_MAX + this->zoomOffset)
        {
            zoomViewAt({event.mouseWheelScroll.x, event.mouseWheelScroll.y}, window, gm.view, Graph::scaleFactor, this->zoomIncrement);
            for (Graph *graph : graphs)
            {
                graph->updatePoints();
            }
            for (Graph* graph : polarGraphs) {
                graph->updatePoints();
            }
        }
        else if (event.mouseWheelScroll.delta < 0 && Graph::scaleFactor * (1 - this->zoomIncrement) >= SCALE_MIN - this->zoomOffset)
        {
            zoomViewAt({event.mouseWheelScroll.x, event.mouseWheelScroll.y}, window, gm.view, Graph::scaleFactor, - this->zoomIncrement);
            for (Graph *graph : graphs)
            {
                graph->updatePoints();
            }
            for (Graph* graph : polarGraphs) {
                graph->updatePoints();
            }
        }
    }

    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
         sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) &&
        sf::Keyboard::isKeyPressed(sf::Keyboard::O))
    {
        this->openFlag = true;
    }
    else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
              sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) &&
             sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        this->saveFlag = true;
    }

}