#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <QPoint>
#include <QPainter>
using namespace std;

class Polygon{
public:
    Polygon();
    ~Polygon();
    vector<QPoint>* get_vertex();
    void draw(QPainter &painter);
    void set_status(bool);
    void push_vertex(QPoint);
    void set_vertex(int, QPoint);
private:
    vector<QPoint> vertex_vec;
    bool is_complete;
};

#endif // POLYGON_H
