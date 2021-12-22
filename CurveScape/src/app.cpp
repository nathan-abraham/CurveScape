#include <iostream>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "app.h"
#include "graph.h"
#include "graphicsManager.h"
#include "events/eventManager.h"
#include "constants.h"
#include "fileOperations.h"

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
    GraphicsManager graphicsManager(window);
    EventManager eventManager;

    sf::Vector2f oldPos;
    static bool moving = false;
    static const float zoomIncrement = 0.075f;
    static const float zoomOffset = 0.001f;

    std::string currentPath = this->argv[0];
    currentPath = currentPath.substr(0, currentPath.size() - std::string("CurveScape.exe").size());
    if (!graphicsManager.font.loadFromFile(currentPath + std::string("assets\\OpenSans-Bold.ttf")))
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

}
