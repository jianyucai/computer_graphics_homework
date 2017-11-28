#include "childwindow.h"

ChildWindow::ChildWindow(){
    imagewidget = new ImageWidget(this);
    setCentralWidget(imagewidget);
}

ChildWindow::~ChildWindow(){}

bool ChildWindow::LoadFile(QString filename){
    imagewidget->Open(filename);
    return true;
}

