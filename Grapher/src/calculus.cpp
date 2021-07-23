#pragma once

#include <iostream>
#include <string>
#include <math.h>
#include <imgui.h>

#include "eval.h"
#define __DBL_EPSILON__ 2.2204460492503131e-16

double nDeriv(const char* expression, float& x) {
    double h = sqrt(__DBL_EPSILON__);
    return (eval(expression, x + h) - eval(expression, x)) / h;
}

double fnInt(const char* expression, float a, float b, float n) {
    double delta_x = (b - a) / n;
    double area = 0;
    for (int i = 0; i < n; i++) {
        area += eval(expression, a + i * delta_x) * delta_x;
    }

    ImGui::Text(std::to_string(area).c_str());
    return area;
}