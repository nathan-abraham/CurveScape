#include <iostream>
#include <fstream>
#include <regex>
#include <string.h>

#include <SFML/Graphics.hpp>
#include <nfd.h>
#include "graph.h"
#include "fileOperations.h"
#include "stateManager.h"

#define LOG(x) std::cout << (x) << std::endl;

bool exists(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

void saveGraphs(const std::vector<Graph*>& graphs, const char* filename) {
    std::ofstream fout;
    fout.open(filename, std::ofstream::out | std::ofstream::trunc);
    fout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    for (Graph* graph : graphs) {
        fout << "<function>" << graph->expression << "</function>\n";
    }
}

void loadGraphs(std::vector<Graph*>& graphs, const char* filename) {
    std::string line;
    std::ifstream fin(filename);

    if (fin.good()) {
        int idx = 0;
        while (std::getline(fin, line)) {
            if (line.size() > 5 && line.substr(0, 5) != "<?xml") {
                std::smatch matches;
                std::regex pattern("<function>(.*)</function>");

                while (std::regex_search(line, matches, pattern)) {
                    strcpy_s(graphs[idx]->expression, 45, matches.str(1).c_str());
                    idx++;
                    line = matches.suffix().str();
                }
            }
        }
        for (Graph* graph : graphs) {
            graph->parseExpr();
            graph->updatePoints();
        }
    }
}

void saveGraphsAs(StateManager& sm, std::vector<Graph*>& graphs) {
    LOG("main loop save as")
    nfdchar_t *rawFilename;
    nfdresult_t result = NFD_SaveDialog("png;jpg;graph", NULL, &rawFilename);

    if (result == NFD_OKAY)
    {
        sm.currentFileOpen = rawFilename;
        if (sm.currentFileOpen.find(".graph") != std::string::npos)
        {
            saveGraphs(graphs, rawFilename);
        }
        else
        {
            sm.imageFilename = rawFilename;

            if (sm.imageFilename.substr(sm.imageFilename.size() - 4).find(".") == std::string::npos)
            {
                LOG("added extension")
                sm.imageFilename += ".png";
            }
            sm.mainActive = false;
            sm.panelActive = false;
            sm.calcActive = false;
            sm.screenshotFlag = true;
            sm.frameDelay = 0;
        }
    }
    else if (result == NFD_CANCEL)
    {
        LOG("canceled saving")
    }
    else
    {
        LOG(NFD_GetError());
    }
    sm.saveAsFlag = false;
}