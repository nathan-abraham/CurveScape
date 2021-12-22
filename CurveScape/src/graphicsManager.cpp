#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include "graphicsManager.h"
#include "graph.h"
#include "constants.h"


GraphicsManager::GraphicsManager(sf::RenderWindow& window) {
    Graph::pointIncr = 1;
    Graph::numPoints = winWidth / Graph::pointIncr;

    ImGui::SFML::Init(window, false);
    window.setFramerateLimit(60);
    window.resetGLStates();
    view = window.getDefaultView();
    Graph::window = &window;
}

GraphicsManager::~GraphicsManager() {
}

void GraphicsManager::setImGuiStyles() {

}