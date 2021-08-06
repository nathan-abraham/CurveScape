#include <iostream>
#include <fstream>
#include <limits>
#include <thread>
#include <chrono>
#include <array>
#include <filesystem>
#include <math.h>

#ifdef _WIN32
#endif

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <nfd.h>

#include "calculus.h"
#include "graph.h"
#include "constants.h"
#include "plotutil.h"
#include "thickLine.h"
#include "fileOperations.h"

// Temp includes, remove later
#include "eval.h"

#include "events/eventManager.h"
#include "events/graphToggledEvent.h"

#define LOG(x) std::cout << (x) << std::endl;
#define GET_VAR_NAME(var) (#var)

int main(int argc, char* argv[])
{
    //std::cout << sf::VideoMode::getDesktopMode().width << ", " << sf::VideoMode::getDesktopMode().height << std::endl;
    std::cout << argc << " arguments" << std::endl;
    for (int i = 0; i < argc; ++i)
    {
        std::cout << "[" << argv[i] << "]" << std::endl;
    }

    Graph::pointIncr = 1;
    Graph::numPoints = winWidth / Graph::pointIncr;

    sf::RenderWindow window(sf::VideoMode(winWidth, winHeight), "Graphing", sf::Style::Default);
    ImGui::SFML::Init(window, false);
    window.setFramerateLimit(60);
    window.resetGLStates();
    sf::Clock deltaClock;
    sf::View view = window.getDefaultView();
    EventManager eventManager;

    sf::Vector2f oldPos;
    static bool moving = false;
    static const float zoomIncrement = 0.075f;
    static const float zoomOffset = 0.001f;
    Graph::window = &window;

    sf::Font font;
    //if (!font.loadFromFile("assets/OpenSans-Regular.ttf")) {
    //    LOG("Could not load font")
    //    return 2;
    //}

    std::string currentPath = argv[0];
    currentPath = currentPath.substr(0, currentPath.size() - std::string("CurveScape.exe").size());
    if (!font.loadFromFile(currentPath + std::string("assets\\OpenSans-Regular.ttf"))) {
        LOG("Could not load font")
        return 2;
    }

    ImGuiIO& IO = ImGui::GetIO();

    //if (exists("assets/OpenSans-Bold.ttf")) {
    //    IO.Fonts->AddFontFromFileTTF("assets/OpenSans-Bold.ttf", 18);
    //}
    if (exists(currentPath + std::string("assets\\OpenSans-Bold.ttf"))) {
        IO.Fonts->AddFontFromFileTTF(std::string(currentPath + std::string("assets\\OpenSans-Bold.ttf")).c_str(), 18);
    }

    IO.Fonts->AddFontDefault();
    ImGui::SFML::UpdateFontTexture();

    //const char* iniFilename = std::string(currentPath + std::string("imgui.ini")).c_str();
    char iniFilename[150];
    strcpy_s(iniFilename, 149, std::string(currentPath + std::string("imgui.ini")).c_str());

    IO.IniFilename = iniFilename;

    sf::Image icon;
    icon.loadFromFile(currentPath + std::string("assets\\icon_transparent.png"));
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    ImGui::GetStyle().FrameRounding = 4.0f;
    ImGui::GetStyle().GrabRounding = 4.0f;

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);


    std::array<sf::Color, 6> graphColors = {atomRed, atomBlue, atomGreen, atomCyan, atomOrange, atomPurple};
    int colorIndex = 0;
    
    std::vector<Graph*> graphs;

    static int currentFuncIndex = 0;
    static int currentCalcIndex = 0;
    static int frameDelay = 0;

    static bool mainActive = true;
    static bool panelActive = true;
    static bool calcActive = false;
    static bool imGuiFocused = false;
    static bool screenshotFlag = false;

    static bool saveAsFlag = false;
    static bool saveFlag = false;
    static bool openFlag = false;

    static const char* comboPreview = "Select a function";
    static const char* calcPreview = "Select a calculation";

    static std::string imageFilename;
    static std::string currentFileOpen = "";

    static float x_coor = 0;
    static float intLowerBound = 0;
    static float intUpperBound = 0;
    static float maxLowerBound = 0;
    static float maxUpperBound = 0;

    static double integralResult = 0;
    static double derivativeResult = 0;
    static double maxResult = 0;
    static bool integralCalculated = false;
    static bool derivativeCalculated = false;
    static bool maxCalculated = false;

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

    static const char* options[] = {
        "nDeriv",
        "fnInt",
        "max",
    };
    static const int numOptions = 3;

    if (argc > 1) {
        loadGraphs(graphs, argv[1]);
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && !imGuiFocused) {
                if (event.mouseButton.button == 0) {
                    moving = true;
                    oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                }
            }

            else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == 0) {
                    moving = false;
                }
            }

            if (event.type == sf::Event::MouseMoved && !imGuiFocused) {
                if (moving) {
                    const sf::Vector2f newPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                    const sf::Vector2f deltaPos = oldPos - newPos;
                    view.setCenter(view.getCenter() + deltaPos);
                    window.setView(view);
                    oldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));

					for (Graph* graph : graphs) {
						graph->updatePoints();
					}
                }
            }

            else if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                view.reset(visibleArea);
                window.setView(sf::View(visibleArea));
                Graph::numPoints = (int)event.size.width;
				for (Graph* graph : graphs) {
					graph->updatePoints();
				}
            }

            if (event.type == sf::Event::MouseWheelScrolled && !IO.WantCaptureMouse) {
                if (event.mouseWheelScroll.delta > 0 && Graph::scaleFactor * (1 + zoomIncrement) <= SCALE_MAX + zoomOffset) {
                    zoomViewAt({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, window, view, Graph::scaleFactor, zoomIncrement);
                    //zoomViewAt({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, window, view, (1.f / zoomAmount));
					for (Graph* graph : graphs) {
						graph->updatePoints();
					}
                }
                else if (event.mouseWheelScroll.delta < 0 && Graph::scaleFactor * (1 - zoomIncrement) >= SCALE_MIN - zoomOffset) {
                    zoomViewAt({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, window, view, Graph::scaleFactor, -zoomIncrement);
                    //zoomViewAt({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, window, view, zoomAmount);
					for (Graph* graph : graphs) {
						graph->updatePoints();
					}
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !imGuiFocused) {
                if (Graph::scaleFactor - zoomIncrement > SCALE_MIN - zoomOffset)
                    Graph::scaleFactor -= zoomIncrement;
				for (Graph* graph : graphs) {
					graph->updatePoints();
				}
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !imGuiFocused) {
                if (Graph::scaleFactor + zoomIncrement < SCALE_MAX + zoomOffset)
                    Graph::scaleFactor += zoomIncrement;
				for (Graph* graph : graphs) {
					graph->updatePoints();
				}
            }

            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) &&
                sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
                openFlag = true;
            }
            else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) &&
                sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                saveFlag = true;
            }
        }

        // Debug
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            //std::cout << "Left mouse button pressed" << std::endl;
        }


        ImGui::SFML::Update(window, deltaClock.restart());

