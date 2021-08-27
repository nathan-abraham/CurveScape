#include <SFML/Graphics.hpp>
#include <nfd.h>
#include <iostream>
#include "constants.h"
#include "imgui.h"

#include "graph.h"
#include "fileOperations.h"
#include "events/eventManager.h"
#include "events/panelToggledEvent.h"
#include "plotutil.h"

#define LOG(x) std::cout << (x) << std::endl;

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

std::vector<sf::CircleShape> plot(int width, int height, int rows, const sf::Vector2f origin, int numPoints, sf::Color pointColor, double(*function)(double), double scaleFactor) {
    double gap = width / rows * scaleFactor;
    std::vector<sf::CircleShape> points(numPoints);
    int idx = 0;
    for (int i = -numPoints / 2; i < numPoints / 2; i++) {
        int radius = 3;
        double x = i / gap;
        sf::CircleShape point(5, 4);
        sf::Vector2f position(i - radius, function(x) * gap + radius);
        point.setPosition(cartesianToSFML(position, origin));
        point.setFillColor(pointColor);
        points[idx] = point;
        idx++;
    }
    return points;
}



void drawGrid(sf::RenderWindow& window, const int& width, const int& height, const int& rows, const sf::Color& lineColor, const float& scaleFactor, 
    const sf::Font& font, const sf::Color& bgColor) {
    int gap = width / rows;
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

    for (int i = 0; i < (sfCurrentCenter.x + sfCurrentSize.x) / scaleFactor; i += gap) {
		sf::Vertex line[] = {
			sf::Vertex(cartesianToSFML(sf::Vector2f(i * scaleFactor, sfCurrentCenter.y - sfCurrentSize.y), origin), lineColor),
			sf::Vertex(cartesianToSFML(sf::Vector2f(i * scaleFactor, sfCurrentCenter.y + sfCurrentSize.y), origin), lineColor),
		};
		window.draw(line, 2, sf::Lines);
        if (i != 0) {
            text.setString(std::to_string(i / gap));
            sf::FloatRect textRect = text.getGlobalBounds();
            text.setPosition(cartesianToSFML(sf::Vector2f(i * scaleFactor - textRect.width / 2.0f, -3), origin));
            sf::RectangleShape back(sf::Vector2f(textRect.width, textRect.height + 5));
            back.setPosition(text.getPosition());
            back.setFillColor(bgColor);

            window.draw(back);
            window.draw(text);
        }
    }

    for (int i = 0; i > (sfCurrentCenter.x - sfCurrentSize.x) / scaleFactor; i -= gap) {
		sf::Vertex line[] = {
			sf::Vertex(cartesianToSFML(sf::Vector2f(i * scaleFactor, sfCurrentCenter.y - sfCurrentSize.y), origin), lineColor),
			sf::Vertex(cartesianToSFML(sf::Vector2f(i * scaleFactor, sfCurrentCenter.y + sfCurrentSize.y), origin), lineColor),
		};
		window.draw(line, 2, sf::Lines);
        if (i != 0) {
            text.setString(std::to_string(i / gap));
            sf::FloatRect textRect = text.getGlobalBounds();
            text.setPosition(cartesianToSFML(sf::Vector2f(i * scaleFactor - textRect.width / 2.0f, -3), origin));
            sf::RectangleShape back(sf::Vector2f(textRect.width, textRect.height + 5));
            back.setPosition(text.getPosition());
            back.setFillColor(bgColor);

            window.draw(back);
            window.draw(text);
        }
    }

    for (int i = 0; i < (sfCurrentCenter.y - sfCurrentSize.y) / scaleFactor; i += gap) {
		sf::Vertex line[] = {
			sf::Vertex(cartesianToSFML(sf::Vector2f(sfCurrentCenter.x + sfCurrentSize.x, i * scaleFactor), origin), lineColor),
			sf::Vertex(cartesianToSFML(sf::Vector2f(sfCurrentCenter.x - sfCurrentSize.x, i * scaleFactor), origin), lineColor),
		};
		window.draw(line, 2, sf::Lines);
        if (i != 0) {
            text.setString(std::to_string(i / gap));
            sf::FloatRect textRect = text.getGlobalBounds();
            text.setPosition(cartesianToSFML(sf::Vector2f(-textRect.width - 6, i * scaleFactor + textRect.height / 1.25f), origin));
            sf::RectangleShape back(sf::Vector2f(textRect.width, textRect.height + 5));
            back.setPosition(text.getPosition());
            back.setFillColor(bgColor);

            window.draw(back);
            window.draw(text);
        }
    }

    for (int i = 0; i > (sfCurrentCenter.y + sfCurrentSize.y) / scaleFactor; i -= gap) {
		sf::Vertex line[] = {
			sf::Vertex(cartesianToSFML(sf::Vector2f(sfCurrentCenter.x + sfCurrentSize.x, i * scaleFactor), origin), lineColor),
			sf::Vertex(cartesianToSFML(sf::Vector2f(sfCurrentCenter.x - sfCurrentSize.x, i * scaleFactor), origin), lineColor),
		};
		window.draw(line, 2, sf::Lines);
        if (i != 0) {
            text.setString(std::to_string(i / gap));
            sf::FloatRect textRect = text.getGlobalBounds();
            text.setPosition(cartesianToSFML(sf::Vector2f(-textRect.width - 6, i * scaleFactor + textRect.height / 1.25f), origin));
            sf::RectangleShape back(sf::Vector2f(textRect.width, textRect.height + 5));
            back.setPosition(text.getPosition());
            back.setFillColor(bgColor);

            window.draw(back);
            window.draw(text);
        }
    }
}

