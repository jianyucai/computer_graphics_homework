#ifndef IDWWARPING_H
#define IDWWARPING_H

#include "warping.h"

class IdwWarping: public Warping{
public:
    IdwWarping(QImage*, vector<QPoint>);
    QPoint f(QPoint);
    QImage warp();
    void setMode(int);
    //QImage remove_white_gap(QImage*, vector<double>, vector<double>);

private:
    double sigma(QPoint, QPoint);
    void compute_T();

private:
    /* inherit from class: Warping
    QImage* image;
    vector<QPoint> p_list;
    vector<QPoint> q_list;
    vector<double> t_list;
    */
    int control_point_num;
    int mode = 0;
    int mu = 2;
    int r = 100;
    double sigma_0 = 0.0001;

};

#endif // IDWWARPING_H
