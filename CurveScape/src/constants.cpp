#include <SFML/Graphics.hpp>
#include "constants.h"

int winWidth = (sf::VideoMode::getDesktopMode().height * 0.8 + 50) / 100 * 100;
int winHeight = winWidth;
const int ROWS = 20;
const int gap = winWidth / ROWS;
