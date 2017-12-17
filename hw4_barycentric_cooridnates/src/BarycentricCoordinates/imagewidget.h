#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include "polygon.h"
#include "barycentriccoordinates.h"

using namespace std;

enum status {
    NORMAL,     // normal status
    DRAWING,    // is drawing polygon
    DRAGGING    // is dragging a point in polygon
};

class DisplayWidget;

class ImageWidget: public QWidget {
    Q_OBJECT
public:
    ImageWidget();
    ~ImageWidget();
    int get_width();
    int get_height();
    vector<QPoint> add_bias(vector<QPoint>);
    vector<QPoint> substract_bias(vector<QPoint>);

public:
    DisplayWidget* display_widget;

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* e);

public slots:
    // open an image
    void Open();
    // set status to DRAWING
    void Draw();
    // set status to DRAGGING
    void Drag();


private:
    QImage		*ptr_image;				// image
    QImage      *ptr_image_backup;
    QString     fileName;
    bool        is_writable;            // if the image_widget can be drawed
    status      widget_status;          // status of image_widget
    Polygon*    polygon;
    int dragging_vertex_idx;

    // triangulation param
    vector<QPoint> vertex_vec_param;

    // triangulation results: triangle vertexes
    vector<QPoint> triangle_vertex_vec;
    // stores the bias of rect
    int width_bias;
    int height_bias;

    // barycentric coordinates class
    BarycentricCoordinates* bc;
    QPoint old_vertex;

};


class DisplayWidget: public QWidget {
    Q_OBJECT
public:
    DisplayWidget(QImage* img);
    ~DisplayWidget();
protected:
    void paintEvent(QPaintEvent* e);
private:
    QImage		*ptr_image;
};
#endif // IMAGEWIDGET_H
