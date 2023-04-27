#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <numeric>
#include <cassert>
#include "util.h"
#include "Steiner.h"
#include <unistd.h>

using namespace std;

inline void split(string &str, const string &delim, vector<string> &v) {
    size_t pos = 0;
    string token;
    v.clear();
    while ((pos = str.find(delim)) != string::npos) {
        token = str.substr(0, pos);
        str.erase(0, pos + delim.size());
        v.push_back(token);
    }
    v.push_back(str);
}

inline Point string2Point(string str) {
    vector<string> tokens;
    split(str, ",", tokens);
    int x = stoi(tokens[0].substr(1));
    tokens[1].pop_back();
    int y = stoi(tokens[1]);
    return Point(x, y);
}

inline string getFileName(const string &filePathName, bool getFile) {
    string retStr = filePathName;
    string::size_type pos = retStr.rfind("/");
    if (pos != string::npos) {
        if (getFile) retStr = retStr.substr(pos + 1);
        else retStr = retStr.substr(0, pos);
    }
    return retStr;
}

void Steiner::parse(const string &fileName) {
    _name = getFileName(fileName, true);
    ifstream ifs(fileName, ifstream::in);
    string buf;
    Point p;
    ifs >> buf >> buf >> buf;
    buf.pop_back();
    p = string2Point(buf);
    _boundaryLeft = p.x;
    _boundaryBottom = p.y;
    ifs >> buf;
    p = string2Point(buf);
    _boundaryRight = p.x;
    _boundaryTop = p.y;

    ifs >> buf >> buf >> buf;
    _points.resize(stoi(buf));
    _init_p = _points.size();
    for (unsigned i = 0; i < _points.size(); ++i) {
        ifs >> buf >> buf >> buf;
        _points[i] = string2Point(buf);
    }
    ifs.close();
}

void Steiner::init() {
    _edges.clear();
    _set.clear();
    _MST.clear();
    _edges_del.clear();
    _lca_place.clear();
    _lca_queries.clear();
    _lca_answer_queries.clear();
    _visit.clear();
    _ancestor.clear();
    _par.clear();
    _rank.clear();
    _newE.clear();
    _table.clear();
    _table_cnt.clear();
}

extern bool gDoplot;

void Steiner::solve() {
#ifdef VERBOSE
    TimeUsage timer;
#endif
    buildRSG();
#ifdef VERBOSE
    timer.showUsage("buildRSG", TimeUsage::PARTIAL);
    timer.start(TimeUsage::PARTIAL);
#endif
    buildMST();
#ifdef VERBOSE
    timer.showUsage("buildMST", TimeUsage::PARTIAL);
    timer.start(TimeUsage::PARTIAL);
#endif
    if (gDoplot) {
        _init_edges = _edges;
        _init_MST = _MST;
    }
    for (int eId: _MST) _MST_cost += _edges[eId].weight;
    buildRST();
#ifdef VERBOSE
    timer.showUsage("buildRST", TimeUsage::PARTIAL);
    timer.start(TimeUsage::PARTIAL);
#endif
    unsigned numIter = 2;
    if (_init_p >= 10) numIter = 2;
    if (_init_p >= 100) numIter = 3;
    if (_init_p >= 500) numIter = 4;
    for (unsigned iter = 1; iter < numIter; ++iter) {
        init();
        buildRSG();
#ifdef VERBOSE
        timer.showUsage("buildRSG", TimeUsage::PARTIAL);
        timer.start(TimeUsage::PARTIAL);
#endif
        buildMST();
#ifdef VERBOSE
        timer.showUsage("buildMST", TimeUsage::PARTIAL);
        timer.start(TimeUsage::PARTIAL);
#endif
        buildRST();
#ifdef VERBOSE
        timer.showUsage("buildRST", TimeUsage::PARTIAL);
        timer.start(TimeUsage::PARTIAL);
#endif
    }
    for (auto &e: _newE) _MRST_cost += e.weight;
    for (int eId: _MST) {
        if (_edges_del[eId]) continue;
        _MRST_cost += _edges[eId].weight;
    }
    cerr << _name << endl;
    cerr << "RSG edge   : " << _edges.size() << endl;
    cerr << "MST length : " << _MST_cost << endl;
    cerr << "MRST length: " << _MRST_cost << endl;
    cerr << "Improvement: "
         << (double) (_MST_cost - _MRST_cost) / _MST_cost * 100
         << "%" << endl;
}

void Steiner::addEdge(int p1, int p2) {
    if (p1 == p2) return;
    int weight = abs(_points[p1].x - _points[p2].x) +
                 abs(_points[p1].y - _points[p2].y);
    _edges.emplace_back(Edge(p1, p2, weight));
    _points[p1].neighbors.emplace_back(p2);
    _points[p2].neighbors.emplace_back(p1);
}

void Steiner::buildRSG() {
    vector<int> order1, order2;
    order1.resize(_points.size());
    iota(order1.begin(), order1.end(), 0);
    order2 = order1;
    sort(order1.begin(), order1.end(),
         [&](int i1, int i2) {
             return _points[i1].x + _points[i1].y < _points[i2].x + _points[i2].y;
         });
    sort(order2.begin(), order2.end(),
         [&](int i1, int i2) {
             return _points[i1].x - _points[i1].y < _points[i2].x - _points[i2].y;
         });
    vector<int> A1, A2;
    A1.reserve(_points.size());
    A2.reserve(_points.size());
    for (int pId: order1) {
        Point &p = _points[pId];
        if (!A1.empty()) {
            auto it = A1.end();
            do {
                --it;
                Point &tmp = _points[*it];
                if (p.y - tmp.y >= p.x - tmp.x &&
                    p.y - tmp.y > 0 &&
                    p.x - tmp.x > 0) {
                    addEdge(pId, *it);
                    it = A1.erase(it);
                    //break;
                }
            } while (it != A1.begin());
        }
        if (!A2.empty()) {
            auto it = A2.end();
            do {
                --it;
                Point &tmp = _points[*it];
                if (p.y - tmp.y < p.x - tmp.x &&
                    p.y - tmp.y >= 0 &&
                    p.x - tmp.x > 0) {
                    addEdge(pId, *it);
                    it = A2.erase(it);
                    //break;
                }
            } while (it != A2.begin());
        }
        A1.emplace_back(pId);
        A2.emplace_back(pId);
    }
    A1.clear();
    A2.clear();
    for (int pId: order2) {
        Point &p = _points[pId];
        if (!A1.empty()) {
            auto it = A1.end();
            do {
                --it;
                Point &tmp = _points[*it];
                if (tmp.y - p.y <= p.x - tmp.x &&
                    p.y - tmp.y < 0 &&
                    p.x - tmp.x > 0) {
                    addEdge(pId, *it);
                    it = A1.erase(it);
                    //break;
                }
            } while (it != A1.begin());
        }
        if (!A2.empty()) {
            auto it = A2.end();
            do {
                --it;
                Point &tmp = _points[*it];
                if (tmp.y - p.y > p.x - tmp.x &&
                    p.y - tmp.y < 0 &&
                    p.x - tmp.x >= 0) {
                    addEdge(pId, *it);
                    it = A2.erase(it);
                    //break;
                }
            } while (it != A2.begin());
        }
        A1.emplace_back(pId);
        A2.emplace_back(pId);
    }

}

unsigned Steiner::findSet(int x) {
    return x == _set[x] ? _set[x] : (_set[x] = findSet(_set[x]));
}

void Steiner::unionSet(int x, int y, int z) {
    int xroot = findSet(x);
    int yroot = findSet(y);
    assert (xroot != yroot);
    _set[xroot] = z;
    _set[yroot] = z;
}

void Steiner::buildMST() {
    sort(_edges.begin(), _edges.end(),
         [](Edge e1, Edge e2) {
             return e1.weight < e2.weight;
         });
    _set.resize(_edges.size() + _points.size());
    iota(_set.begin(), _set.end(), 0);
    _lca_place.resize(_points.size());
    for (unsigned i = 0; i < _edges.size(); ++i) {
        Edge &e = _edges[i];
        unsigned head1 = findSet(e.p1 + _edges.size());
        unsigned head2 = findSet(e.p2 + _edges.size());
        if (head1 != head2) {
            set<int> neighbors;
            for (int n: _points[e.p1].neighbors) neighbors.emplace(n);
            for (int n: _points[e.p2].neighbors) neighbors.emplace(n);
            neighbors.erase(e.p1);
            neighbors.erase(e.p2);
            for (int w: neighbors) {
                if (head1 == findSet(w + _edges.size())) {
                    _lca_place[w].emplace_back(_lca_queries.size());
                    _lca_place[e.p1].emplace_back(_lca_queries.size());
                    _lca_queries.emplace_back(w, e.p1, i);
                } else {
                    _lca_place[w].emplace_back(_lca_queries.size());
                    _lca_place[e.p2].emplace_back(_lca_queries.size());
                    _lca_queries.emplace_back(w, e.p2, i);
                }
            }
            unionSet(head1, head2, i);
            e.left = head1;
            e.right = head2;
            _MST.emplace_back(i);
        }
    }
    _root = findSet(0);
}

