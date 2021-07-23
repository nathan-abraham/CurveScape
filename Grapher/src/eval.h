#pragma once

#include "tinyexpr/tinyexpr.h"
#include <utility>

double eval(double, double(*func)(double));

double eval(const char* input, double x);

std::string clean(std::string expression);
