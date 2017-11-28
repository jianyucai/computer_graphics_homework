#ifndef SEAMLESSCLONING_H
#define SEAMLESSCLONING_H

#include <QImage>
#include <QPoint>
#include <Eigen/Dense>
#include <vector>
using namespace std;

class SeamlessCloning{
public:
    SeamlessCloning(QImage*, QImage*, vector<QPoint>, vector<QPoint>);
    ~SeamlessCloning();
    void seamless_cloning(); // the operation of seamless cloning

private:
    QImage* cloned_image;
    QImage* source_image;
    vector<QPoint> cloned_point_list;
    vector<QPoint> source_point_list;
    int** clone_map;   // used to store whether a point is in the clone zone or background zone
    bool** border_map; // used to store the border of cloned zone

    vector<QPoint> get_neibours(QPoint); // get neibours of a QPoint

    bool on_border(QPoint); // whether a point is on the border of cloned zone

};

#endif // SEAMLESSCLONING_H
