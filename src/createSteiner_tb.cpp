#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <numeric>
#include <cassert>
#include <vector>
#include "Steiner.h"
#include <tuple>

#include <cstring>
#include <cstdio>
#include "util.h"

bool gDoplot = true; //needed for solving

int main() {
    //Definitions
    std::vector<std::vector<Point>> FullNetlist, TempNet;
    std::vector<std::vector<Point>> ReleventNetlist;
    std::vector<Boundary> RestrictedArea;
    Point p;
    Boundary area = Boundary(0, 100, 0, 100);
    const std::vector<std::string> colors = {"purple", "green", "orange", "black"};
    //creating a sample netlist
    int numNets = 2;
    int numPins = 10;
    for (int k = 0; k < numNets; k++) {
        std::vector<Point> TotalPoints;
        for (int i = 0; i < numPins; i++) {
            p.x = (rand() % 100);
            p.y = rand() % 100;
            TotalPoints.push_back(p);
        }
        FullNetlist.push_back(TotalPoints);
    }
    TempNet = FullNetlist;
    //erasing a single element from a vector of vectors
    //TempNet[0].erase(TempNet[0].begin() + 1);

    std::cout << "TempNet: " << std::to_string(TempNet[0].at(1).x) << std::endl;
    std::cout << "FullNetlist: " << std::to_string(FullNetlist[0].at(1).x) << std::endl;
    std::vector<Steiner> allSteiners;
    std::vector<std::vector<std::vector<int>>> horizontal, vertical;
    for (int i = 0; i < FullNetlist.size(); i++) {
        Steiner test;
        test.createSteiner("createSt_tb", FullNetlist[i], area);
        allSteiners.push_back(test);
    }

    //multiple solves
    for (int i = 0; i < allSteiners.size(); i++) {
        allSteiners[i].solve();
    }

    std::ofstream outputFile("createSt_tb.plt", std::ofstream::out);
    //intialize File
    int index = allSteiners[0].initializeFile(outputFile);
    std::cout << "beforeplotMultiple" << std::endl;
    //multiple plots
    for (int i = 0; i < allSteiners.size(); i++) {
        std::cout << "loop " << i << std::endl;

        index = allSteiners[i].plotMultiple(outputFile, index, horizontal, vertical, colors.at(i % colors.size()));
    }

    //assumes input to be a vector of ints with an index compared with a vector of a vector of ints
    for (int i = 0; i < horizontal.size(); i++) {
        checkNets(outputFile, i, horizontal[i], vertical);
    }

    outputFile << "plot 1000000000" << std::endl;
    outputFile << "pause -1 'Press any key'" << std::endl;
    outputFile.close();

    // std::vector<int> tempMST = allSteiners[0].getMST();
    // std::cout << tempMST.size() << std::endl;
    return 0;
}