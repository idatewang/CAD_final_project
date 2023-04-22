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
    const std::vector<std::string> inputNets = {"../testbench/case1", "../testbench/case2"};
    //creating a sample netlist
    int numNets = 2;
    int numPins = 10;
    srand(time(NULL));
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
    //from created Netlist
    for (int i = 0; i < FullNetlist.size(); i++) {
        Steiner test;
        test.createSteiner("createSt_tb", FullNetlist[i], area);
        allSteiners.push_back(test);
    }
    // from case txt
    // for (int i = 0; i < inputNets.size(); i++) {
    //     Steiner test;
    //     test.parse(inputNets.at(i));
    //     allSteiners.push_back(test);
    // }

    //multiple solves
    for (int i = 0; i < allSteiners.size(); i++) {
        allSteiners[i].solve();
    }

    std::ofstream outputFile("createSt_tb.plt", std::ofstream::out);
    std::ofstream revisedFile("createSt_tb2.plt", std::ofstream::out);
    //intialize File
    int index = allSteiners[0].initializeFile(outputFile);
    int index2 = allSteiners[0].initializeFile(revisedFile);
    std::vector<std::vector<Reroute>> errors;
    std::cout << "beforeplotMultiple" << std::endl;
    // index = allSteiners[1].plotMultiple(outputFile, index, horizontal, vertical, colors.at(1 % colors.size()));
    //multiple plots
    for (int i = 0; i < allSteiners.size(); i++) {
        std::cout << "loop " << i << std::endl;

        index = allSteiners[i].plotMultiple(outputFile, index, horizontal, vertical, colors.at(i % colors.size()));
    }


    //assumes input to be a vector of ints with an index compared with a vector of a vector of ints
    checkNets(outputFile, errors, horizontal, vertical);

    fixError(errors, horizontal, vertical);
    //revisedFile plot
    for (int i = 0; i < allSteiners.size(); i++) {
        // std::cout << "loop revised " << i << std::endl;

        index2 = allSteiners[i].plotFixed(revisedFile, index2, horizontal, vertical, colors, i);
    }

    // Reroute x = Reroute(0, 0, 0, 0, 10, 0, 10, 0);
    // Reroute y = Reroute(0, 0, 0, 0, 10, 0, 10, 5);
    // if(x < y)
    // {
    // std::cout << "This is incorrect" << std::endl;
    // }
    // else
    // {
    // std::cout << "Function is working" << std::endl;
    // }
    outputFile << "plot 1000000000" << std::endl;
    outputFile << "pause -1 'Press any key'" << std::endl;
    outputFile.close();

    revisedFile << "plot 1000000000" << std::endl;
    revisedFile << "pause -1 'Press any key'" << std::endl;
    revisedFile.close();
    fixError(errors, horizontal, vertical);
    // std::vector<int> tempMST = allSteiners[0].getMST();
    // std::cout << tempMST.size() << std::endl;
    return 0;
}