#pragma once
#include "stateManager.h"

bool exists(const std::string& name);
void saveGraphs(const std::vector<Graph*>& graphs, const char* filename);
void loadGraphs(std::vector<Graph*>&graphs, const char* filename);
void saveGraphsAs(StateManager& sm, std::vector<Graph*>& graphs);
