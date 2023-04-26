#ifndef _STEINER_H
#define _STEINER_H

#include <vector>
#include <string>
#include <tuple>
#include "datastructure.h"
#include "findPath.h"

class Steiner {
public:
    Steiner() :
            _name(""), _boundaryLeft(-1), _boundaryRight(-1),
            _boundaryTop(-1), _boundaryBottom(-1),
            _MST_cost(0), _MRST_cost(0) {}

    ~Steiner() {}

    void parse(const std::string &fileName);

    void createSteiner(const std::string &fileName, std::vector<Point> Nets, Boundary Bounds);

    void solve();

    void plot(const std::string &plotName);

    int initializeFile(std::ofstream &of);

    int plotMultiple(std::ofstream &file, int idx, std::vector<std::vector<std::vector<std::vector<int>>>> &edgeList,
                     std::vector<std::vector<Point>> &nodeList, std::string color);

    int plotFixed(std::ofstream &file, int idx, std::vector<std::vector<std::vector<std::vector<int>>>> &edgeList, std::vector<std::string> color, int initialColor);

    void cleanNetlist(std::vector<std::vector<std::vector<std::vector<int>>>> &edgeList, int steinerNum);

    std::vector<Point> getPoints(); //needs a getter
    std::vector<Edge> getEdges(); //needs a getter
    std::vector<int> getMST(); //needs a getter
    std::vector<bool> getEdges_del(); //neeeds a getter
    void outfile(const std::string &outfileName);

    // helper getters
    std::vector<Point> get_points() {
        return _points;
    }

    std::string get_name() {
        return _name;
    }

    int* get_bounds() {
//        std::vector<int> bounds;
//        bounds.push_back(_boundaryBottom);
//        bounds.push_back(_boundaryLeft);
//        bounds.push_back(_boundaryTop);
//        bounds.push_back(_boundaryRight);
        int bounds[4];
        bounds[0] = _boundaryBottom;
        bounds[1] = _boundaryLeft;
        bounds[2] = _boundaryTop;
        bounds[3] = _boundaryRight;
        return bounds;
    }

    // helper setters
    void set_bounds(int bottom, int left, int top, int right) {
        _boundaryBottom = bottom;
        _boundaryLeft = left;
        _boundaryTop = top;
        _boundaryRight = right;
    }

    void set_points(Point point) {
        _points.push_back(point);
    }

private: // helper functions
    // parse ---------------------
    void addEdge(int p1, int p2);

    // solve ---------------------
    unsigned findSet(int pId);

    void unionSet(int x, int y, int z);

    void init();

    void buildRSG();

    void buildMST();

    void buildRST();

    // LCA -----------------------
    int tarfind(int x);

    void tarunion(int x, int y);

    void tarjanLCA(int x);

private: // members
    std::string _name;
    int _boundaryLeft, _boundaryRight;
    int _boundaryTop, _boundaryBottom;
    int _init_p;
    int _root;
    std::vector<Point> _points;
    std::vector<Edge> _edges;
    std::vector<int> _set;
    std::vector<int> _MST;
    std::vector<bool> _edges_del;
    std::vector<std::vector<int>> _lca_place; // adj-list of index
    std::vector<std::tuple<int, int, int>> _lca_queries; // p, p, e
    std::vector<int> _lca_answer_queries; // longest e
    // tarjan -------------
    std::vector<bool> _visit;
    std::vector<int> _ancestor;
    std::vector<int> _par;
    std::vector<int> _rank;
    //----------------------
    std::vector<std::tuple<int, int, int, int>> _table;
    std::vector<int> _table_cnt;
    std::vector<Edge> _newE;

    long long _MST_cost;
    long long _MRST_cost;
    // plot ----------------
    std::vector<Edge> _init_edges;
    std::vector<int> _init_MST;
};

void checkNets(std::ofstream &file, std::vector<Reroute> &errors, std::vector<std::vector<std::vector<std::vector<int>>>> &edgeList);

// void fixError(std::vector<std::vector<Reroute>> &errors, std::vector<std::vector<std::vector<std::vector<int>>>> &edgeList, int buffer);

void mark_delete(int **grid, Position start, int bound_x, int bound_y, vector<vector<Position>> &path);

void source_propagate(int **grid, Position start, int bound_x, int bound_y, vector<Position> &path);

void map_generate(vector<std::vector<std::vector<std::vector<int>>>> edge, std::vector<Reroute> intersect, std::vector<Point> pin, std::vector<Point> node, int bound_x, int bound_y, int tree_order);

bool FindPath(int **grid, Position start, Position finish, int &PathLen, Position *&path, int n, int m);
#endif