int Steiner::tarfind(int x) {
    return x == _par[x] ? _par[x] : (_par[x] = tarfind(_par[x]));
}

void Steiner::tarunion(int x, int y) {
    int xroot = tarfind(x);
    int yroot = tarfind(y);
    if (_rank[xroot] > _rank[yroot]) _par[yroot] = xroot;
    else if (_rank[xroot] < _rank[yroot]) _par[xroot] = yroot;
    else {
        _par[yroot] = xroot;
        ++_rank[xroot];
    }
}

void Steiner::tarjanLCA(int x) {
    _par[x] = x;
    _ancestor[x] = x;
    if (x < (int) _edges.size()) {
        tarjanLCA(_edges[x].left);
        tarunion(x, _edges[x].left);
        _ancestor[tarfind(x)] = x;
        tarjanLCA(_edges[x].right);
        tarunion(x, _edges[x].right);
        _ancestor[tarfind(x)] = x;
    }
    _visit[x] = true;
    if (x >= (int) _edges.size()) {
        int u = x - _edges.size();
        for (unsigned i = 0; i < _lca_place[u].size(); ++i) {
            int which = _lca_place[u][i];
            int v = get<0>(_lca_queries[which]) == u ?
                    get<1>(_lca_queries[which]) : get<0>(_lca_queries[which]);
            v += _edges.size();
            if (_visit[v]) _lca_answer_queries[which] = _ancestor[tarfind(v)];
        }
    }
}

void Steiner::buildRST() {
    _lca_answer_queries.resize(_lca_queries.size());
    _visit.resize(_edges.size() + _points.size());
    _ancestor.resize(_edges.size() + _points.size());
    _par.resize(_edges.size() + _points.size());
    _rank.resize(_edges.size() + _points.size());
    tarjanLCA(_root);
    _table.reserve(_lca_queries.size());
    _table_cnt.resize(_edges.size());
    for (unsigned i = 0; i < _lca_queries.size(); ++i) {
        int p = get<0>(_lca_queries[i]);
        int ae = get<2>(_lca_queries[i]);
        int de = _lca_answer_queries[i];
        Point &pnt = _points[p];
        Edge &add_e = _edges[ae];
        Edge &del_e = _edges[de];
        int gain = del_e.weight;
        int mxx = max(_points[add_e.p1].x, _points[add_e.p2].x);
        int mnx = min(_points[add_e.p1].x, _points[add_e.p2].x);
        int mxy = max(_points[add_e.p1].y, _points[add_e.p2].y);
        int mny = min(_points[add_e.p1].y, _points[add_e.p2].y);
        if (pnt.x < mnx) gain -= mnx - pnt.x;
        else if (pnt.x > mxx) gain -= pnt.x - mxx;
        if (pnt.y < mny) gain -= mny - pnt.y;
        else if (pnt.y > mxy) gain -= pnt.y - mxy;
        if (gain > 0) {
            ++_table_cnt[ae];
            ++_table_cnt[de];
            _table.emplace_back(p, ae, de, gain);
        }
    }
    auto sortByGain = [&](tuple<int, int, int, int> t1,
                          tuple<int, int, int, int> t2) {
        if (get<3>(t1) == get<3>(t2)) {
            return _table_cnt[get<1>(t1)] + _table_cnt[get<2>(t1)] <
                   _table_cnt[get<1>(t2)] + _table_cnt[get<2>(t2)];
        }
        return get<3>(t1) > get<3>(t2);
    };
    sort(_table.begin(), _table.end(), sortByGain);
#ifdef DEBUG
    for (unsigned i = 0; i < _table.size(); ++i) {
      cerr << get<0>(_table[i]) << " (" << _edges[get<1>(_table[i])].p1 << ","
           << _edges[get<1>(_table[i])].p2 << ")"
           << " (" << _edges[get<2>(_table[i])].p1 << ","
           << _edges[get<2>(_table[i])].p2 << ") "
           << get<3>(_table[i]) << endl;
    }
#endif
    _edges_del.resize(_edges.size());
    for (unsigned i = 0; i < _table.size(); ++i) {
        int ae = get<1>(_table[i]);
        int de = get<2>(_table[i]);
        if (_edges_del[ae] || _edges_del[de]) continue;
        Point p = _points[get<0>(_table[i])];
        Edge &add_e = _edges[ae];
        int mxx = max(_points[add_e.p1].x, _points[add_e.p2].x);
        int mnx = min(_points[add_e.p1].x, _points[add_e.p2].x);
        int mxy = max(_points[add_e.p1].y, _points[add_e.p2].y);
        int mny = min(_points[add_e.p1].y, _points[add_e.p2].y);
        int sx = p.x, sy = p.y;
        if (p.x < mnx) sx = mnx;
        else if (p.x > mxx) sx = mxx;
        if (p.y < mny) sy = mny;
        else if (p.y > mxy) sy = mxy;
        if (sx != p.x || sy != p.y) {
            int pId = get<0>(_table[i]);
            int new_pId = _points.size();
            Point new_p = Point(sx, sy);
            _points.emplace_back(new_p);
            int weight = abs(p.x - new_p.x) + abs(p.y - new_p.y);
            int weight1 = abs(_points[add_e.p1].x - new_p.x) +
                          abs(_points[add_e.p1].y - new_p.y);
            int weight2 = abs(_points[add_e.p2].x - new_p.x) +
                          abs(_points[add_e.p2].y - new_p.y);
            _newE.emplace_back(Edge(new_pId, pId, weight));
            _newE.emplace_back(Edge(new_pId, add_e.p1, weight1));
            _newE.emplace_back(Edge(new_pId, add_e.p2, weight2));
        }
        _edges_del[ae] = true;
        _edges_del[de] = true;
    }

}

void Steiner::plot(const string &plotName) {
    string ofileName = plotName;
    ofstream of(ofileName, ofstream::out);
    of << "set size ratio -1" << endl;
    of << "set nokey" << endl;
    of << "set xrange["
       << (_boundaryRight - _boundaryLeft) * -0.05 << ":"
       << (_boundaryRight - _boundaryLeft) * 1.05 << "]" << endl;
    of << "set yrange["
       << (_boundaryTop - _boundaryBottom) * -0.05 << ":"
       << (_boundaryTop - _boundaryBottom) * 1.05 << "]" << endl;
    int idx = 1;
    of << "set object " << idx++ << " rect from "
       << _boundaryLeft << "," << _boundaryBottom << " to "
       << _boundaryRight << "," << _boundaryTop << "fc rgb \"grey\" behind\n";
    // point
    for (int i = 0; i < _init_p; ++i) {
        of << "set object circle at first " << _points[i].x << ","
           << _points[i].y << " radius char 0.3 fillstyle solid "
           << "fc rgb \"red\" front\n";
    }
    // RSG
    for (unsigned i = 0; i < _init_edges.size(); ++i) {
        Point &p1 = _points[_init_edges[i].p1];
        Point &p2 = _points[_init_edges[i].p2];
        of << "set arrow " << idx++ << " from "
           << p1.x << "," << p1.y << " to "
           << p2.x << "," << p2.y
           << " nohead lc rgb \"white\" lw 1 front\n";
    }
    // MST
    for (unsigned i = 0; i < _init_MST.size(); ++i) {
        Point &p1 = _points[_init_edges[_init_MST[i]].p1];
        Point &p2 = _points[_init_edges[_init_MST[i]].p2];
        of << "set arrow " << idx++ << " from "
           << p1.x << "," << p1.y << " to "
           << p2.x << "," << p2.y
           << " nohead lc rgb \"blue\" lw 1 front\n";
    }
    // s-point
    for (unsigned i = _init_p; i < _points.size(); ++i) {
        of << "set object circle at first " << _points[i].x << ","
           << _points[i].y << " radius char 0.3 fillstyle solid "
           << "fc rgb \"yellow\" front\n";
    }
    // RST
    for (unsigned i = 0; i < _MST.size(); ++i) {
        if (_edges_del[_MST[i]]) continue;
        Point &p1 = _points[_edges[_MST[i]].p1];
        Point &p2 = _points[_edges[_MST[i]].p2];
        if (p1.x != p2.x) {
            of << "set arrow " << idx++ << " from "
               << p1.x << "," << p1.y << " to "
               << p2.x << "," << p1.y
               << " nohead lc rgb \"black\" lw 1.5 front\n";
        }
        if (p1.y != p2.y) {
            of << "set arrow " << idx++ << " from "
               << p2.x << "," << p1.y << " to "
               << p2.x << "," << p2.y
               << " nohead lc rgb \"black\" lw 1.5 front\n";
        }
    }
    for (unsigned i = 0; i < _newE.size(); ++i) {
        of << "set arrow " << idx++ << " from "
           << _points[_newE[i].p1].x << "," << _points[_newE[i].p1].y << " to "
           << _points[_newE[i].p2].x << "," << _points[_newE[i].p1].y
           << " nohead lc rgb \"black\" lw 1.5 front\n";
        of << "set arrow " << idx++ << " from "
           << _points[_newE[i].p2].x << "," << _points[_newE[i].p1].y << " to "
           << _points[_newE[i].p2].x << "," << _points[_newE[i].p2].y
           << " nohead lc rgb \"black\" lw 1.5 front\n";
    }
    of << "plot 1000000000" << endl;
    of << "pause -1 'Press any key'" << endl;
    of.close();
}

