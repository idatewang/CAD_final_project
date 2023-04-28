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
    // const std::vector<std::string> colors = {"purple", "green", "orange", "black"};
    const std::vector<std::string> colors = {"red", "orange", "yellow", "green", "blue", "violet", "black", "brown"};
    const std::vector<std::string> inputNets = {"../testbench/case_0.txt", "../testbench/case_1.txt", "../testbench/case_2.txt", "../testbench/case_3.txt"};
//    const std::vector<std::string> inputNets = {"../testbench/case1", "../testbench/case2"};
    //creating a sample netlist
    int numNets = 3;
    int numPins = 10;
    //srand(time(NULL));

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
    for (int i = 0; i < TempNet.size(); i++) {
        for (int j = 0; j < TempNet[i].size(); j++) {
            std::cout << std::to_string(TempNet[i].at(j).x) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << std::endl;

    TempNet.erase(TempNet.begin());
    //erasing a single element from a vector of vectors
    for (int i = 0; i < TempNet.size(); i++) {
        for (int j = 0; j < TempNet[i].size(); j++) {
            std::cout << std::to_string(TempNet[i].at(j).x) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "TempNet: " << std::to_string(TempNet[0].at(1).x) << std::endl;
    TempNet[0].erase(TempNet[0].begin() + 1);

    std::cout << "TempNet: " << std::to_string(TempNet[0].at(1).x) << std::endl;
    std::cout << "FullNetlist: " << std::to_string(FullNetlist[0].at(1).x) << std::endl;


    std::vector<Steiner> allSteiners;
    std::vector<std::vector<std::vector<int>>> horizontal, vertical;

    std::vector<std::vector<std::vector<std::vector<int>>>> edgeList;
    std::vector<std::vector<Point>> nodeList;

    // //from created Netlist
    // for (int i = 0; i < FullNetlist.size(); i++) {
    // Steiner test;
    // test.createSteiner("createSt_tb", FullNetlist[i], area);
    // allSteiners.push_back(test);
    // }
    // from case txt
    for (int i = 0; i < inputNets.size(); i++) {
        Steiner test;
        test.parse(inputNets.at(i));
        allSteiners.push_back(test);
        for (int j = 0; j < test.getPoints().size(); ++j) {
            cout<<test.getPoints()[j].x;
        }
    }
    vector<vector<Point>> pin_nodes;
    for (int i = 0; i < inputNets.size(); ++i) {
        vector<Point> temp;
        pin_nodes.push_back(temp);
    }
    for (int i = 0; i < allSteiners.size(); ++i) {
        for (int j = 0; j < allSteiners[i].getPoints().size(); ++j) {
            pin_nodes[i].emplace_back(allSteiners[i].getPoints().at(j).x,allSteiners[i].getPoints().at(j).y);
        }
    }
    //multiple solves
    for (int i = 0; i < allSteiners.size(); i++) {
        allSteiners[i].solve();
    }
    std::ofstream outputFile("createSt_tb.plt", std::ofstream::out);
    std::ofstream revisedFile("createSt_tb2.plt", std::ofstream::out);
    std::ofstream revisedAgainFile("createSt_tb3.plt", std::ofstream::out);
    //intialize File
    int index = allSteiners[0].initializeFile(outputFile);
    int index2 = allSteiners[0].initializeFile(revisedFile);
    int index3 = allSteiners[0].initializeFile(revisedAgainFile);
    std::vector<Reroute> errors;
    std::cout << "beforeplotMultiple" << std::endl;
    // index = allSteiners[1].plotMultiple(outputFile, index, edgeList, colors.at(1 % colors.size()));
    //multiple plots
    for (int i = 0; i < allSteiners.size(); i++) {
        std::cout << "loop " << i << std::endl;

        index = allSteiners[i].plotMultiple(outputFile, index, edgeList, nodeList, colors.at(i % colors.size()));
    }
    std::vector<std::vector<std::vector<std::vector<int>>>> edgeList_cp = edgeList;

    //assumes input to be a vector of ints with an index compared with a vector of a vector of ints
    checkNets(outputFile, errors, edgeList);


    int bound_x = allSteiners[0].get_bounds()[2] - allSteiners[0].get_bounds()[0];
    int bound_y = allSteiners[0].get_bounds()[3] - allSteiners[0].get_bounds()[1];
    map_generate(edgeList_cp,errors,pin_nodes,nodeList,bound_x,bound_y);




    for (int i = 0; i < allSteiners.size(); i++) {
        // std::cout << "loop revised " << i << std::endl;

        index2 = allSteiners[i].plotFixed(revisedFile, index2, edgeList, colors, i);
    }

    //removes safe spaces that do not have a point
    for (int i = 0; i < allSteiners.size(); i++) {
        std::cout << "loop revised " << i << std::endl;

        allSteiners[i].cleanNetlist(edgeList, i);
    }
    //revisedFile plot
    for (int i = 0; i < allSteiners.size(); i++) {
        // std::cout << "loop revised " << i << std::endl;

        index2 = allSteiners[i].plotFixed(revisedAgainFile, index3, edgeList, colors, i);
    }

    outputFile << "plot 1000000000" << std::endl;
    outputFile << "pause -1 'Press any key'" << std::endl;
    outputFile.close();

    revisedFile << "plot 1000000000" << std::endl;
    revisedFile << "pause -1 'Press any key'" << std::endl;
    revisedFile.close();

    revisedAgainFile << "plot 1000000000" << std::endl;
    revisedAgainFile << "pause -1 'Press any key'" << std::endl;
    revisedAgainFile.close();
    // // std::vector<int> tempMST = allSteiners[0].getMST();
    // // std::cout << tempMST.size() << std::endl;
    return 0;
}