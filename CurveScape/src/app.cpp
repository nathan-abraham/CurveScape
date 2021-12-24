#include <iostream>
#include <array>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <nfd.h>

#include "app.h"
#include "graph.h"
#include "graphicsManager.h"
#include "events/eventManager.h"
#include "events/graphToggledEvent.h"
#include "events/panelToggledEvent.h"
#include "constants.h"
#include "fileOperations.h"
#include "stateManager.h"
#include "plotutil.h"
#include "calculus.h"
#include "eval.h"
#include "fileOperations.h"

#define LOG(x) std::cout << (x) << std::endl;

App::App(int argc, char* argv[]) {
#ifdef _DEBUG
    std::cout << sf::VideoMode::getDesktopMode().width << ", " << sf::VideoMode::getDesktopMode().height << std::endl;
    std::cout << argc << " arguments" << std::endl;
    for (int i = 0; i < argc; ++i)
    {
        std::cout << "[" << argv[i] << "]" << std::endl;
    }
#endif
    this->argc = argc;
    this->argv = argv;
}

void App::run() {
    sf::RenderWindow window(sf::VideoMode(winWidth, winHeight), "Graphing", sf::Style::Default);
    GraphicsManager gm(window);
    StateManager sm;
    EventManager eventManager;

    sf::Vector2f oldPos;
    static bool moving = false;
    static const float zoomIncrement = 0.075f;
    static const float zoomOffset = 0.001f;

    std::string currentPath = this->argv[0];
    currentPath = currentPath.substr(0, currentPath.size() - std::string("CurveScape.exe").size());
    if (!gm.font.loadFromFile(currentPath + std::string("assets\\OpenSans-Bold.ttf")))
    {
        return;
    }

    ImGuiIO &IO = ImGui::GetIO();

    if (exists(currentPath + std::string("assets\\OpenSans-Bold.ttf")))
    {
        IO.Fonts->AddFontFromFileTTF(std::string(currentPath + std::string("assets\\OpenSans-Bold.ttf")).c_str(), 18);
    }

    IO.Fonts->AddFontDefault();
    ImGui::SFML::UpdateFontTexture();

    char iniFilename[150];
    strcpy_s(iniFilename, 149, std::string(currentPath + std::string("imgui.ini")).c_str());

    IO.IniFilename = iniFilename;

    gm.icon.loadFromFile(currentPath + std::string("assets\\icon_transparent.png"));
    window.setIcon(gm.icon.getSize().x, gm.icon.getSize().y, gm.icon.getPixelsPtr());
    gm.setImGuiStyles();

    std::array<sf::Color, 6> graphColors = {atomRed, atomBlue, atomGreen, atomCyan, atomOrange, atomPurple};
    int colorIndex = 0;

    std::vector<Graph*> graphs;
    std::vector<Graph*> polarGraphs;

    Graph y1("", "y1", true, graphColors[0]);
    Graph y2("", "y2", true, graphColors[1]);
    Graph y3("", "y3", true, graphColors[2]);
    Graph y4("", "y4", true, graphColors[3]);
    Graph y5("", "y5", true, graphColors[4]);
    Graph y6("", "y6", true, graphColors[5]);

    graphs.push_back(&y1);
    graphs.push_back(&y2);
    graphs.push_back(&y3);
    graphs.push_back(&y4);
    graphs.push_back(&y5);
    graphs.push_back(&y6);

    Graph polar_y1("", "r1", true, graphColors[0], true);
    Graph polar_y2("", "r2", true, graphColors[1], true);
    Graph polar_y3("", "r3", true, graphColors[2], true);

    polarGraphs.push_back(&polar_y1);
    polarGraphs.push_back(&polar_y2);
    polarGraphs.push_back(&polar_y3);

    ImVec4 *colors = ImGui::GetStyle().Colors;

    if (argc > 1)
    {
        loadGraphs(graphs, argv[1]);
        sm.currentFileOpen = argv[1];
    }

    while (window.isOpen())
    {
        sf::Event event;
        if (window.waitEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && !sm.imGuiFocused)
            {
                if (event.mouseButton.button == 0)
                {
                    moving = true;
                    oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                }
            }

            else if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == 0)
                {
                    moving = false;
                }
            }

            if (event.type == sf::Event::MouseMoved && !sm.imGuiFocused)
            {
                if (moving)
                {
                    const sf::Vector2f newPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                    const sf::Vector2f deltaPos = oldPos - newPos;
                    gm.view.setCenter(gm.view.getCenter() + deltaPos);
                    window.setView(gm.view);
                    oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));

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
                if (event.mouseWheelScroll.delta > 0 && Graph::scaleFactor * (1 + zoomIncrement) <= SCALE_MAX + zoomOffset)
                {
                    zoomViewAt({event.mouseWheelScroll.x, event.mouseWheelScroll.y}, window, gm.view, Graph::scaleFactor, zoomIncrement);
                    for (Graph *graph : graphs)
                    {
                        graph->updatePoints();
                    }
                    for (Graph* graph : polarGraphs) {
                        graph->updatePoints();
                    }
                }
                else if (event.mouseWheelScroll.delta < 0 && Graph::scaleFactor * (1 - zoomIncrement) >= SCALE_MIN - zoomOffset)
                {
                    zoomViewAt({event.mouseWheelScroll.x, event.mouseWheelScroll.y}, window, gm.view, Graph::scaleFactor, -zoomIncrement);
                    for (Graph *graph : graphs)
                    {
                        graph->updatePoints();
                    }
                    for (Graph* graph : polarGraphs) {
                        graph->updatePoints();
                    }
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !sm.imGuiFocused)
            {
                if (Graph::scaleFactor - zoomIncrement > SCALE_MIN - zoomOffset)
                    Graph::scaleFactor -= zoomIncrement;
                for (Graph *graph : graphs)
                {
                    graph->updatePoints();
                }
                for (Graph* graph : polarGraphs) {
                    graph->updatePoints();
                }
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !sm.imGuiFocused)
            {
                if (Graph::scaleFactor + zoomIncrement < SCALE_MAX + zoomOffset)
                    Graph::scaleFactor += zoomIncrement;
                for (Graph *graph : graphs)
                {
                    graph->updatePoints();
                }
                for (Graph* graph : polarGraphs) {
                    graph->updatePoints();
                }
            }

            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                 sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) &&
                sf::Keyboard::isKeyPressed(sf::Keyboard::O))
            {
                sm.openFlag = true;
            }
            else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                      sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) &&
                     sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                sm.saveFlag = true;
            }
        }

        // Debug
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            //std::cout << "Left mouse button pressed" << std::endl;
            for (Graph* graph : graphs) {
                graph->updatePoints();
            }
        }

        ImGui::SFML::Update(window, gm.deltaClock.restart());