void Steiner::outfile(const string &outfileName) {
    ofstream of(outfileName, ofstream::out);
    of << "NumRoutedPins = " << _init_p << endl;
    of << "WireLength = " << _MRST_cost << endl;
    for (unsigned i = 0; i < _MST.size(); ++i) {
        if (_edges_del[_MST[i]]) continue;
        Point &p1 = _points[_edges[_MST[i]].p1];
        Point &p2 = _points[_edges[_MST[i]].p2];
        if (p1.x != p2.x) {
            of << "H-line "
               << "(" << p1.x << "," << p1.y << ") "
               << "(" << p2.x << "," << p1.y << ")"
               << endl;
        }
        if (p1.y != p2.y) {
            of << "V-line "
               << "(" << p2.x << "," << p1.y << ") "
               << "(" << p2.x << "," << p2.y << ")"
               << endl;
        }
    }
    for (unsigned i = 0; i < _newE.size(); ++i) {
        Point &p1 = _points[_newE[i].p1];
        Point &p2 = _points[_newE[i].p2];
        if (p1.x != p2.x) {
            of << "H-line "
               << "(" << p1.x << "," << p1.y << ") "
               << "(" << p2.x << "," << p1.y << ")"
               << endl;
        }
        if (p1.y != p2.y) {
            of << "V-line "
               << "(" << p2.x << "," << p1.y << ") "
               << "(" << p2.x << "," << p2.y << ")"
               << endl;
        }
    }
    of.close();
}

//Added Code
void Steiner::createSteiner(const std::string &fileName, std::vector<Point> Nets, Boundary Bounds) {
    _name = getFileName(fileName, true);
    _boundaryLeft = Bounds.xleft;
    _boundaryRight = Bounds.xright;
    _boundaryTop = Bounds.ytop;
    _boundaryBottom = Bounds.ybot;

    _points.resize(Nets.size());
    for (int i = 0; i < Nets.size(); i++) {
        _points[i] = Nets.at(i);
    }
    _init_p = _points.size();
}

int Steiner::initializeFile(std::ofstream &of) {
    of << "set size ratio -1" << endl;
    of << "set nokey" << endl;
    of << "set xrange["
       << (_boundaryRight - _boundaryLeft) * -0.05 << ":"
       << (_boundaryRight - _boundaryLeft) * 1.05 << "]" << endl;
    of << "set yrange["
       << (_boundaryTop - _boundaryBottom) * -0.05 << ":"
       << (_boundaryTop - _boundaryBottom) * 1.05 << "]" << endl;
    int idx = 1;
    of << "set object " << idx++ << " rect from "
       << _boundaryLeft << "," << _boundaryBottom << " to "
       << _boundaryRight << "," << _boundaryTop << "fc rgb \"grey\" behind\n";
    return idx;
}

int Steiner::plotMultiple(std::ofstream &file, int idx, std::vector<std::vector<std::vector<std::vector<int>>>> &edgeList,
                          std::vector<std::vector<Point>> &nodeList, std::string color) {
    // point
    for (int i = 0; i < _init_p; ++i) {

        file << "set object circle at first " << _points[i].x << ","
             << _points[i].y << " radius char 0.3 fillstyle solid "
             << "fc rgb \"black\" front\n";
        file << "set object circle at first " << _points[i].x << ","
             << _points[i].y << " radius char 0.15 fillstyle solid "
             << "fc rgb \""
             << color
             << "\" front\n";
    }
    // RSG
    for (unsigned i = 0; i < _init_edges.size(); ++i) {
        Point &p1 = _points[_init_edges[i].p1];
        Point &p2 = _points[_init_edges[i].p2];
        // file << "set arrow " << idx++ << " from "
        // << p1.x << "," << p1.y << " to "
        // << p2.x << "," << p2.y
        // << " nohead lc rgb \"white\" lw 1 front\n";
    }
    // MST
    for (unsigned i = 0; i < _init_MST.size(); ++i) {
        Point &p1 = _points[_init_edges[_init_MST[i]].p1];
        Point &p2 = _points[_init_edges[_init_MST[i]].p2];
        // file << "set arrow " << idx++ << " from "
        // << p1.x << "," << p1.y << " to "
        // << p2.x << "," << p2.y
        // << " nohead lc rgb \"blue\" lw 1 front\n";
    }
    std::vector<Point> tempNodes;
    // s-point
    for (unsigned i = _init_p; i < _points.size(); ++i) {
        tempNodes.push_back(_points.at(i));
        file << "set object circle at first " << _points[i].x << ","
             << _points[i].y << " radius char 0.3 fillstyle solid "
             << "fc rgb \"yellow\" front\n";
    }
    std::vector<std::vector<int>> tempSpace;
    // RST
    for (unsigned i = 0; i < _MST.size(); ++i) {
        //if (_edges_del[_MST[i]]) continue;
        Point &p1 = _points[_edges[_MST[i]].p1];
        Point &p2 = _points[_edges[_MST[i]].p2];
        bool leftRightShift = false;
        bool topDownShift = false;
        if (p1.x != p2.x) {
            leftRightShift = true;
            std::vector<int> tempHEdge;
            file << "set arrow " << idx++ << " from "
                 << p1.x << "," << p1.y << " to "
                 << p2.x << "," << p1.y
                 << " nohead lc rgb \""
                 << color
                 << "\" lw 1.5 back\n";
            //checkEdges << "H " << p1.y << " " << p1.x << " " << p2.x << std::endl;

            if (p1.x < p2.x) {
                tempHEdge.push_back(p1.x);
                tempHEdge.push_back(p1.y);
                tempHEdge.push_back(p2.x);
                tempHEdge.push_back(p1.y);
            } else {
                tempHEdge.push_back(p2.x);
                tempHEdge.push_back(p1.y);
                tempHEdge.push_back(p1.x);
                tempHEdge.push_back(p1.y);
            }
            tempSpace.push_back(tempHEdge);
        }
        if (p1.y != p2.y) {
            topDownShift = true;
            std::vector<int> tempVEdge;
            file << "set arrow " << idx++ << " from "
                 << p2.x << "," << p1.y << " to "
                 << p2.x << "," << p2.y
                 << " nohead lc rgb \""
                 << color
                 << "\" lw 1.5 back\n";
            // checkEdges << "V " << p2.x << " " << p1.y << " " << p2.y << std::endl;

            if (p1.y < p2.y) {
                tempVEdge.push_back(p2.x);
                tempVEdge.push_back(p1.y);
                tempVEdge.push_back(p2.x);
                tempVEdge.push_back(p2.y);
            } else {
                tempVEdge.push_back(p2.x);
                tempVEdge.push_back(p2.y);
                tempVEdge.push_back(p2.x);
                tempVEdge.push_back(p1.y);
            }
            tempSpace.push_back(tempVEdge);
        }
        if (topDownShift & leftRightShift) {
            Point tempP = Point(p2.x, p1.y);
            tempNodes.push_back(tempP);
        }
    }
    nodeList.push_back(tempNodes);
    std::vector<std::vector<std::vector<int>>> tempNet;
    tempNet.push_back(tempSpace);
    edgeList.push_back(tempNet);
    return idx;
}