void drawGrid2(sf::RenderWindow& window, const int& width, const int& height, const int& rows, const sf::Color& lineColor, const float& scaleFactor, 
    const sf::Font& font, const sf::Color& bgColor) {

    const int gap = width / rows;
    float benchmark = scaleFactor / 3;

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

    for (int i = 0; i < (sfCurrentCenter.x + sfCurrentSize.x) / scaleFactor; i += gap) {
		sf::Vertex line[] = {
			sf::Vertex(cartesianToSFML(sf::Vector2f(i * scaleFactor, sfCurrentCenter.y - sfCurrentSize.y), origin), lineColor),
			sf::Vertex(cartesianToSFML(sf::Vector2f(i * scaleFactor, sfCurrentCenter.y + sfCurrentSize.y), origin), lineColor),
		};
		window.draw(line, 2, sf::Lines);
        if (i != 0) {
            text.setString(std::to_string(i / gap));
            sf::FloatRect textRect = text.getGlobalBounds();
            text.setPosition(cartesianToSFML(sf::Vector2f(i * scaleFactor - textRect.width / 2.0f, -3), origin));
            sf::RectangleShape back(sf::Vector2f(textRect.width, textRect.height + 5));
            back.setPosition(text.getPosition());
            back.setFillColor(bgColor);

            window.draw(back);
            window.draw(text);
        }
    }

    for (int i = 0; i > (sfCurrentCenter.x - sfCurrentSize.x) / scaleFactor; i -= gap) {
		sf::Vertex line[] = {
			sf::Vertex(cartesianToSFML(sf::Vector2f(i * scaleFactor, sfCurrentCenter.y - sfCurrentSize.y), origin), lineColor),
			sf::Vertex(cartesianToSFML(sf::Vector2f(i * scaleFactor, sfCurrentCenter.y + sfCurrentSize.y), origin), lineColor),
		};
		window.draw(line, 2, sf::Lines);
        if (i != 0) {
            text.setString(std::to_string(i / gap));
            sf::FloatRect textRect = text.getGlobalBounds();
            text.setPosition(cartesianToSFML(sf::Vector2f(i * scaleFactor - textRect.width / 2.0f, -3), origin));
            sf::RectangleShape back(sf::Vector2f(textRect.width, textRect.height + 5));
            back.setPosition(text.getPosition());
            back.setFillColor(bgColor);

            window.draw(back);
            window.draw(text);
        }
    }

    for (int i = 0; i < (sfCurrentCenter.y - sfCurrentSize.y) / scaleFactor; i += gap) {
		sf::Vertex line[] = {
			sf::Vertex(cartesianToSFML(sf::Vector2f(sfCurrentCenter.x + sfCurrentSize.x, i * scaleFactor), origin), lineColor),
			sf::Vertex(cartesianToSFML(sf::Vector2f(sfCurrentCenter.x - sfCurrentSize.x, i * scaleFactor), origin), lineColor),
		};
		window.draw(line, 2, sf::Lines);
        if (i != 0) {
            text.setString(std::to_string(i / gap));
            sf::FloatRect textRect = text.getGlobalBounds();
            text.setPosition(cartesianToSFML(sf::Vector2f(-textRect.width - 6, i * scaleFactor + textRect.height / 1.25f), origin));
            sf::RectangleShape back(sf::Vector2f(textRect.width, textRect.height + 5));
            back.setPosition(text.getPosition());
            back.setFillColor(bgColor);

            window.draw(back);
            window.draw(text);
        }
    }

    for (int i = 0; i > (sfCurrentCenter.y + sfCurrentSize.y) / scaleFactor; i -= gap) {
		sf::Vertex line[] = {
			sf::Vertex(cartesianToSFML(sf::Vector2f(sfCurrentCenter.x + sfCurrentSize.x, i * scaleFactor), origin), lineColor),
			sf::Vertex(cartesianToSFML(sf::Vector2f(sfCurrentCenter.x - sfCurrentSize.x, i * scaleFactor), origin), lineColor),
		};
		window.draw(line, 2, sf::Lines);
        if (i != 0) {
            text.setString(std::to_string(i / gap));
            sf::FloatRect textRect = text.getGlobalBounds();
            text.setPosition(cartesianToSFML(sf::Vector2f(-textRect.width - 6, i * scaleFactor + textRect.height / 1.25f), origin));
            sf::RectangleShape back(sf::Vector2f(textRect.width, textRect.height + 5));
            back.setPosition(text.getPosition());
            back.setFillColor(bgColor);

            window.draw(back);
            window.draw(text);
        }
    }
}