#ifdef _DEBUG
        ImGui::ShowDemoWindow();
#endif

        if (sm.mainActive)
        {
            mainMenuBar(eventManager, graphs, sm);
        }

        if (sm.panelActive)
        {

            ImGui::Begin("Graphing Panel", &sm.panelActive, ImGuiWindowFlags_MenuBar); // begin window

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

            if (ImGui::SliderFloat("Zoom", &Graph::scaleFactor, SCALE_MIN, SCALE_MAX))
            {
                for (Graph *graph : graphs)
                {
                    graph->updatePoints();
                }
            }

            for (int i = 0; i < graphs.size(); ++i)
            {
                if (ImGui::InputText(graphs[i]->name, graphs[i]->expression, IM_ARRAYSIZE(graphs[i]->expression)))
                {
                    graphs[i]->parseExpr();
                    graphs[i]->updatePoints();
                }
                ImGui::SameLine(0, 20);

                if (i < graphColors.size())
                {
                    sf::Color &current = graphColors[i];
                    colors[ImGuiCol_CheckMark] = ImVec4(current.r / 255.0f, current.g / 255.0f, current.b / 255.0f, 1.0f);
                }
                if (ImGui::Checkbox(graphs[i]->checkName, &graphs[i]->on))
                {
                    eventManager.addEvent(new GraphToggledEvent(&graphs[i]->on));
                }
            }

            ImGui::PopFont();
            ImGui::End(); // end window
        }

        if (sm.polarPanelActive) {
            ImGui::Begin("Polar Graphing Panel", &sm.polarPanelActive, ImGuiWindowFlags_MenuBar); // begin window

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

            if (ImGui::SliderFloat("Zoom", &Graph::scaleFactor, SCALE_MIN, SCALE_MAX))
            {
                for (Graph* graph : polarGraphs)
                {
                    graph->updatePoints();
                }
            }

            for (int i = 0; i < polarGraphs.size(); ++i)
            {
                if (ImGui::InputText(polarGraphs[i]->name, polarGraphs[i]->expression, IM_ARRAYSIZE(polarGraphs[i]->expression)))
                {
                    polarGraphs[i]->parseExpr();
                    polarGraphs[i]->updatePoints();
                }
                ImGui::SameLine(0, 20);

                if (i < graphColors.size())
                {
                    sf::Color &current = graphColors[i];
                    colors[ImGuiCol_CheckMark] = ImVec4(current.r / 255.0f, current.g / 255.0f, current.b / 255.0f, 1.0f);
                }
                if (ImGui::Checkbox(polarGraphs[i]->checkName, &polarGraphs[i]->on))
                {
                    eventManager.addEvent(new GraphToggledEvent(&polarGraphs[i]->on));
                }
            }

            ImGui::PopFont();
            ImGui::End(); // end window
        }

        if (sm.calcActive)
        {
            ImGui::Begin("Calculate", &sm.calcActive);
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

            if (ImGui::BeginCombo("##selectFunc", sm.comboPreview))
            {
                for (int i = 0; i < graphs.size(); i++)
                {
                    if (strcmp(graphs[i]->expression, "") == 0)
                        continue;
                    const bool is_selected = i == sm.currentFuncIndex;
                    if (ImGui::Selectable(graphs[i]->expression, is_selected))
                    {
                        sm.currentFuncIndex = i;
                        strcpy_s(sm.comboPreview, 30, graphs[i]->expression);
                    }
                    if (is_selected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            if (strcmp(sm.comboPreview, "Select a function") != 0)
            {
                if (ImGui::BeginCombo("##selectCalc", sm.calcPreview))
                {
                    for (int i = 0; i < sm.numOptions; i++)
                    {
                        const bool calc_selected = i == sm.currentCalcIndex;
                        if (ImGui::Selectable(sm.options[i], calc_selected))
                        {
                            sm.currentCalcIndex = i;
                            strcpy_s(sm.calcPreview, 30, sm.options[i]);
                        }
                        if (calc_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                if (strcmp(sm.calcPreview, "nDeriv") == 0)
                {
                    ImGui::InputFloat("x-value", &sm.x_coor);
                    if (ImGui::Button("Submit##derivSumbit"))
                    {
                        sm.derivativeResult = nDeriv(clean(graphs[sm.currentFuncIndex]->expression).c_str(), sm.x_coor);
                        sm.derivativeCalculated = true;
                    }
                    if (sm.derivativeCalculated)
                    {
                        std::string output = std::string("Result: ") + std::to_string(sm.derivativeResult);
                        ImGui::Text(output.c_str());
                    }
                }
                else if (strcmp(sm.calcPreview, "fnInt") == 0)
                {
                    ImGui::InputFloat("Lower Bound", &sm.intLowerBound);
                    ImGui::InputFloat("Upper Bound", &sm.intUpperBound);
                    if (ImGui::Button("Submit##intSumbit"))
                    {
                        //std::thread integral(fnInt, graphs[currentFuncIndex]->expression, lower_bound, upper_bound, 100000);
                        //integral.detach();
                        sm.integralResult = fnInt(clean(graphs[sm.currentFuncIndex]->expression).c_str(), sm.intLowerBound, sm.intUpperBound, 400000);
                        sm.integralCalculated = true;
                    }
                    if (sm.integralCalculated)
                    {
                        std::string output = std::string("Result: ") + std::to_string(sm.integralResult);
                        ImGui::Text(output.c_str());
                    }
                }
                else if (strcmp(sm.calcPreview, "max") == 0)
                {
                    ImGui::InputFloat("Lower Bound", &sm.maxLowerBound);
                    ImGui::InputFloat("Upper Bound", &sm.maxUpperBound);
                    if (ImGui::Button("Submit##maxSubmit"))
                    {
                        sm.maxResult = func_max(clean(graphs[sm.currentFuncIndex]->expression).c_str(), sm.maxLowerBound, sm.maxUpperBound);
                        sm.maxCalculated = true;
                    }
                    if (sm.maxCalculated)
                    {
                        std::string output = std::string("Result: ") + std::to_string(sm.maxResult);
                        ImGui::Text(output.c_str());
                    }
                }
            }

            ImGui::PopFont();
            ImGui::End();
        }
        sm.imGuiFocused = ImGui::IsAnyItemActive();

        if (sm.openFlag)
        {
            nfdchar_t *rawFilename;
            nfdresult_t result = NFD_OpenDialog("graph", NULL, &rawFilename);

            if (result == NFD_OKAY)
            {
                loadGraphs(graphs, rawFilename);
                sm.currentFileOpen = rawFilename;
            }
            else
            {
                LOG(NFD_GetError());
            }
            sm.openFlag = false;
        }
        else if (sm.saveFlag)
        {
            if (sm.currentFileOpen == "" || sm.currentFileOpen.find(".png") != std::string::npos || sm.currentFileOpen.find(".jpg") != std::string::npos)
            {
                sm.saveAsFlag = true;
            }
            else
            {
                sm.saveAsFlag = false;
                saveGraphs(graphs, sm.currentFileOpen.c_str());
            }
            sm.saveFlag = false;
        }

        if (sm.saveAsFlag)
        {
            saveGraphsAs(sm, graphs);
        }

        if (sm.screenshotFlag && !sm.panelActive && !sm.calcActive && sm.frameDelay == 3)
        {
            take_screenshot(window, sm.imageFilename);
            sm.panelActive = true;
            sm.calcActive = true;
            sm.mainActive = true;
        }

        window.clear(bgColor);

        if (sm.panelActive) {
            drawGrid(window, winWidth, winHeight, ROWS, lineColor, Graph::scaleFactor, gm.font, bgColor);
        }
        else if (sm.polarPanelActive) {
            drawPolarGrid(window, winWidth, winHeight, ROWS, lineColor, Graph::scaleFactor, gm.font, bgColor);
        }

        for (int i = 0; i < graphs.size(); ++i)
        {
            if (graphs[i]->shouldGraph())
                graphs[i]->draw(window);
        }
        for (int i = 0; i < polarGraphs.size(); ++i) {
            if (polarGraphs[i]->shouldGraph())
                polarGraphs[i]->draw(window);
        }

        ImGui::SFML::Render(window);
        window.display();

        if (sm.frameDelay > 100000)
        {
            sm.frameDelay = 50;
        }
        sm.frameDelay++;
    }

    ImGui::SFML::Shutdown();
}