std::vector<Point> Steiner::getPoints() {
    return _points;
} //needs a getter
std::vector<Edge> Steiner::getEdges() {
    return _edges;
}        //needs a getter
std::vector<int> Steiner::getMST() {
    return _MST;
}        //needs a getter
std::vector<bool> Steiner::getEdges_del() {
    return _edges_del;
}    //neeeds a getter
void checkNets(std::ofstream &file, std::vector<Reroute> &errors, std::vector<std::vector<std::vector<std::vector<int>>>> &edgeList) {
    for (int a = 0; a < edgeList.size(); a++) {
        for (int b = 0; b < edgeList[a].size(); b++) {
            for (int c = 0; c < edgeList[a][b].size(); c++) {
                bool cDecrement = false;
                int x1 = edgeList[a][b][c].at(0);
                int y1 = edgeList[a][b][c].at(1);
                int x2 = edgeList[a][b][c].at(2);
                int y2 = edgeList[a][b][c].at(3);
                for (int i = a; i < edgeList.size(); i++) {
                    if (a != i) {
                        for (int j = 0; j < edgeList[i].size(); j++) {
                            for (int k = 0; k < edgeList[i][j].size(); k++) {
                                int sampleX1 = edgeList[i][j][k].at(0);
                                int sampleY1 = edgeList[i][j][k].at(1);
                                int sampleX2 = edgeList[i][j][k].at(2);
                                int sampleY2 = edgeList[i][j][k].at(3);
                                if (((x1 == x2) & (sampleX1 == sampleX2) & (x1 == sampleX1)) & (((y1 >= sampleY1) & (y2 <= sampleY2)) | ((sampleY1 >= y1) & (sampleY2 <= y2)))) {
                                } else if (((y1 == y2) & (sampleY1 == sampleY2) & (y1 == sampleY1)) & (((x1 >= sampleX1) & (x2 <= sampleX2)) | ((sampleX1 >= x1) & (sampleX2 <= x2)))) {
                                } else if (((sampleX1 >= x1) & (sampleX2 <= x2) & (y1 >= sampleY1) & (y2 <= sampleY2)) | ((x1 >= sampleX1) & (x2 <= sampleX2) & (sampleY1 >= y1) & (sampleY2 <= y2))) {
                                    if (x1 == x2) {
                                        file << "set object circle at first " << x1 << ","
                                             << sampleY1 << " radius char 0.3 fillstyle solid "
                                             << "fc rgb \"red\" front\n";
                                        Reroute tempVerror = Reroute{a, i, x1, sampleY1};
                                        errors.push_back(tempVerror);

                                    } else if (y1 == y2) {
                                        file << "set object circle at first " << sampleX1 << ","
                                             << y1 << " radius char 0.3 fillstyle solid "
                                             << "fc rgb \"red\" front\n";
                                        Reroute tempHerror = Reroute{i, a, sampleX1, y1};
                                        errors.push_back(tempHerror);
                                    }
                                    std::vector<std::vector<int>> moveSpace;
                                    moveSpace.push_back({sampleX2, sampleY2, sampleX2, sampleY2});
                                    edgeList[i][j].erase(edgeList[i][j].begin() + k);
                                    k--;
                                    int movedEdges = 0;
                                    for (int m = 0; m < moveSpace.size(); m++) {
                                        int mX1 = moveSpace[m].at(0);
                                        int mY1 = moveSpace[m].at(1);
                                        int mX2 = moveSpace[m].at(2);
                                        int mY2 = moveSpace[m].at(3);
                                        for (int n = 0; n < edgeList[i][j].size(); n++) {
                                            int nX1 = edgeList[i][j][n].at(0);
                                            int nY1 = edgeList[i][j][n].at(1);
                                            int nX2 = edgeList[i][j][n].at(2);
                                            int nY2 = edgeList[i][j][n].at(3);
                                            if (((mX1 == mX2) & (nX1 == nX2) & (mX1 == nX1)) & (((mY2 >= nY1) & (mY2 <= nY2)) | ((nY2 >= mY1) & (nY2 <= mY2)))) {
                                                moveSpace.push_back(edgeList[i][j].at(n));
                                                edgeList[i][j].erase(edgeList[i][j].begin() + n);
                                                if (n <= k) {
                                                    k--;
                                                    movedEdges++;
                                                }
                                                n--;
                                            } else if (((mY1 == mY2) & (nY1 == nY2) & (mY1 == nY1)) & (((mX2 >= nX1) & (mX2 <= nX2)) | ((nX2 >= mX1) & (nX2 <= mX2)))) {
                                                moveSpace.push_back(edgeList[i][j].at(n));
                                                edgeList[i][j].erase(edgeList[i][j].begin() + n);
                                                if (n <= k) {
                                                    k--;
                                                    movedEdges++;
                                                }
                                                n--;
                                            } else if (((nX1 >= mX1) & (nX2 <= mX2) & (mY1 >= nY1) & (mY2 <= nY2)) | ((mX1 >= nX1) & (mX2 <= nX2) & (nY1 >= mY1) & (nY2 <= mY2))) {
                                                moveSpace.push_back(edgeList[i][j].at(n));
                                                edgeList[i][j].erase(edgeList[i][j].begin() + n);
                                                // std::cout << "inside 6 deep for loop" << j << std::endl;
                                                if (n <= k) {
                                                    k--;
                                                    movedEdges++;
                                                }
                                                n--;
                                            }
                                        }
                                    }
                                    edgeList[i].push_back(moveSpace);
                                    cDecrement = true;
                                    // std::vector<Reroute> temp;
                                    // Reroute left = Reroute(a, 0, j, xAxis, xMin, xMax, yAxis, yAxis);
                                    // Reroute right = Reroute(a, 0, j, xAxis, xMax, xMin, yAxis, yAxis);
                                    // Reroute bottom = Reroute(j, 1, a, yAxis, yMin, yMax, xAxis, xAxis);
                                    // Reroute top = Reroute(j, 1, a, yAxis, yMax, yMin, xAxis, xAxis);
                                    // found = 1;
                                    // temp.push_back(left);
                                    // temp.push_back(right);
                                    // temp.push_back(bottom);
                                    // temp.push_back(top);
                                    // errors.push_back(temp);
                                    // vertical[j].erase(vertical[j].begin() + k);

                                }
                            }
                        }
                    }
                }
                if (cDecrement) {
                    std::vector<std::vector<int>> moveSpaceSlow;
                    moveSpaceSlow.push_back({x2, y2, x2, y2});
                    edgeList[a][b].erase(edgeList[a][b].begin() + c);
                    c--;
                    int movedEdges = 0;
                    for (int e = 0; e < moveSpaceSlow.size(); e++) {
                        int eX1 = moveSpaceSlow[e].at(0);
                        int eY1 = moveSpaceSlow[e].at(1);
                        int eX2 = moveSpaceSlow[e].at(2);
                        int eY2 = moveSpaceSlow[e].at(3);
                        for (int f = 0; f < edgeList[a][b].size(); f++) {
                            int fX1 = edgeList[a][b][f].at(0);
                            int fY1 = edgeList[a][b][f].at(1);
                            int fX2 = edgeList[a][b][f].at(2);
                            int fY2 = edgeList[a][b][f].at(3);
                            if (((eX1 == eX2) & (fX1 == fX2) & (eX1 == fX1)) & (((eY2 >= fY1) & (eY2 <= fY2)) | ((fY2 >= eY1) & (fY2 <= eY2)))) {
                                moveSpaceSlow.push_back(edgeList[a][b].at(f));
                                edgeList[a][b].erase(edgeList[a][b].begin() + f);
                                if (f <= c) {
                                    c--;
                                    movedEdges++;
                                }
                                f--;
                            } else if (((eY1 == eY2) & (fY1 == fY2) & (eY1 == fY1)) & (((eX2 >= fX1) & (eX2 <= fX2)) | ((fX2 >= eX1) & (fX2 <= eX2)))) {
                                moveSpaceSlow.push_back(edgeList[a][b].at(f));
                                edgeList[a][b].erase(edgeList[a][b].begin() + f);
                                if (f <= c) {
                                    c--;
                                    movedEdges++;
                                }
                                f--;
                            } else if (((fX1 >= eX1) & (fX2 <= eX2) & (eY1 >= fY1) & (eY2 <= fY2)) | ((eX1 >= fX1) & (eX2 <= fX2) & (fY1 >= eY1) & (fY2 <= eY2))) {
                                // std::cout << "moveSpaceSlow: " << moveSpaceSlow.size() << std::endl;
                                moveSpaceSlow.push_back(edgeList[a][b].at(f));
                                // std::cout << "moveSpaceSlow: " << moveSpaceSlow.size() << std::endl;
                                edgeList[a][b].erase(edgeList[a][b].begin() + f);
                                // std::cout << "inside 6 not so deep for loop" << std::endl;
                                if (f <= c) {
                                    c--;
                                    movedEdges++;
                                }
                                f--;
                            }
                        }
                    }
                    edgeList[a].push_back(moveSpaceSlow);
                }
                // std::cout << c <<std::endl;
            }
        }
    }
}