void zoomViewAt(const sf::Vector2i& pixel, sf::RenderWindow& window, sf::View& view, float& scalef, const float& incr)
{
    const sf::Vector2f beforeCoord(SFMLToCartesian(window.mapPixelToCoords(pixel), origin));
    scalef *= (1 + incr);
    //const sf::Vector2f afterCoord(beforeCoord * scalef / (scalef - incr));
    const sf::Vector2f afterCoord(beforeCoord * scalef / (scalef / (1 + incr)));

    //std::cout << "Before: " << beforeCoord.x << ", " << beforeCoord.y << std::endl;
    //std::cout << "After: " << afterCoord.x << ", " << afterCoord.y << std::endl << std::endl;

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
    bool& calc, bool& screen, int& frame, bool& saveAsFlag, bool& saveFlag, bool& openFlag)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            menuFile(em, graphs, current, imageFilename, main, panel, calc, screen, frame, saveAsFlag, saveFlag, openFlag);
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
                em.addEvent(new PanelToggledEvent(&panel));
            }
            if (ImGui::MenuItem("Calculation Panel", NULL, calc)) {
                calc = !calc;
                em.addEvent(new PanelToggledEvent(&calc));
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void menuFile(EventManager& em, std::vector<Graph*>& graphs, std::string& current, std::string& imageFilename, bool& main, bool& panel, 
    bool& calc, bool& screen, int& frame, bool& saveAsFlag, bool& saveFlag, bool& openFlag)
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
                menuFile(em, graphs, current, imageFilename, main, panel, calc, screen, frame, saveAsFlag, saveFlag, openFlag);
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
