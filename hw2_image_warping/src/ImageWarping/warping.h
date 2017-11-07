#ifndef WARPING_H
#define WARPING_H

#include <vector>
#include <QImage>
#include <QPoint>

using namespace std;

class Warping{
public:
    Warping();
    virtual QPoint f(QPoint) = 0;
    //virtual vector<double> f(QPoint) = 0;
    virtual QImage warp() = 0;
    QImage remove_white_gap(QImage*, vector<double>, vector<double>);



protected:
    QImage* image;
    vector<QPoint> p_list;
    vector<QPoint> q_list;


};

#endif // WARPING_H
