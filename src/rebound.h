//
// This function:
// 1. Get true boundaries for both nets
// 2. Figure out the overlapping bound (if pts larger then 1)
// 3. If there're more than one overlapping dots, figure out the critical pt closest to each line
// 4. Write the overlapping pts plus the critical pt (if possible) to a third file
// 5. Reduce number, remove overlapping pts, rebound and split pts into four new files
//
// Parameter: Steiner s_1, Steiner s_2
// Output: None
//

#ifndef CAD_FINAL_PROJECT_REBOUND_H
#define CAD_FINAL_PROJECT_REBOUND_H

#include "Steiner.h"
#include <algorithm>
#include <iostream>

using namespace std;

double pt_2_line(double x1, double y1, double x2, double y2, double x3, double y3) {
    // Find the distance between point (x3, y3) and line passing through points (x1, y1) and (x2, y2)
    double num = abs((y2 - y1) * x3 - (x2 - x1) * y3 + x2 * y1 - y2 * x1);
    double den = sqrt(pow(y2 - y1, 2) + pow(x2 - x1, 2));
    return num / den;
}

void rebound(Steiner *s_1, Steiner *s_2) {
    // 1. Get true boundaries for both nets
    int total_pts_1 = s_1->get_points().size();
    if (total_pts_1 == 1)
        return;
    int total_pts_2 = s_2->get_points().size();
    if (total_pts_2 == 1)
        return;
    double min_x_1 = s_1->get_points()[0].x;
    double max_x_1 = s_1->get_points()[0].x;
    double min_y_1 = s_1->get_points()[0].y;
    double max_y_1 = s_1->get_points()[0].y;
    for (int i = 0; i < total_pts_1; ++i) {
        int x = s_1->get_points()[i].x;
        int y = s_1->get_points()[i].y;
        if (max_x_1 < x)
            max_x_1 = x;
        if (max_y_1 < y)
            max_y_1 = y;
        if (min_x_1 > x)
            min_x_1 = x;
        if (min_y_1 > y)
            min_y_1 = y;
    }
//    s_1->set_bounds((int) min_y_1, (int) min_x_1, (int) max_y_1, (int) max_x_1);
    double min_x_2 = s_2->get_points()[0].x;
    double max_x_2 = s_2->get_points()[0].x;
    double min_y_2 = s_2->get_points()[0].y;
    double max_y_2 = s_2->get_points()[0].y;
    for (int i = 0; i < total_pts_2; ++i) {
        int x = s_2->get_points()[i].x;
        int y = s_2->get_points()[i].y;
        if (max_x_2 < x)
            max_x_2 = x;
        if (max_y_2 < y)
            max_y_2 = y;
        if (min_x_2 > x)
            min_x_2 = x;
        if (min_y_2 > y)
            min_y_2 = y;
    }
//    s_2->set_bounds((int) min_y_2, (int) min_x_2, (int) max_y_2, (int) max_x_2);
    cout << "file 1 rebound (" << min_x_1 << "," << min_y_1 << ") (" << max_x_1 << "," << max_y_1 << ")" << endl;
    cout << "file 2 rebound (" << min_x_2 << "," << min_y_2 << ") (" << max_x_2 << "," << max_y_2 << ")" << endl;

    // 2. Figure out the overlapping bound
    double min_x_overlap;
    double min_y_overlap;
    double max_x_overlap;
    double max_y_overlap;
    double x_overlap = max(0.0, min(max_x_1, max_x_2) - max(min_x_1, min_x_2));
    double y_overlap = max(0.0, min(max_y_1, max_y_2) - max(min_y_1, min_y_2));
    if (x_overlap == 0 || y_overlap == 0) {
        cout << "The bounds do not overlap." << endl;
        return;
    } else {
        min_x_overlap = max(min_x_1, min_x_2);
        min_y_overlap = max(min_y_1, min_y_2);
        max_x_overlap = min(max_x_1, max_x_2);
        max_y_overlap = min(max_y_1, max_y_2);
        cout << "The overlapping bound is (" << min_x_overlap << ", " << min_y_overlap << "), (" << max_x_overlap << ", " << max_y_overlap << ")." << endl;
    }

    // 3. If there're more than one overlapping dots, figure out the critical pt closest to each line
    // figure out pts overlapping
    vector<Point> overlapping_pts_1;
    vector<Point> other_pts_1;
    vector<Point> overlapping_pts_2;
    vector<Point> other_pts_2;
    for (int i = 0; i < total_pts_1; ++i) {
        int x_1 = s_1->get_points()[i].x;
        int y_1 = s_1->get_points()[i].y;
        int x_2 = s_2->get_points()[i].x;
        int y_2 = s_2->get_points()[i].y;
        if (x_1 > min_x_overlap && y_1 > min_y_overlap && x_1 < max_x_overlap && y_1 < max_y_overlap)
            overlapping_pts_1.push_back(s_1->get_points()[i]);
        else
            other_pts_1.push_back(s_1->get_points()[i]);
        if (x_2 > min_x_overlap && y_2 > min_y_overlap && x_2 < max_x_overlap && y_2 < max_y_overlap)
            overlapping_pts_2.push_back(s_2->get_points()[i]);
        else
            other_pts_2.push_back(s_2->get_points()[i]);
    }
    vector<double> pts_2_line_1;
    vector<double> pts_2_line_2;
    for (int i = 0; i < other_pts_1.size(); ++i) {
        pts_2_line_1.push_back(pt_2_line(min_x_1, min_y_1, min_x_1, max_y_1, other_pts_1[i].x, other_pts_1[i].y));
        pts_2_line_1.push_back(pt_2_line(min_x_1, min_y_1, max_x_1, min_y_1, other_pts_1[i].x, other_pts_1[i].y));
        pts_2_line_1.push_back(pt_2_line(min_x_1, max_y_1, max_x_1, max_y_1, other_pts_1[i].x, other_pts_1[i].y));
        pts_2_line_1.push_back(pt_2_line(max_x_1, max_y_1, max_x_1, min_y_1, other_pts_1[i].x, other_pts_1[i].y));
    }
    for (int i = 0; i < other_pts_2.size(); ++i) {
        pts_2_line_2.push_back(pt_2_line(min_x_2, min_y_2, min_x_2, max_y_2, other_pts_2[i].x, other_pts_2[i].y));
        pts_2_line_2.push_back(pt_2_line(min_x_2, min_y_2, max_x_2, min_y_2, other_pts_2[i].x, other_pts_2[i].y));
        pts_2_line_2.push_back(pt_2_line(min_x_2, max_y_2, max_x_2, max_y_2, other_pts_2[i].x, other_pts_2[i].y));
        pts_2_line_2.push_back(pt_2_line(max_x_2, max_y_2, max_x_2, min_y_2, other_pts_2[i].x, other_pts_2[i].y));
    }
    // determine critical pt and its ind if overlapping pts > 1
    if (overlapping_pts_1.size() > 1) {
        int min_pts_ind_1 = 0;
        double pts_2_line_min_1 = pts_2_line_1[0];
        for (int i = 0; i < pts_2_line_1.size(); ++i) {
            if (pts_2_line_min_1 > pts_2_line_1[i]) {
                pts_2_line_min_1 = pts_2_line_1[i];
                min_pts_ind_1 = i / 4;
            }
        }
        // removing crit pt from other pts and adding to overlapping pts
        overlapping_pts_1.push_back(other_pts_1[min_pts_ind_1]);
        cout << "file 1 critical pt (" << other_pts_1[min_pts_ind_1].x << ", " << other_pts_1[min_pts_ind_1].y << ")" << endl;
        other_pts_1.erase(other_pts_1.begin() + min_pts_ind_1);
    }
    if (overlapping_pts_2.size() > 1) {
        int min_pts_ind_2 = 0;
        double pts_2_line_min_2 = pts_2_line_2[0];
        for (int i = 0; i < pts_2_line_2.size(); ++i) {
            if (pts_2_line_min_2 > pts_2_line_2[i]) {
                pts_2_line_min_2 = pts_2_line_2[i];
                min_pts_ind_2 = i / 4;
            }
        }
        // removing crit pt from other pts and adding to overlapping pts
        overlapping_pts_2.push_back(other_pts_2[min_pts_ind_2]);
        cout << "file 2 critical pt (" << other_pts_2[min_pts_ind_2].x << ", " << other_pts_2[min_pts_ind_2].y << ")" << endl;
        other_pts_2.erase(other_pts_2.begin() + min_pts_ind_2);
    }

    // 4. Write the overlapping pts plus the critical pt (if possible) to a third file
    ofstream of_overlap(s_1->get_name() + "_" + s_2->get_name() + ".txt", ofstream::out);
    of_overlap << "Boundary = (" << min_x_overlap << "," << min_y_overlap << "), (" << max_x_overlap << "," << max_y_overlap << ")" << endl;
    of_overlap << "NumPins = " << overlapping_pts_1.size() << endl;
    for (int i = 0; i < overlapping_pts_1.size(); ++i)
        of_overlap << "PIN p" << i << " (" << overlapping_pts_1[i].x << "," << overlapping_pts_1[i].y << ")" << endl;
    of_overlap << "NumPins = " << overlapping_pts_2.size() << endl;
    for (int i = 0; i < overlapping_pts_2.size(); ++i)
        of_overlap << "PIN p" << i << " (" << overlapping_pts_2[i].x << "," << overlapping_pts_2[i].y << ")" << endl;
    of_overlap.close();

    // 5. Reduce number, remove overlapping pts, rebound and split pts into four new files
    ofstream of_1_1(s_1->get_name() + "_1.txt", ofstream::out);
    of_1_1 << "Boundary = (" << min_x_1 << "," << max_y_overlap << "), (" << max_x_1 << "," << max_y_1 << ")" << endl;
    int pt_count_1_1 = 0;
    vector<Point> pts_1_1;
    ofstream of_1_2(s_1->get_name() + "_2.txt", ofstream::out);
    of_1_2 << "Boundary = (" << min_x_1 << "," << min_y_overlap << "), (" << min_x_overlap << "," << max_y_overlap << ")" << endl;
    int pt_count_1_2 = 0;
    vector<Point> pts_1_2;
    ofstream of_1_3(s_1->get_name() + "_3.txt", ofstream::out);
    of_1_3 << "Boundary = (" << max_x_overlap << "," << min_y_overlap << "), (" << max_x_1 << "," << max_y_overlap << ")" << endl;
    int pt_count_1_3 = 0;
    vector<Point> pts_1_3;
    ofstream of_1_4(s_1->get_name() + "_4.txt", ofstream::out);
    of_1_4 << "Boundary = (" << min_x_1 << "," << min_y_1 << "), (" << max_x_1 << "," << min_y_overlap << ")" << endl;
    int pt_count_1_4 = 0;
    vector<Point> pts_1_4;
    for (int i = 0; i < other_pts_1.size(); ++i) {
        if (other_pts_1[i].y > max_y_overlap) {
            pt_count_1_1++;
            pts_1_1.push_back(other_pts_1[i]);
        } else if (other_pts_1[i].y <= max_y_overlap && other_pts_1[i].y >= min_y_overlap && other_pts_1[i].x <= min_x_overlap) {
            pt_count_1_2++;
            pts_1_2.push_back(other_pts_1[i]);
        } else if (other_pts_1[i].y <= max_y_overlap && other_pts_1[i].y >= min_y_overlap && other_pts_1[i].x >= min_x_overlap) {
            pt_count_1_3++;
            pts_1_3.push_back(other_pts_1[i]);
        } else {
            pt_count_1_4++;
            pts_1_4.push_back(other_pts_1[i]);
        }
    }
    of_1_1 << "NumPins = " << pt_count_1_1 << endl;
    for (int i = 0; i < pts_1_1.size(); ++i) {
        of_1_1 << "PIN p" << i << " (" << pts_1_1[i].x << "," << pts_1_1[i].y << ")" << endl;
    }
    of_1_2 << "NumPins = " << pt_count_1_2 << endl;
    for (int i = 0; i < pts_1_2.size(); ++i) {
        of_1_2 << "PIN p" << i << " (" << pts_1_2[i].x << "," << pts_1_2[i].y << ")" << endl;
    }
    of_1_3 << "NumPins = " << pt_count_1_3 << endl;
    for (int i = 0; i < pts_1_3.size(); ++i) {
        of_1_3 << "PIN p" << i << " (" << pts_1_3[i].x << "," << pts_1_3[i].y << ")" << endl;
    }
    of_1_4 << "NumPins = " << pt_count_1_4 << endl;
    for (int i = 0; i < pts_1_4.size(); ++i) {
        of_1_4 << "PIN p" << i << " (" << pts_1_4[i].x << "," << pts_1_4[i].y << ")" << endl;
    }


    ofstream of_2_1(s_2->get_name() + "_1.txt", ofstream::out);
    of_2_1 << "Boundary = (" << min_x_2 << "," << max_y_overlap << "), (" << max_x_2 << "," << max_y_2 << ")" << endl;
    int pt_count_2_1 = 0;
    vector<Point> pts_2_1;
    ofstream of_2_2(s_2->get_name() + "_2.txt", ofstream::out);
    of_2_2 << "Boundary = (" << min_x_2 << "," << min_y_overlap << "), (" << min_x_overlap << "," << max_y_overlap << ")" << endl;
    int pt_count_2_2 = 0;
    vector<Point> pts_2_2;
    ofstream of_2_3(s_2->get_name() + "_3.txt", ofstream::out);
    of_2_3 << "Boundary = (" << max_x_overlap << "," << min_y_overlap << "), (" << max_x_2 << "," << max_y_overlap << ")" << endl;
    int pt_count_2_3 = 0;
    vector<Point> pts_2_3;
    ofstream of_2_4(s_2->get_name() + "_4.txt", ofstream::out);
    of_2_4 << "Boundary = (" << min_x_2 << "," << min_y_2 << "), (" << max_x_2 << "," << min_y_overlap << ")" << endl;
    int pt_count_2_4 = 0;
    vector<Point> pts_2_4;
    for (int i = 0; i < other_pts_2.size(); ++i) {
        if (other_pts_2[i].y > max_y_overlap) {
            pt_count_2_1++;
            pts_2_1.push_back(other_pts_2[i]);
        } else if (other_pts_2[i].y <= max_y_overlap && other_pts_2[i].y >= min_y_overlap && other_pts_2[i].x <= min_x_overlap) {
            pt_count_2_2++;
            pts_2_2.push_back(other_pts_2[i]);
        } else if (other_pts_2[i].y <= max_y_overlap && other_pts_2[i].y >= min_y_overlap && other_pts_2[i].x >= min_x_overlap) {
            pt_count_2_3++;
            pts_2_3.push_back(other_pts_2[i]);
        } else {
            pt_count_2_4++;
            pts_2_4.push_back(other_pts_2[i]);
        }
    }
    of_2_1 << "NumPins = " << pt_count_2_1 << endl;
    for (int i = 0; i < pts_2_1.size(); ++i) {
        of_2_1 << "PIN p" << i << " (" << pts_2_1[i].x << "," << pts_2_1[i].y << ")" << endl;
    }
    of_2_2 << "NumPins = " << pt_count_2_2 << endl;
    for (int i = 0; i < pts_2_2.size(); ++i) {
        of_2_2 << "PIN p" << i << " (" << pts_2_2[i].x << "," << pts_2_2[i].y << ")" << endl;
    }
    of_2_3 << "NumPins = " << pt_count_2_3 << endl;
    for (int i = 0; i < pts_2_3.size(); ++i) {
        of_2_3 << "PIN p" << i << " (" << pts_2_3[i].x << "," << pts_2_3[i].y << ")" << endl;
    }
    of_2_4 << "NumPins = " << pt_count_2_4 << endl;
    for (int i = 0; i < pts_2_4.size(); ++i) {
        of_2_4 << "PIN p" << i << " (" << pts_2_4[i].x << "," << pts_2_4[i].y << ")" << endl;
    }

    of_1_1.close();
    of_1_2.close();
    of_1_3.close();
    of_1_4.close();
    of_2_1.close();
    of_2_2.close();
    of_2_3.close();
    of_2_4.close();
}
//
#endif //CAD_FINAL_PROJECT_REBOUND_H
