#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <vector>

using namespace std;

class ImageWidget: public QWidget {
    Q_OBJECT
public:
    ImageWidget();
    ~ImageWidget();

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void paintEvent(QPaintEvent* e);

public slots:
    // File IO
    void Open();												// Open an image file, support ".bmp, .png, .jpg" format
    void Save();												// Save image to current file
    void SaveAs();												// Save image to another file

    // Image processing
    void Invert();												// Invert pixel value in image
    void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
    void TurnGray();											// Turn image to gray-scale map
    void Restore();												// Restore image to origin

    // Image Warping
    void Delete();
    void IdwWarp();
    void RbfWarp();

private:
    QImage		*ptr_image;				// image
    QImage		*ptr_image_backup;

    bool draw_status;         //1: is drawing, 0: not drawing
    bool has_image;           //1: has a image on the board
    QPoint start_point;       //start point of shape
    QPoint end_point;         //end point of shape
    vector<QPoint> pointList; //store the shape drawed by user

    QString fileName;

    double x_bias;
    double y_bias;
};

#endif // IMAGEWIDGET_H