int Steiner::plotFixed(std::ofstream &file, int idx, std::vector<std::vector<std::vector<std::vector<int>>>> &edgeList, std::vector<std::string> color, int initialColor) {
    // point
    for (int i = 0; i < _init_p; ++i) {

        file << "set object circle at first " << _points[i].x << ","
             << _points[i].y << " radius char 0.3 fillstyle solid "
             << "fc rgb \"black\" front\n";
        file << "set object circle at first " << _points[i].x << ","
             << _points[i].y << " radius char 0.15 fillstyle solid "
             << "fc rgb \""
             << color.at(initialColor)
             << "\" front\n";
    }
    // RSG
    for (unsigned i = 0; i < _init_edges.size(); ++i) {
        Point &p1 = _points[_init_edges[i].p1];
        Point &p2 = _points[_init_edges[i].p2];
        // file << "set arrow " << idx++ << " from "
        // << p1.x << "," << p1.y << " to "
        // << p2.x << "," << p2.y
        // << " nohead lc rgb \"white\" lw 1 front\n";
    }
    // MST
    for (unsigned i = 0; i < _init_MST.size(); ++i) {
        Point &p1 = _points[_init_edges[_init_MST[i]].p1];
        Point &p2 = _points[_init_edges[_init_MST[i]].p2];
        // file << "set arrow " << idx++ << " from "
        // << p1.x << "," << p1.y << " to "
        // << p2.x << "," << p2.y
        // << " nohead lc rgb \"blue\" lw 1 front\n";
    }
    // s-point
    for (unsigned i = _init_p; i < _points.size(); ++i) {
        // file << "set object circle at first " << _points[i].x << ","
        // << _points[i].y << " radius char 0.3 fillstyle solid "
        // << "fc rgb \"yellow\" front\n";
    }
    // RST
    if (!initialColor) {
        for (int i = 0; i < edgeList.size(); i++) {
            for (int j = 0; j < edgeList[i].size(); j++) {
                for (int k = 0; k < edgeList[i][j].size(); k++) {
                    file << "set arrow " << idx++ << " from "
                         << edgeList[i][j][k].at(0) << "," << edgeList[i][j][k].at(1) << " to "
                         << edgeList[i][j][k].at(2) << "," << edgeList[i][j][k].at(3)
                         << " nohead lc rgb \""
                         << color.at(j % color.size())
                         << "\" lw 1.5 back\n";
                }
            }
        }
    }
    return idx;
}

// void Steiner::createGrid(std::ofstream &file, int spacing)
// {
// int height = _boundaryTop - _boundaryBottom;
// int width = _boundaryRight - _boundaryLeft;
// for(int k = 0; (k < hight/spacing); k++){
// file << "set arrow " << idx++ << " from "
// << edgeList[i][j][k].at(0) << "," << edgeList[i][j][k].at(1) << " to "
// << edgeList[i][j][k].at(2) << "," << edgeList[i][j][k].at(3)
// << " nohead lc rgb \""
// << color.at(j % color.size())
// << "\" lw 1.5 back\n";
// }
// }
void Steiner::cleanNetlist(std::vector<std::vector<std::vector<std::vector<int>>>> &edgeList, int steinerNum) {
    for (int i = 0; i < edgeList[steinerNum].size(); i++) {
        for (int j = 0; j < edgeList[steinerNum][i].size(); j++) {
            if ((edgeList[steinerNum][i][j].at(0) == edgeList[steinerNum][i][j].at(2)) & (edgeList[steinerNum][i][j].at(1) == edgeList[steinerNum][i][j].at(3))) {
                bool foundRepetition = true;
                for (int k = 0; k < _init_p; k++) {
                    if ((_points[k].x == edgeList[steinerNum][i][j].at(0)) & (_points[k].y == edgeList[steinerNum][i][j].at(1))) {
                        foundRepetition = false;
                        break;
                    }
                }
                if (foundRepetition) {
                    edgeList[steinerNum][i].erase(edgeList[steinerNum][i].begin() + j);
                    j--;
                }
            }
        }

    }
    for (int i = 0; i < edgeList[steinerNum].size(); i++) {
        bool again = true;
        while (again) {
            again = false;
            bool removeSpace = true;
            std::vector<std::vector<int>> pointsFound;
            for (int j = 0; j < edgeList[steinerNum][i].size(); j++) {
                bool foundP1 = false;
                bool foundP2 = false;
                for (int k = 0; k < pointsFound.size(); k++) {
                    if ((edgeList[steinerNum][i][j].at(0) == pointsFound[k].at(0)) & (edgeList[steinerNum][i][j].at(1) == pointsFound[k].at(1))) {
                        pointsFound[k].at(2)++;
                        foundP1 = true;
                    }
                    if ((edgeList[steinerNum][i][j].at(2) == pointsFound[k].at(0)) & (edgeList[steinerNum][i][j].at(3) == pointsFound[k].at(1))) {
                        pointsFound[k].at(2)++;
                        foundP2 = true;
                    }
                }
                if (!foundP1) {
                    pointsFound.push_back({edgeList[steinerNum][i][j].at(0), edgeList[steinerNum][i][j].at(1), 0});
                }
                if (!foundP2) {
                    pointsFound.push_back({edgeList[steinerNum][i][j].at(2), edgeList[steinerNum][i][j].at(3), 0});
                }
            }
            for (int j = 0; j < _init_p; ++j) {
                for (int k = 0; k < pointsFound.size(); k++) {
                    if ((_points[j].x == pointsFound[k].at(0)) & (_points[j].y == pointsFound[k].at(1))) {
                        pointsFound[k].at(2)++;
                        removeSpace = false;
                    }
                }
            }
            if (removeSpace) {
                edgeList[steinerNum].erase(edgeList[steinerNum].begin() + i);
                i--;
            } else {
                //std::cout << "pointSize: " << pointsFound.size() << std::endl;
                for (int j = 0; j < pointsFound.size(); j++) {
                    if (pointsFound[j].at(2) == 0) {
                        for (int k = 0; k < edgeList[steinerNum][i].size(); k++) {
                            if ((edgeList[steinerNum][i][k].at(0) == pointsFound[j].at(0)) & (edgeList[steinerNum][i][k].at(1) == pointsFound[j].at(1))) {
                                edgeList[steinerNum][i].erase(edgeList[steinerNum][i].begin() + k);
                                k--;
                                again = true;
                                break;
                                // std::cout << "left edge1 is erased" << std::endl;
                            } else if ((edgeList[steinerNum][i][k].at(2) == pointsFound[j].at(0)) & (edgeList[steinerNum][i][k].at(3) == pointsFound[j].at(1))) {
                                edgeList[steinerNum][i].erase(edgeList[steinerNum][i].begin() + k);
                                k--;
                                again = true;
                                break;
                                // std::cout << "right edge1 is erased" << std::endl;
                            }
                            // std::cout << "loop did not break: " << k << std::endl;
                        }
                    }
                }
            }
            // std::cout << "pointSize: " << pointsFound.size() << std::endl;
        }
    }
}
// void fixError(std::vector<std::vector<Reroute>> &errors, std::vector<std::vector<std::vector<int>>> &horizontal, std::vector<std::vector<std::vector<int>>> &vertical, int buffer) {

