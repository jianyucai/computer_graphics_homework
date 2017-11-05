#ifndef RBFWARPING_H
#define RBFWARPING_H

#include "warping.h"


class RbfWarping: public Warping{
public:
    RbfWarping(QImage*, vector<QPoint>);
    QPoint f(QPoint);
    //vector<double> f(QPoint);
    QImage warp();

private:
    QPoint p_m(QPoint);
    double get_distance(QPoint, QPoint);
    void compute_r();
    void compute_alpha();
    double radial_func(int, QPoint);

private:
    /* inherit from class: Warping
    QImage* image;
    */

    int control_point_num;
    vector<double> alpha_list;
    vector<double> r_list;
    vector<vector<double> > control_point_distance;
    double mu = 0.5;

};

#endif // RBFWARPING_H
