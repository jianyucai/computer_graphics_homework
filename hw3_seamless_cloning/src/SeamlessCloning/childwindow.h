#ifndef CHILDWINDOW_H
#define CHILDWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPainter>
#include <QRect>
#include "imagewidget.h"

class ImageWidget;

class ChildWindow: public QMainWindow{
    Q_OBJECT
public:
    ChildWindow();
    ~ChildWindow(void);
    bool LoadFile(QString filename);
    QString current_file(){
        return current_file_;
    }

public:
    ImageWidget			*imagewidget;
    QString				current_file_;
};

#endif // CHILDWINDOW_H