#ifdef _DEBUG
        ImGui::ShowDemoWindow();
#endif

        if (mainActive) {
            mainMenuBar(eventManager, graphs, currentFileOpen, imageFilename, mainActive, panelActive, 
                calcActive, screenshotFlag, frameDelay, saveAsFlag, saveFlag, openFlag);
        }

        if (panelActive) {

            ImGui::Begin("Graphing Panel", &panelActive, ImGuiWindowFlags_MenuBar); // begin window

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

            if (ImGui::SliderFloat("Zoom", &Graph::scaleFactor, SCALE_MIN, SCALE_MAX)) {
                for (Graph* graph : graphs) {
                    graph->updatePoints();
                }
            }

            for (int i = 0; i < graphs.size(); ++i) {
                if (ImGui::InputText(graphs[i]->name, graphs[i]->expression, IM_ARRAYSIZE(graphs[i]->expression))) {
                    //if (!ImGui::IsKeyPressed(59) && !ImGui::IsKeyPressed(66)) {
                    //    eventManager.addEvent(new TextEnteredEvent(graphs[i]->expression));
                    //    LOG("added event")
                    //}
                    //else {
                        //eventManager.addEvent(new TextDeletedEvent(graphs[i]->expression, eventManager.undo.top()->lastChar));
                    //}
                    graphs[i]->parseExpr();
                    graphs[i]->updatePoints();
                }
                ImGui::SameLine(0, 20);

                if (i < graphColors.size()) {
                    sf::Color& current = graphColors[i];
                    colors[ImGuiCol_CheckMark] = ImVec4(current.r / 255.0f, current.g / 255.0f, current.b / 255.0f, 1.0f);
                }
                if (ImGui::Checkbox(graphs[i]->checkName, &graphs[i]->on)) {
                    eventManager.addEvent(new GraphToggledEvent(&graphs[i]->on));
                }
            }


            ImGui::PopFont();
            ImGui::End(); // end window
        }

        if (calcActive) {
            ImGui::Begin("Calculate", &calcActive);
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);

            if (ImGui::BeginCombo("##selectFunc", comboPreview)) {
                for (int i = 0; i < graphs.size(); i++) {
                    if (strcmp(graphs[i]->expression, "") == 0) continue;
                    const bool is_selected = i == currentFuncIndex;
                    if (ImGui::Selectable(graphs[i]->expression, is_selected)) {
                        currentFuncIndex = i;
                        comboPreview = graphs[i]->expression;
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            if (strcmp(comboPreview, "Select a function") != 0) {
                if (ImGui::BeginCombo("##selectCalc", calcPreview)) {
                    for (int i = 0; i < numOptions; i++) {
                        const bool calc_selected = i == currentCalcIndex;
                        if (ImGui::Selectable(options[i], calc_selected)) {
                            currentCalcIndex = i;
                            calcPreview = options[i];
                        }
                        if (calc_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                if (strcmp(calcPreview, "nDeriv") == 0) {
                    ImGui::InputFloat("x-value", &x_coor);
                    if (ImGui::Button("Submit##derivSumbit")) {
                        derivativeResult = nDeriv(clean(graphs[currentFuncIndex]->expression).c_str(), x_coor);
                        derivativeCalculated = true;
                    }
                    if (derivativeCalculated) {
						std::string output = std::string("Result: ") + std::to_string(derivativeResult);
						ImGui::Text(output.c_str());
                    }
                }
                else if (strcmp(calcPreview, "fnInt") == 0) {
                    ImGui::InputFloat("Lower Bound", &intLowerBound);
                    ImGui::InputFloat("Upper Bound", &intUpperBound);
                    if (ImGui::Button("Submit##intSumbit")) {
                        //std::thread integral(fnInt, graphs[currentFuncIndex]->expression, lower_bound, upper_bound, 100000);
                        //integral.detach();
                        integralResult = fnInt(clean(graphs[currentFuncIndex]->expression).c_str(), intLowerBound, intUpperBound, 400000);
                        integralCalculated = true;
                    }
					if (integralCalculated) {
						std::string output = std::string("Result: ") + std::to_string(integralResult);
						ImGui::Text(output.c_str());
					}
                }
                else if (strcmp(calcPreview, "max") == 0) {
                    ImGui::InputFloat("Lower Bound", &maxLowerBound);
                    ImGui::InputFloat("Upper Bound", &maxUpperBound);
                    if (ImGui::Button("Submit##maxSubmit")) {
                        maxResult = func_max(clean(graphs[currentFuncIndex]->expression).c_str(), maxLowerBound, maxUpperBound);
                        maxCalculated = true;
                    }
					if (maxCalculated) {
						std::string output = std::string("Result: ") + std::to_string(maxResult);
						ImGui::Text(output.c_str());
					}
                }
            }

            ImGui::PopFont();
            ImGui::End();
        }
        imGuiFocused = ImGui::IsAnyItemActive();

        if (openFlag) {
			nfdchar_t* rawFilename;
			nfdresult_t result = NFD_OpenDialog("graph", NULL, &rawFilename);

			if (result == NFD_OKAY) {
				loadGraphs(graphs, rawFilename);
			}
			else {
				LOG(NFD_GetError());
			}
            openFlag = false;
        }
        else if (saveFlag) {
			if (currentFileOpen == "" || currentFileOpen.find(".png") != std::string::npos
                || currentFileOpen.find(".jpg") != std::string::npos) {
				saveAsFlag = true;
			}
			else {
				saveAsFlag = false;
				saveGraphs(graphs, currentFileOpen.c_str());
			}
            saveFlag = false;
        }

        if (saveAsFlag) {
            LOG("main loop save as")
			nfdchar_t* rawFilename;
			nfdresult_t result = NFD_SaveDialog("png;jpg;graph", NULL, &rawFilename);

			if (result == NFD_OKAY) {
				currentFileOpen = rawFilename;
				if (currentFileOpen.find(".graph") != std::string::npos) {
					saveGraphs(graphs, rawFilename);
				}
				else {
					imageFilename = rawFilename;

					if (imageFilename.substr(imageFilename.size() - 4).find(".") == std::string::npos) {
						LOG("added extension")
						imageFilename += ".png";
					}
					mainActive = false;
					panelActive = false;
					calcActive = false;
					screenshotFlag = true;
					frameDelay = 0;
				}

			}
			else if (result == NFD_CANCEL) {
				LOG("canceled saving")
			}
			else {
				LOG(NFD_GetError());
			}
            saveAsFlag = false;
        }

		if (screenshotFlag && !panelActive && !calcActive && frameDelay == 3) {
			take_screenshot(window, imageFilename);
			panelActive = true;
			calcActive = true;
            mainActive = true;
		}

		window.clear(bgColor);
		drawGrid2(window, winWidth, winHeight, ROWS, lineColor, Graph::scaleFactor, font, bgColor);
		for (int i = 0; i < graphs.size(); ++i) {
			if (graphs[i]->on)
				graphs[i]->draw(window);
		}

		ImGui::SFML::Render(window);
		window.display();

        if (frameDelay > 100000) {
            frameDelay = 50;
        }
        frameDelay++;
    }
    
    ImGui::SFML::Shutdown();

    return 0;
}