// while (!errors.empty()) {
// // std::cout << errors.size() << std::endl;
// int min = 4000;
// int minIdx = 0;
// int netIdx = 0;
// Reroute distance;
// int verticalIdx;
// int verticalNetNum;
// int horizontalIdx;
// int horizontalNetNum;
// for (int i = 0; i < errors[0].size(); i++) {
// Reroute candidate = errors[0].at(i);
// if (candidate.isV) {
// verticalIdx = vertical[candidate.netNum].size() + 1;
// for (int j = 0; j < vertical[candidate.netNum].size(); j++) {
// if (i % 2 == 0) {
// if ((candidate.head == vertical[candidate.netNum][j].at(1)) & (candidate.tail == vertical[candidate.netNum][j].at(2)) & (candidate.lWing == vertical[candidate.netNum][j].at(0))) {
// verticalIdx = j;
// verticalNetNum = candidate.netNum;
// // std::cout << "found bottom at " << verticalIdx << std::endl;
// // std::cout << vertical[candidate.netNum][j].at(0) << std::endl;
// // std::cout << vertical[candidate.netNum][j].at(1) << std::endl;
// // std::cout << vertical[candidate.netNum][j].at(2) << std::endl;
// }
// } else {
// if ((candidate.head == vertical[candidate.netNum][j].at(2)) & (candidate.tail == vertical[candidate.netNum][j].at(1)) & (candidate.lWing == vertical[candidate.netNum][j].at(0))) {
// verticalIdx = j;
// verticalNetNum = candidate.netNum;
// // std::cout << "found top at " << verticalIdx << std::endl;
// // std::cout << vertical[candidate.netNum][j].at(0) << std::endl;
// // std::cout << vertical[candidate.netNum][j].at(1) << std::endl;
// // std::cout << vertical[candidate.netNum][j].at(2) << std::endl;
// }
// }
// }
// // std::cout << verticalIdx << std::endl;
// if (verticalIdx < vertical[candidate.netNum].size()) {
// errors[0].at(i).head = candidate.head + (((i % 2) * 2 - 1) * buffer);
// errors[0].at(i).lWing = candidate.lWing + (((i % 2) * 2 - 1) * buffer);
// errors[0].at(i).rWing = candidate.rWing - (((i % 2) * 2 - 1) * buffer);
// // std::cout << errors[0].at(i).head << " " << errors[0].at(i).lWing << " " << errors[0].at(i).rWing << std::endl;
// bool verticalHead = true;
// bool verticalWing = true;
// while (!(errors[0].at(i) == candidate) | verticalHead | verticalWing) {
// verticalHead = false;
// verticalWing = false;
// candidate = errors[0].at(i);
// for (int m = 0; m < vertical[candidate.netNum].size(); m++) {
// int smallNum = vertical[candidate.netNum][m][1];
// int bigNum = vertical[candidate.netNum][m][2];
// if ((smallNum <= candidate.head) & (bigNum >= candidate.head)) {
// if (i % 2 == 0) {
// if ((vertical[candidate.netNum][m][0] >= candidate.lWing) && (vertical[candidate.netNum][m][0] <= candidate.rWing)) {
// errors[0].at(i).head = smallNum - buffer;
// verticalHead = true;
// }
// } else {
// if ((vertical[candidate.netNum][m][0] >= candidate.rWing) && (vertical[candidate.netNum][m][0] <= candidate.lWing)) {
// errors[0].at(i).head = bigNum + buffer;
// verticalHead = true;
// }
// }
// }
// }
// candidate = errors[0].at(i);
// for (int n = 0; n < horizontal[candidate.netNum].size(); n++) {
// int smallNum = horizontal[candidate.netNum][n][1];
// int bigNum = horizontal[candidate.netNum][n][2];
// if (i % 2 == 0) {
// if ((horizontal[candidate.netNum][n][0] >= candidate.head) & (horizontal[candidate.netNum][n][0] <= candidate.issuePoint)) {
// if ((smallNum <= candidate.lWing) & (bigNum >= candidate.lWing)) {
// errors[0].at(i).lWing = smallNum - buffer;
// verticalWing = true;
// }
// if ((smallNum <= candidate.rWing) & (bigNum >= candidate.rWing)) {
// errors[0].at(i).rWing = bigNum + buffer;
// verticalWing = true;
// }
// }
// } else {
// if ((horizontal[candidate.netNum][n][0] <= candidate.head) & (horizontal[candidate.netNum][n][0] >= candidate.issuePoint)) {
// if ((smallNum <= candidate.lWing) & (bigNum >= candidate.lWing)) {
// errors[0].at(i).lWing = bigNum + buffer;
// verticalWing = true;
// // std::cout << "Wing adjusted" << std::endl;
// }
// if ((smallNum <= candidate.rWing) & (bigNum >= candidate.rWing)) {
// errors[0].at(i).rWing = smallNum - buffer;
// verticalWing = true;
// // std::cout << "Wing adjusted" << std::endl;
// }
// }
// }
// }
// }
// }
// } else {
// horizontalIdx = horizontal[candidate.netNum].size() + 1;
// for (int j = 0; j < horizontal[candidate.netNum].size(); j++) {
// if (i % 2 == 0) {
// if ((candidate.head == horizontal[candidate.netNum][j].at(1)) & (candidate.tail == horizontal[candidate.netNum][j].at(2)) & (candidate.lWing == horizontal[candidate.netNum][j].at(0))) {
// horizontalIdx = j;
// horizontalNetNum = candidate.netNum;
// // std::cout << "found left at " << horizontalIdx << std::endl;
// // std::cout << horizontal[candidate.netNum][j].at(0) << std::endl;
// // std::cout << horizontal[candidate.netNum][j].at(1) << std::endl;
// // std::cout << horizontal[candidate.netNum][j].at(2) << std::endl;
// }
// } else {
// if ((candidate.head == horizontal[candidate.netNum][j].at(2)) & (candidate.tail == horizontal[candidate.netNum][j].at(1)) & (candidate.lWing == horizontal[candidate.netNum][j].at(0))) {
// horizontalIdx = j;
// horizontalNetNum = candidate.netNum;
// // std::cout << "found right at " << horizontalIdx << std::endl;
// // std::cout << horizontal[candidate.netNum][j].at(0) << std::endl;
// // std::cout << horizontal[candidate.netNum][j].at(1) << std::endl;
// // std::cout << horizontal[candidate.netNum][j].at(2) << std::endl;
// }
// }
// }
// // std::cout << horizontalIdx << std::endl;
// if (horizontalIdx < horizontal[candidate.netNum].size()) {
// errors[0].at(i).head = candidate.head + (((i % 2) * 2 - 1) * buffer);
// errors[0].at(i).lWing = candidate.lWing + (((i % 2) * 2 - 1) * buffer);
// errors[0].at(i).rWing = candidate.rWing - (((i % 2) * 2 - 1) * buffer);
// // std::cout << errors[0].at(i).head << " " << errors[0].at(i).lWing << " " << errors[0].at(i).rWing  << std::endl;
// bool horizontalHead = true;
// bool horizontalWing = true;
// while (!(errors[0].at(i) == candidate) | horizontalHead | horizontalWing) {
// horizontalHead = false;
// horizontalWing = false;
// candidate = errors[0].at(i);
// for (int m = 0; m < horizontal[candidate.netNum].size(); m++) {
// int smallNum = horizontal[candidate.netNum][m][1];
// int bigNum = horizontal[candidate.netNum][m][2];
// if ((smallNum <= candidate.head) & (bigNum >= candidate.head)) {
// if (i % 2 == 0) {
// if ((horizontal[candidate.netNum][m][0] >= candidate.lWing) && (horizontal[candidate.netNum][m][0] <= candidate.rWing)) {
// errors[0].at(i).head = smallNum - buffer;
// horizontalHead = true;
// }
// } else {
// if ((horizontal[candidate.netNum][m][0] >= candidate.rWing) && (horizontal[candidate.netNum][m][0] <= candidate.lWing)) {
// errors[0].at(i).head = bigNum + buffer;
// horizontalHead = true;
// }
// }
// }
// }
// candidate = errors[0].at(i);
// for (int n = 0; n < vertical[candidate.netNum].size(); n++) {
// int smallNum = vertical[candidate.netNum][n][1];
// int bigNum = vertical[candidate.netNum][n][2];
// if (i % 2 == 0) {
// if ((vertical[candidate.netNum][n][0] >= candidate.head) & (vertical[candidate.netNum][n][0] <= candidate.issuePoint)) {
// if ((smallNum <= candidate.lWing) & (bigNum >= candidate.lWing)) {
// errors[0].at(i).lWing = smallNum - buffer;
// horizontalWing = true;
// // std::cout << "Wing adjusted" << std::endl;
// }
// if ((smallNum <= candidate.rWing) & (bigNum >= candidate.rWing)) {
// errors[0].at(i).rWing = bigNum + buffer;
// horizontalWing = true;
// // std::cout << "Wing adjusted" << std::endl;
// }
// }
// } else {
// if ((vertical[candidate.netNum][n][0] <= candidate.head) & (vertical[candidate.netNum][n][0] >= candidate.issuePoint)) {
// if ((smallNum <= candidate.lWing) & (bigNum >= candidate.lWing)) {
// errors[0].at(i).lWing = bigNum + buffer;
// horizontalWing = true;
// }
// if ((smallNum <= candidate.rWing) & (bigNum >= candidate.rWing)) {
// errors[0].at(i).rWing = smallNum - buffer;
// horizontalWing = true;
// }
// }
// }
// }
// }
// }
// }

// if (i == 0) {
// distance = errors[0].at(0);
// minIdx = i;
// } else if ((candidate < distance) & (candidate.head <= 100) & (candidate.head >= 0) & (candidate.lWing <= 100) & (candidate.lWing >= 0) & (candidate.rWing <= 100) & (candidate.rWing >= 0)) {
// distance = candidate;
// minIdx = i;
// // std::cout << minIdx << std::endl;
// // std::cout << "issuePoint: " << distance.issuePoint << std::endl;
// // std::cout << "otherPoint: " << distance.rWing << std::endl;
// }
// }
// if ((verticalIdx < vertical[verticalNetNum].size()) & (horizontalIdx < horizontal[horizontalNetNum].size())) {
// if (minIdx < 2) {
// int hLeft = distance.issuePoint;
// int hRight = distance.head;

// std::vector<int> tempH;
// if (hRight < hLeft) {
// hRight = hLeft;
// hLeft = distance.head;
// }

