#include <string>
#include <string.h>
#include "stateManager.h"

StateManager::StateManager() {
    currentFuncIndex = 0;
    currentCalcIndex = 0;
    frameDelay = 0;

    mainActive = true;
    panelActive = true;
    calcActive = false;
    polarPanelActive = false;
    imGuiFocused = false;
    screenshotFlag = false;

    saveAsFlag = false;
    saveFlag = false;

    strcpy_s(comboPreview, 30, "Select a function");
    strcpy_s(calcPreview, 30, "Select a calculation");

    currentFileOpen = "";
    x_coor = 0;
    intLowerBound = 0;
    intUpperBound = 0;
    maxLowerBound = 0;
    maxUpperBound = 0;

    integralResult = 0;
    derivativeResult = 0;
    maxResult = 0;
    integralCalculated = false;
    derivativeCalculated = false;
    maxCalculated = false;

    strcpy_s(options[0], 10, "nDeriv");
    strcpy_s(options[1], 10, "fnInt");
    strcpy_s(options[2], 10, "max");
    numOptions = 3;
}