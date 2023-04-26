#ifndef _DATASTRUCTURE_H
#define _DATASTRUCTURE_H

#include <vector>
#include <stdlib.h>

class Point {
public:
    Point(int xx = 0, int yy = 0)
            : x(xx), y(yy) {}

    ~Point() {}

    bool operator==(const Point &p1) {
        return x == p1.x && y == p1.y;
    }

    int x;
    int y;
    std::vector<int> neighbors;
};

class Edge {
public:
    Edge(int pp1, int pp2, int w)
            : p1(pp1), p2(pp2), weight(w),
              left(-1), right(-1) {}

    ~Edge() {}

    bool operator==(const Edge &c1) {
        return p1 == c1.p1 && p2 == c1.p2;
    }

    int p1, p2;
    int weight;
    int left, right;
};

class Boundary {
public:
    Boundary(int xMin, int xMax, int yMin, int yMax)
            : xleft(xMin), xright(xMax), ybot(yMin), ytop(yMax) {}

    ~Boundary() {}

    int xleft, xright, ybot, ytop;
};

class Reroute {
public:
    Reroute(int xxNet = 0, int yyNet = 0, int xx = 0, int yy = 0)
            : xNet(xxNet), yNet(yyNet), x(xx), y(yy) {}

    ~Reroute() {}

    int xNet, yNet, x, y;
};

#endif