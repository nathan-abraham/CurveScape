#include <iostream>
#include <fstream>
#include <regex>
#include <string.h>
#include <SFML/Graphics.hpp>
#include "tinyxml2/tinyxml2.h"
#include "graph.h"
#include "fileOperations.h"

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