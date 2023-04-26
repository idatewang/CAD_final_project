#include <iostream>
#include <list>
#include <queue>
#include <fstream>
#include <string>

using namespace std;

class Position {
public:
    int row, col;

    Position() : row(), col() {}

    Position(int row, int col) : row(row), col(col) {}
};



//***************主函数**************
//int main() {
//    int i = 0, j = 0, k = 0;
//    vector<Position> start, finish;
//    int min_length = INT32_MAX;
//    Position* min_route;
//    int PathLen;
//    Position *path;
//    //打开输入文件，文件的第一行为矩阵的行列数：n,m
//    string input;
//    cout << "enter input file name" << endl;
//    cin >> input;
//    ifstream inputFile(input.c_str());
//    if (!inputFile) {
//        cout << "can't open input file." << endl;
//        return 0;
//    }
//    //请输入输出文件名
//    string output;
//    ofstream outputFile;
//    cout << "enter output file name" << endl;
//    cin >> output;
//    outputFile.open(output.c_str(), ios::trunc);
//    if (!outputFile) {
//        cout << "can't open output file." << endl;
//        return 0;
//    }
//
//    inputFile >> n;//取得矩阵的行数n
//    inputFile >> m;//取得矩阵的行数m
//    grid = new_arry(n + 2, m + 2);//直接创建一个动态二维数组，好象用起来很方便！
//    //从输入文件中读取矩阵
//    for (i = 1; i <= n; i++) {
//        for (j = 1; j <= m; j++) {
//            inputFile >> grid[i][j];
//            Position pos(i,j);
//            if (grid[i][j] == -1) {
//                start.push_back(pos);
//            }
//            if (grid[i][j] == -2) {
//                finish.push_back(pos);
//            }
//            cout << grid[i][j] << " ";
//        }
//        cout << endl;
//    }
//    for (auto l : start) {
//        for (auto o : finish) {
//            FindPath(l, o, PathLen, path);
//            if (PathLen<min_length){
//                min_length = PathLen;
//                min_route = path;
//            }
//        }
//    }
//    //输出结果
//    cout << endl << "After maze route: " << endl << endl;
//    for (i = 0; i < n + 2; i++) {
//        for (j = 0; j < m + 2; j++) {
//            for (k = 0; k < min_length; k++)
//                if (i == min_route[k].row && j == min_route[k].col) {
//                    cout << "* ";
//                    outputFile << "* ";
//                    break;
//                }
//            if (k == min_length) {
//                cout << grid[i][j] << " ";
//                outputFile << grid[i][j] << " ";
//            }
//        }
//        cout << endl;
//        outputFile << endl;
//    }
//    cout << "Shortest Length: " << min_length << endl;
//    outputFile << "Shortest Length: " << min_length << endl;
//    inputFile.close();
//    outputFile.close();
//    delete[]grid;
//    return 0;
//}