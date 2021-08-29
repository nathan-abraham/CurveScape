#include <string>
#include "eval.h"


double eval(double x, double(*func)(double)) {
    return func(x);
}

double eval(const char* input, double x) {
    te_variable vars[] = {
        {"x", &x}
    };
    int error;

    te_expr* expr = te_compile(input, vars, 1, &error);
    if (expr) {
        double result = te_eval(expr);
        te_free(expr);
        return result;
    }
    return NAN;
}

std::string clean(std::string expression) {
    for (int i = 1; i < expression.size(); i++) {
        if ((isdigit(expression[i - 1]) || expression[i - 1] == 'x') && expression[i] == 'x') {
            expression.insert(i, 1, '*');
        }
    }
    return expression;
}
