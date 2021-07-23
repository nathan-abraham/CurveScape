#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include "dropdown.cpp"
#include "textbox.cpp"

class Calculate {
    public:
    std::string type;
    DropdownMenu* options = NULL;
    DropdownMenu* funcs = NULL;
    TextBox* first = NULL;
    TextBox* second = NULL;
    Button* submit = NULL;
    
    
};