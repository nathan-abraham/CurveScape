#pragma once

#include <iostream>
#include <string>
#include <future>
#include <math.h>
#include <imgui.h>

#include "eval.h"
#include "constants.h"
#include "stateManager.h"

#define __DBL_EPSILON__ 2.2204460492503131e-16

double nDeriv(const char* expression, float& x) {
    double h = sqrt(__DBL_EPSILON__);
    return (eval(expression, x + h) - eval(expression, x)) / h;
}

std::mutex integralMutex;

void fnInt(std::string expression, StateManager* sm, float n) {
    float a = sm->intLowerBound;
    float b = sm->intUpperBound;
    const char* expr = expression.c_str();
    double delta_x = (b - a) / n;
    double area = 0;
    for (int i = 0; i < n; ++i) {
        area += eval(expr, a + i * delta_x) * delta_x;
    }

    std::lock_guard<std::mutex> lock(integralMutex);
    sm->integralCalculated = true;
    sm->integralResult = area;
}

double func_max(const char* expression, float a, float b) {
    double result = 0;
    for (double i = a; i <= b; i += (b - a) / gap) {
        result = std::max(eval(expression, i), result);
    }
    return result;
}