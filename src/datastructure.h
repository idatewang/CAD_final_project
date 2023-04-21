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
    Reroute(int NetNumber = 0, int V = 0, int issue = 0, int issuePointt = 0, int hhead = 0, int ttail = 0, int leftWing = 0, int rightWing = 0)
            : netNum(NetNumber), isV(V), movingNet(issue), issuePoint(issuePointt), head(hhead), tail(ttail), lWing(leftWing), rWing(rightWing) {}

    ~Reroute() {}

    bool operator==(const Reroute &c1) {
        return head == c1.head && lWing == c1.lWing && rWing == c1.rWing;
    }

    bool operator<(const Reroute &c1) {
        int sum = (2 * abs(tail - head)) + abs(lWing - rWing);
        int c1Sum = (2 * abs(c1.tail - c1.head)) + abs(c1.lWing - c1.rWing);
        bool solution = (sum < c1Sum);
        if (sum == c1Sum) {
            solution = (abs(issuePoint - head) * abs(lWing - rWing)) < (abs(c1.issuePoint - c1.head) * abs(c1.lWing - c1.rWing));
        }
        return solution;
    }

    int netNum, isV, movingNet, issuePoint, head, tail, lWing, rWing;
};

#endif