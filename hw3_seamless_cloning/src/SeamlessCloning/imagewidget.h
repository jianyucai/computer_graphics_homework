#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QPoint>
#include <QWidget>
#include <QPainter>
#include <vector>
#include "childwindow.h"

using namespace std;

class ChildWindow;
enum DrawStatus{
    kChoose,
    kPaste,
    kNone
};

class ImageWidget : public QWidget{
    Q_OBJECT
public:
    ImageWidget(ChildWindow *relatewindow);
    ~ImageWidget(void);

    int ImageWidth();	// Width of image
    int ImageHeight();	// Height of image
    void set_draw_status_to_choose();
    void set_draw_status_to_paste();
    QImage* image();
    void set_source_window(ChildWindow* childwindow);

protected:
    void paintEvent(QPaintEvent *paintevent);
    void mousePressEvent(QMouseEvent *mouseevent);
    void mouseMoveEvent(QMouseEvent *mouseevent);
    void mouseReleaseEvent(QMouseEvent *mouseevent);

public slots:
    // File IO
    void Open(QString filename);								// Open an image file, support ".bmp, .png, .jpg" format
    void Save();												// Save image to current file
    void SaveAs();												// Save image to another file

    // Image processing
    void Invert();												// Invert pixel value in image
    void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
    void TurnGray();											// Turn image to gray-scale map
    void Restore();												// Restore image to origin

public:
    QPoint point_start;		// Left top point of rectangle region
    QPoint point_end;		// Right bottom point of rectangle region


    QImage *image_;				// image
    QImage *image_backup;
    vector<QPoint> cloned_point_list; // store cloned points with coordinate in cloned_image
    vector<QPoint> source_point_list; // store cloned points with coordinate in source_image
private:
    // Pointer of child window
    ChildWindow	*source_window;	// Source child window

    // Signs
    DrawStatus draw_status;		// Enum type of draw status
    bool is_choosing;
    bool is_pasting;

};

#endif // IMAGEWIDGET_H