// tempH.push_back(distance.lWing);
// tempH.push_back(hLeft);
// tempH.push_back(hRight);
// horizontal[distance.movingNet].push_back(tempH);
// tempH.clear();
// tempH.push_back(distance.rWing);
// tempH.push_back(hLeft);
// tempH.push_back(hRight);
// horizontal[distance.movingNet].push_back(tempH);
// std::cout << "NetNum: " << distance.netNum << std::endl;
// std::cout << "isV: " << distance.isV << std::endl;
// std::cout << "movingNet: " << distance.movingNet << std::endl;
// std::cout << "issuePoint: " << distance.issuePoint << std::endl;
// std::cout << "head: " << distance.head << std::endl;
// std::cout << "tail: " << distance.tail << std::endl;
// std::cout << "lWing: " << distance.lWing << std::endl;
// std::cout << "rWing: " << distance.rWing << std::endl << std::endl;

// int hTop = distance.lWing;
// int hBottom = distance.rWing;

// if (hTop < hBottom) {
// hTop = hBottom;
// hBottom = distance.lWing;
// }
// std::vector<int> tempV;
// tempV.push_back(distance.head);
// tempV.push_back(hBottom);
// tempV.push_back(hTop);
// vertical[distance.movingNet].push_back(tempV);

// int verticalTop = vertical[distance.movingNet][verticalIdx].at(2);
// vertical[distance.movingNet][verticalIdx].at(2) = hBottom;
// if (vertical[distance.movingNet][verticalIdx].at(1) > vertical[distance.movingNet][verticalIdx].at(2)) {
// vertical[distance.movingNet][verticalIdx].at(2) = vertical[distance.movingNet][verticalIdx].at(1);
// vertical[distance.movingNet][verticalIdx].at(1) = hBottom;
// }
// tempV.clear();
// tempV.push_back(vertical[distance.movingNet][verticalIdx].at(0));
// tempV.push_back(hTop);
// tempV.push_back(verticalTop);
// if (hTop > verticalTop) {
// tempV.at(1) = tempV.at(2);
// tempV.at(2) = hTop;
// }
// vertical[distance.movingNet].push_back(tempV);

// } else {
// int vTop = distance.issuePoint;
// int vBottom = distance.head;

// std::vector<int> tempV;
// if (vTop < vBottom) {
// vTop = vBottom;
// vBottom = distance.issuePoint;
// }

// tempV.push_back(distance.lWing);
// tempV.push_back(vBottom);
// tempV.push_back(vTop);
// vertical[distance.movingNet].push_back(tempV);
// tempV.clear();
// tempV.push_back(distance.rWing);
// tempV.push_back(vBottom);
// tempV.push_back(vTop);
// vertical[distance.movingNet].push_back(tempV);


// int hLeft = distance.lWing;
// int hRight = distance.rWing;

// if (hRight < hLeft) {
// hRight = hLeft;
// hLeft = distance.rWing;
// }
// std::vector<int> tempH;
// tempH.push_back(distance.head);
// tempH.push_back(hLeft);
// tempH.push_back(hRight);
// horizontal[distance.movingNet].push_back(tempH);

// int horizontalRight = horizontal[distance.movingNet][horizontalIdx].at(2);
// horizontal[distance.movingNet][horizontalIdx].at(2) = hLeft;
// if (horizontal[distance.movingNet][horizontalIdx].at(1) > horizontal[distance.movingNet][horizontalIdx].at(2)) {
// horizontal[distance.movingNet][horizontalIdx].at(2) = horizontal[distance.movingNet][horizontalIdx].at(1);
// horizontal[distance.movingNet][horizontalIdx].at(1) = hLeft;
// }
// tempH.clear();
// tempH.push_back(horizontal[distance.movingNet][horizontalIdx].at(0));
// tempH.push_back(hRight);
// tempH.push_back(horizontalRight);
// horizontal[distance.movingNet].push_back(tempH);
// if (hRight > horizontalRight) {
// tempH.at(1) = tempH.at(2);
// tempH.at(2) = hRight;
// }
// // std::cout << "head: " << distance.head << std::endl;
// // std::cout << "hLeft: " << hLeft << std::endl;
// // std::cout << "hRight: " << hRight << std::endl << std::endl;
// }
// }

// errors.erase(errors.begin());
// }
// }

int **new_array(int n, int m) {//所产生的二维数组第0行第0列不可用
    int **aa;
    aa = new int *[n];
    for (int i = 0; i < n; i++)
        aa[i] = new int[m];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            aa[i][j] = 0;
    return aa;
}

void print_grid(int **grid, int bound_x, int bound_y) {
    ofstream outputFile;
    outputFile.open("out.txt", ios::trunc);
    for (int i = bound_y - 1; i >= 0; i--) {
        for (int j = 0; j < bound_x; j++) {
            if (grid[j][i] < 0) {
                //cout << grid[j][i] << " ";
                outputFile << grid[j][i]<< " ";
            } else {
                //cout << grid[j][i] << "  ";
                outputFile << grid[j][i] << " ";
            }
        }
        //cout << endl;
        outputFile << endl;
    }
    outputFile.close();
}

void map_generate(vector<std::vector<std::vector<std::vector<int>>>> edge, std::vector<Reroute> intersect, std::vector<Point> pin, std::vector<Point> node, int bound_x, int bound_y, int tree_order) {
    // build 2d array
    int **grid = new_array(bound_x, bound_y);
    // create sink_vector
    vector<Position> sink_vector;

    // for all other trees, go through every edge in every space and set the coord to 1
    for (int i = 0; i < edge[1].size(); ++i) {
        for (int j = 0; j < edge[1][i].size(); ++j) {
            int x_1 = edge[1][i][j][0];
            int y_1 = edge[1][i][j][1];
            int x_2 = edge[1][i][j][2];
            int y_2 = edge[1][i][j][3];
            if (x_1 == x_2) {
                for (int k = y_1; k <= y_2; ++k) {
                    grid[x_1][k] = 1;
                }
            }
            if (y_1 == y_2) {
                for (int k = x_1; k <= x_2; ++k) {
                    grid[k][y_1] = 1;
                }
            }
        }
    }

    // for tree_order tree, go through every edge in every space and set the coord to -2
    for (int i = 0; i < edge[0].size(); ++i) {
        for (int j = 0; j < edge[0][i].size(); ++j) {
            int x_1 = edge[0][i][j][0];
            int y_1 = edge[0][i][j][1];
            int x_2 = edge[0][i][j][2];
            int y_2 = edge[0][i][j][3];
            if (x_1 == x_2) {
                for (int k = y_1; k <= y_2; ++k) {
                    grid[x_1][k] = -2;
                }
            }
            if (y_1 == y_2) {
                for (int k = x_1; k <= x_2; ++k) {
                    grid[k][y_1] = -2;
                }
            }
        }
    }

    // mark intersections to be a 2
    for (int i = 0; i < intersect.size(); ++i) {
        grid[intersect[i].x][intersect[i].y] = 2;
    }

    // for tree_order tree, mark pin to be -4
    for (int i = 0; i < pin.size(); ++i) {
        grid[pin[i].x][pin[i].y] = -4;
    }
    // for tree_order tree, mark node to be -5
    for (int i = 0; i < node.size(); ++i) {
        grid[node[i].x][node[i].y] = -5;
    }
    // create delete_path vector
    vector<vector<Position>> delete_path;
    // send delete_path to mark_delete for each intersection with pin, node as finish
    for (int i = 0; i < intersect.size(); ++i) {
        Position start(intersect[i].x, intersect[i].y);
        mark_delete(grid, start, bound_x, bound_y, delete_path);
    }
    // set paths to 2
    for (int i = 0; i < delete_path.size(); ++i) {
        for (int j = 0; j < delete_path[i].size(); ++j) {
            int x = delete_path[i][j].row;
            int y = delete_path[i][j].col;
            grid[x][y] = 2;
        }
        // set last element in paths to -1
        int last_element = delete_path[i].size() - 1;
        grid[delete_path[i][last_element].row][delete_path[i][last_element].col] = -1;
    }
    // check grid to construct sink_vector with -2 value
    for (int i = 0; i < bound_x; ++i) {
        for (int j = 0; j < bound_y; ++j) {
            if (grid[i][j] == -2) {
                sink_vector.emplace_back(i, j);
            }
        }
    }
    // create island vector
    vector<Position> island;
    // for every island pin and node that has value -1, send source_propagate as start
    for (int i = 0; i < node.size(); ++i) {
        int x = node[i].x;
        int y = node[i].y;
        if (grid[x][y] == -1) {
            Position start(x, y);
            vector<Position> path;
            source_propagate(grid, start, bound_x, bound_y, path);
            // generate sink_vector_cp
            vector<Position> sink_vector_cp;
            for (int i = 0; i < bound_x; ++i) {
                for (int j = 0; j < bound_y; ++j) {
                    if (grid[i][j] == -2) {
                        sink_vector_cp.emplace_back(i, j);
                    }
                }
            }
            // create min_route and min_path, route and path

            int min_path = INT32_MAX;
            int wirelength;
            Position* min_route;
            vector<Position> route_path;
            //print_grid(grid,bound_x,bound_y);
            //return;
            // call FindPath, send every Position in island vector as start and every sink_vector as finish, return minimum PathLen and path
            FindPath(grid,path[0],sink_vector_cp[0],wirelength,min_route,bound_x,bound_y);
            //delete min_route;
//            for (int j = 0; j < path.size(); ++j) {
//                for (int k = 0; k < sink_vector_cp.size(); ++k) {
//                    if (FindPath(grid,path[j],sink_vector_cp[k],wirelength,route_path,bound_x,bound_y)){
//                        break;
//                    }
//                }
//            }
        }
        // after finding the min_route, change all of its value to -2
        // adding min_route to sink_vector
    }
    //print_grid(grid, bound_x, bound_y);
    delete[] grid;
    // check if there's any position that are marked 2 in other nets and restore them to 1
}

