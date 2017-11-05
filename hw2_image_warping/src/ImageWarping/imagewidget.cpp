#include "imagewidget.h"
#include "imagewarping.h"
#include <iostream>
#include <QFileDialog>
#include <QMouseEvent>
#include "idwwarping.h"
#include "rbfwarping.h"

using namespace std;

ImageWidget::ImageWidget(){
    ptr_image = new QImage();
    ptr_image_backup = new QImage();
    draw_status = false;
    has_image = false;
}


ImageWidget::~ImageWidget(){}


void ImageWidget::paintEvent(QPaintEvent *e){
    QPainter painter;
    painter.begin(this);

    // Draw background
    painter.setBrush(Qt::lightGray);
    QRect back_rect(0, 0, width(), height());
    painter.drawRect(back_rect);

    // Draw image
    QRect rect = QRect( (width()-ptr_image->width())/2,
                        (height()-ptr_image->height())/2,
                        ptr_image->width(),
                        ptr_image->height());

    painter.drawImage(rect, *ptr_image);

    if(has_image == true){
        painter.setPen(QPen(Qt::red, 2));
        for(size_t i=0; i<pointList.size(); i+=2)
            painter.drawLine(pointList[i], pointList[i+1]);
        painter.drawLine(start_point, end_point);
    }

    painter.end();
}


void ImageWidget::Open(){
    // Open file
    fileName = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

    // Load file
    if (!fileName.isEmpty()){
        ptr_image->load(fileName);
        *(ptr_image_backup) = *(ptr_image);
    }

    cout<<"image size: "<<ptr_image->width()<<' '<<ptr_image->height()<<endl;

    has_image = true;

    x_bias = (width() - ptr_image->width()) / 2;
    y_bias = (height() - ptr_image->height()) / 2;

    update();
}


void ImageWidget::Save(){
    SaveAs();
}


void ImageWidget::SaveAs(){
    QString save_filename = fileName;
    int idx = fileName.lastIndexOf(".");
    save_filename = save_filename.insert(idx, "_warped");
    if (save_filename.isNull()){
        return;
    }

    ptr_image->save(save_filename);
}


void ImageWidget::Invert(){

    for (int i=0; i<ptr_image->width(); i++){
        for (int j=0; j<ptr_image->height(); j++){
            QRgb color = ptr_image->pixel(i, j);
            ptr_image -> setPixel(i, j, qRgb(255-qRed(color), 255-qGreen(color), 255-qBlue(color)) );
        }
    }
    // equivalent member function of class QImage
    // ptr_image_->invertPixels(QImage::InvertRgb);
    update();
}


void ImageWidget::Mirror(bool ishorizontal, bool isvertical){

    QImage image_tmp(*(ptr_image));
    int width = ptr_image->width();
    int height = ptr_image->height();

    if (ishorizontal)
        if (isvertical)
            for (int i=0; i<width; i++)
                for (int j=0; j<height; j++)
                    ptr_image->setPixel(i, j, image_tmp.pixel(width-1-i, height-1-j));

        else
            for (int i=0; i<width; i++)
                for (int j=0; j<height; j++)
                    ptr_image->setPixel(i, j, image_tmp.pixel(i, height-1-j));

    else
        if (isvertical)
            for (int i=0; i<width; i++)
                for (int j=0; j<height; j++)
                    ptr_image->setPixel(i, j, image_tmp.pixel(width-1-i, j));

    // equivalent member function of class QImage
    //*(ptr_image_) = ptr_image_->mirrored(true, true);
    update();
}


void ImageWidget::TurnGray(){

    for (int i=0; i<ptr_image->width(); i++){
        for (int j=0; j<ptr_image->height(); j++){
            QRgb color = ptr_image->pixel(i, j);
            int gray_value = (qRed(color)+qGreen(color)+qBlue(color))/3;
            ptr_image -> setPixel(i, j, qRgb(gray_value, gray_value, gray_value));
        }
    }

    update();
}


void ImageWidget::Restore(){
    *(ptr_image) = *(ptr_image_backup);
    update();
}

void ImageWidget::Delete(){}


void ImageWidget::IdwWarp(){
    vector<QPoint> bias_point_list;
    for(size_t i=0; i<pointList.size(); ++i){
        bias_point_list.push_back(QPoint(pointList[i].x()-x_bias, pointList[i].y()-y_bias));
    }

    IdwWarping idw = IdwWarping(ptr_image, bias_point_list);
    //idw.setMode(1);
    *ptr_image = idw.warp();

    pointList.clear();
    start_point = end_point = QPoint(0,0);

    update();
}


void ImageWidget::RbfWarp(){
    vector<QPoint> bias_point_list;
    for(size_t i=0; i<pointList.size(); ++i){
        bias_point_list.push_back(QPoint(pointList[i].x()-x_bias, pointList[i].y()-y_bias));
    }

    RbfWarping rbf = RbfWarping(ptr_image, bias_point_list);
    *ptr_image = rbf.warp();


    pointList.clear();
    start_point = end_point = QPoint(0,0);

    update();
}







void ImageWidget::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        draw_status = true;
        start_point = end_point = event->pos();
        update();
    }
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event){
    if(draw_status){
        end_point = event->pos();
        update();
    }
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event){
        draw_status = false;
        pointList.push_back(start_point);
        pointList.push_back(end_point);
        update();
}

/*
void ImageWidget::paintEvent(QPaintEvent*){
    QPainter painter(this);
    for(size_t i=0; i<pointList.size()/2; ++i)
        painter.drawLine(pointList[i], pointList[i]+1);
    painter.drawLine(start_point, end_point);

    update();
}
*/
