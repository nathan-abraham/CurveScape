#pragma once
#include <future>
#include <SFML/Graphics.hpp>
#include <imgui.h>

#include "graphicsManager.h"
#include "graph.h"

class StateManager {
public:
    int currentFuncIndex = 0;
    int currentCalcIndex = 0;
    int frameDelay = 0;

    bool mainActive = true;
    bool panelActive = true;
    bool calcActive = false;
    bool polarPanelActive = false;
    bool imGuiFocused = false;
    bool screenshotFlag = false;

    bool saveAsFlag = false;
    bool saveFlag = false;
    bool openFlag = false;

    char comboPreview[30];
    char calcPreview[30];

    std::string imageFilename;
    std::string currentFileOpen;

    float x_coor;
    float intLowerBound;
    float intUpperBound;
    float maxLowerBound;
    float maxUpperBound;

    double integralResult;
    double derivativeResult;
    double maxResult;

    bool integralCalculated;
    bool integralLoading;
    bool derivativeCalculated;
    bool maxCalculated;

    char options[3][10];
    int numOptions;

    sf::Vector2f oldPos;
    bool moving;
    float zoomIncrement;
    float zoomOffset;


    std::future<void> future;

    StateManager();
    void handleEvents(sf::RenderWindow& window, GraphicsManager& gm, sf::Event& event, std::vector<Graph*>& graphs, std::vector<Graph*>& polarGraphs, ImGuiIO& IO);
};
