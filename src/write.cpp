#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <numeric>
#include <cassert>
#include <vector>
#include <string>
#include "Steiner.h"

//PIN p0 (94,60)
void printPins(std::ofstream &file, std::vector <Point> pin) {
    file << "NumPins = " << pin.size() << std::endl;
    for (int i = 0; i < pin.size(); i++) {
        file << "PIN p" << i << " (" << pin.at(i).x << "," << pin.at(i).y << ")" << std::endl;
    }
}

void printFile(std::ofstream &output, std::vector <std::vector<Point>> test) {
    //std::ofstream output(temp);
    output << "Hello world!" << std::endl;
    for (int i = 0; i < test.size(); i++) {
        printPins(output, test[i]);
    }
    output.close();
}

int main() {
    //Definitions
    std::vector <std::vector<Point>> FullNetlist, TempNet;
    std::vector <std::vector<Point>> ReleventNetlist;
    std::vector <Boundary> RestrictedArea;
    Point p;
    Boundary area = Boundary(0, 50, 0, 50);

    //creating a sample netlist
    int numNets = 2;
    int numPins = 10;
    for (int k = 0; k < numNets; k++) {
        std::vector <Point> TotalPoints;
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

    //Finding points that are inside the boundary
    for (int k = 0; k < TempNet.size(); k++) {
        std::vector <Point> Relevent;
        for (int i = 0; i < TempNet[k].size(); i++) {
            Point check = TempNet[k].at(i);
            if ((check.x >= area.xleft) & (check.x <= area.xright) & (check.y >= area.ybot) & (check.y <= area.ytop)) {
                Relevent.push_back(TempNet[k].at(i));
                TempNet[k].erase(TempNet[k].begin() + i);
                i--;
            }
        }
        ReleventNetlist.push_back(Relevent);
    }

    //Finding the point outside of the boundary within the netlist
    for (int k = 0; k < TempNet.size(); k++) {
        int outside = 0;
        int inside = 0;
        int minLength = abs(TempNet[k].at(0).x - ReleventNetlist[k].at(0).x) + abs(TempNet[k].at(0).y - ReleventNetlist[k].at(0).y);
        for (int i = 0; i < TempNet[k].size(); i++) {
            Point candidate = TempNet[k].at(i);
            for (int j = 0; j < ReleventNetlist[k].size(); j++) {
                int testLength = abs(candidate.x - ReleventNetlist[k].at(j).x) + abs(candidate.y - ReleventNetlist[k].at(j).y);
                if (testLength < minLength) {
                    minLength = testLength;
                    outside = i;
                    inside = j;

                }
            }
        }
        ReleventNetlist[k].push_back(TempNet[k].at(outside));
        TempNet[k].push_back(ReleventNetlist[k].at(inside));
    }
    //sample output file name
    std::string temp = "output.txt";

    //other ways to write a string
    std::string full = "temp" + std::to_string(24) + temp;


    std::ofstream option(temp);
    std::ofstream option2("output2.txt");
    std::ofstream option3("output3.txt");
    printFile(option, FullNetlist);
    option.close();
    printFile(option2, TempNet);
    option2.close();
    printFile(option3, ReleventNetlist);
    option3.close();
    std::cout << temp << std::endl;

    return 0;
}