void mark_delete(int **grid, Position start, int bound_x, int bound_y, vector<vector<Position>> &path) {
    Position offset[4];
    offset[0].row = 0;
    offset[0].col = 1;//右
    offset[1].row = 1;
    offset[1].col = 0;//下
    offset[2].row = 0;
    offset[2].col = -1;//左
    offset[3].row = -1;
    offset[3].col = 0;//上
    int NumOfNbrs = 4;//相邻方格数
    Position here, nbr;
    for (int i = 0; i < NumOfNbrs; i++) {
        here.row = start.row;
        here.col = start.col;
        vector<Position> individual_path;
        while (true) {
            // offset position
            nbr.row = here.row + offset[i].row;
            nbr.col = here.col + offset[i].col;
            // check if on boundary
            if (nbr.row == -1 || nbr.row == bound_x || nbr.col == -1 || nbr.col == bound_y) {
                break;
            }
            // check if a node or a pin is reached
            if (grid[nbr.row][nbr.col] == -4 || grid[nbr.row][nbr.col] == -5) {
                individual_path.emplace_back(nbr.row, nbr.col);
                break;
            }
            // check if the path is not empty or other nets
            if (grid[nbr.row][nbr.col] != 0 && grid[nbr.row][nbr.col] != 1) {
                individual_path.emplace_back(nbr.row, nbr.col);
                here.row = nbr.row;
                here.col = nbr.col;
            } else {
                break;
            }

        }
        if (!individual_path.empty()) {
            path.push_back(individual_path);
        }
    }
}

void source_propagate(int **grid, Position start, int bound_x, int bound_y, vector<Position> &path) {
    // looks all four dirs and if the value == -2, record in path, change value to -1
    Position offset[4];
    offset[0].row = 0;
    offset[0].col = 1;//右
    offset[1].row = 1;
    offset[1].col = 0;//下
    offset[2].row = 0;
    offset[2].col = -1;//左
    offset[3].row = -1;
    offset[3].col = 0;//上
    int NumOfNbrs = 4;//相邻方格数
    Position here, nbr;
    here.row = start.row;
    here.col = start.col;
    for (int i = 0; i < NumOfNbrs; i++) {
        while (true) {
            // offset position
            nbr.row = here.row + offset[i].row;
            nbr.col = here.col + offset[i].col;
            // check if on boundary
            if (nbr.row == -1 || nbr.row == bound_x || nbr.col == -1 || nbr.col == bound_y) {
                break;
            }
            // check if location is -2
            if (grid[nbr.row][nbr.col] == -2) {
                path.emplace_back(nbr.row, nbr.col);
                grid[nbr.row][nbr.col] = -1;
                here.row = nbr.row;
                here.col = nbr.col;
            } else {
                break;
            }

        }
    }

}

bool FindPath(int **grid, Position start, Position finish, int &PathLen, Position* &route_path, int n, int m) {//计算从起始位置start到目标位置finish的最短布线路径
    // make new grid every pass
    int grid_copy[n][m];
    map<pair<int,int>,int> visit_map;
//    int** grid_copy_copy = new_array(n,m);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            grid_copy[i][j] = grid[i][j];
        }
    }
    //找到最短布线路径则返回false
    int i = 0;
    if ((start.row == finish.row) && (start.col == finish.col)) {
        PathLen = 0;
        return true;
    }
    //设置方格阵列“围墙”
    for (i = 0; i <= m + 1; i++)
        grid_copy[0][i] = grid_copy[n + 1][i] = 1;//顶部和底部
    for (i = 0; i <= n + 1; i++)
        grid_copy[i][0] = grid_copy[i][m + 1] = 1;//左翼和右翼
    //初始化相对位移
    Position offset[4];
    offset[0].row = 0;
    offset[0].col = 1;//右
    offset[1].row = 1;
    offset[1].col = 0;//下
    offset[2].row = 0;
    offset[2].col = -1;//左
    offset[3].row = -1;
    offset[3].col = 0;//上
    int NumOfNbrs = 4;//相邻方格数
    Position here, nbr;
    here.row = start.row;
    here.col = start.col;
    grid_copy[start.row][start.col] = 2;//起始位置的距离标记为2

    grid_copy[finish.row][finish.col] = 0;//终点标记为0

    queue<Position, list<Position> > Q;

    do {
        for (i = 0; i < NumOfNbrs; i++) {
            // set adj 4 grids to curr grid cost + 1
            nbr.row = here.row + offset[i].row; // 按右，下，左，上，移动
            nbr.col = here.col + offset[i].col;
            // check if on boundary
            if (nbr.row == -1 || nbr.row == n || nbr.col == -1 || nbr.col == m) {
                continue;
            }
            // check if visited
            if (visit_map[pair<int,int>(nbr.row,nbr.col)] == 1){
                continue;
            } else {
                visit_map[pair<int,int>(nbr.row,nbr.col)] = 1;
            }
            // if routable
            if (grid_copy[nbr.row][nbr.col] == 0 || grid_copy[nbr.row][nbr.col] == -1 || grid_copy[nbr.row][nbr.col] == -2 || grid_copy[nbr.row][nbr.col] == -4 || grid_copy[nbr.row][nbr.col] == -5) {
                grid_copy[nbr.row][nbr.col] = grid_copy[here.row][here.col] + 1; // curr + 1,
                // check if any of the 4 is the sink
                if ((nbr.row == finish.row) && (nbr.col == finish.col))
                    break;
                Q.push(nbr);
            }
        }
        // 是否到达目标位置finish?
        if ((nbr.row == finish.row) && (nbr.col == finish.col))
            break;//完成布线
        //活结点队列是否非空
        if (Q.empty()) {
            PathLen = INT32_MAX;
            return false;//无解
        }
        here = Q.front();//取下一扩展结点
        Q.pop();//删除第一个元素
        //cout<<here.row<<"," << here.col <<endl;
        //sleep(1);
//        for (int i = 0; i < n; ++i) {
//            for (int j = 0; j < m; ++j) {
//                grid_copy_copy[i][j] = grid_copy[i][j];
//            }
//        }
//        print_grid(grid_copy_copy, n, m);
//        delete []grid_copy_copy;
    } while (true);
    ofstream outputFile;
    outputFile.open("out.txt", ios::trunc);
    for (int i = m - 1; i >= 0; i--) {
        for (int j = 0; j < n; j++) {
            if (grid_copy[j][i] < 0) {
                //cout << grid[j][i] << " ";
                outputFile << grid_copy[j][i]<< " ";
            } else {
                //cout << grid[j][i] << "  ";
                outputFile << grid_copy[j][i] << " ";
            }
        }
        //cout << endl;
        outputFile << endl;
    }
    outputFile.close();
    //构造最短布线路径
    PathLen = grid_copy[finish.row][finish.col] - 2; // 相对于start点的距离 total length - 2 (start cost)
    route_path = new Position[PathLen];

    //从目标位置finish开始向起始位置回朔
    here = finish;
    for (int j = PathLen - 1; j >= 0; j--) {
        route_path[j] = here;
        //找前驱位置
        for (int i = 0; i < NumOfNbrs; i++) {
            nbr.row = here.row + offset[i].row;//按右，下，左，上，移动
            nbr.col = here.col + offset[i].col;
            if (grid_copy[nbr.row][nbr.col] == j + 2)
                break;//距离依次减小：7-6-5-4-3-2
        }
        here = nbr;//向前移动
    }
//    for (int j = 0; j < PathLen; ++j) {
//        route_path.push_back(Position(0,0));
//    }
//    for (int j = 0; j < path.size(); ++j) {
//        route_path.push_back(path[j]);
//    }
    return true;
}