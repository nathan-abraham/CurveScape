#include "thickLine.h"
#include "imgui.h"
#include "plotutil.h"
#include <iostream>
#include <string>

#define M_PI 3.14159265358979323846

const float ThickLine::vertOffset = 0.1f;

ThickLine::ThickLine(const sf::Vector2f& a, const sf::Vector2f& b) {
	this->p1 = a;
	this->p2 = b;
	calc();
}

ThickLine::ThickLine(const sf::Color& color) { 
	this->line.setFillColor(color);
}

double findDist(const sf::Vector2f& a, const sf::Vector2f& b) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

float rtd(float r) {
	return r * 180 / M_PI;
}

float dtr(float r) {
	return r * M_PI / 180;
}

void ThickLine::setCoor(const sf::Vector2f& a, const sf::Vector2f& b) {
	p1 = a;
	p2 = b;
	calc();
}

void ThickLine::calc() {
	const float thickness = 4;
	float angle = rtd(atan((p2.y - p1.y) / (p2.x - p1.x)));
	line.setRotation(0);
	line.rotate(angle);
	//if (p2.y <= p1.y)
		line.setPosition(sf::Vector2f(p1.x - thickness / 2 * cos(dtr(angle)), p1.y + thickness / 2 * sin(dtr(angle))));
	//else
	//	line.setPosition(sf::Vector2f(p1.x + thickness / 2 * cos(dtr(angle)), p1.y - thickness / 2 * sin(dtr(angle))));
	line.setSize(sf::Vector2f(findDist(p1, p2), thickness));

	linePos = line.getPosition();
	lineRect = line.getGlobalBounds();
}

bool ThickLine::isOver(sf::RenderWindow& window, const sf::Vector2i& pos) {
	//if (pos.x < linePos.x)
	//	return false;
	//else if (pos.x > linePos.x + lineRect.width)
	//	return false;
	//else if (pos.y < linePos.y)
	//	return false;
	//else if (pos.y > linePos.y + lineRect.height)
	//	return false;
	//return true;
	return lineRect.contains(window.mapPixelToCoords(pos));
}

void ThickLine::draw(sf::RenderWindow& window) {
	window.draw(line);


	if (isOver(window, sf::Mouse::getPosition(window))) {
		sf::Vector2f temp(SFMLToCartesian(p1, origin));
		std::string toolTip = std::to_string(temp.x / Graph::scaleFactor / (winWidth / ROWS)) + std::string(", ") + 
			std::to_string(temp.y / Graph::scaleFactor / (winWidth / ROWS));
		ImGui::SetTooltip(toolTip.c_str());
	}


	//sf::Vertex line2[] {
	//	sf::Vertex(p1, sf::Color::Blue),
	//	sf::Vertex(p2, sf::Color::Blue)
	//};
	//window.draw(line2, 2, sf::Lines);
}