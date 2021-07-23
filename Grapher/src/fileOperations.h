#pragma once

bool exists(const std::string& name);
void saveGraphs(const std::vector<Graph*>& graphs, const char* filename);
void loadGraphs(std::vector<Graph*>&graphs, const char* filename);
