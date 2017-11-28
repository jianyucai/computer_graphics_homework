#include "imagewidget.h"
#include <QImage>
#include <QtWidgets>
#include <iostream>
using namespace std;


ImageWidget::ImageWidget(ChildWindow *relatewindow){
    image_ = new QImage();
    image_backup = new QImage();

    draw_status = kNone;
    is_choosing = false;
    is_pasting  = false;

    point_start = QPoint(0, 0);
    point_end   = QPoint(0, 0);

    source_window = NULL;
}


ImageWidget::~ImageWidget(void){}


int ImageWidget::ImageWidth(){
    return image_->width();
}


int ImageWidget::ImageHeight(){
    return image_->height();
}


void ImageWidget::set_draw_status_to_choose(){
    draw_status = kChoose;
}


void ImageWidget::set_draw_status_to_paste(){
    draw_status = kPaste;
}


QImage* ImageWidget::image(){
    return image_;
}


void ImageWidget::set_source_window(ChildWindow* childwindow){
    source_window = childwindow;
}


void ImageWidget::paintEvent(QPaintEvent *paintevent){
    QPainter painter;
    painter.begin(this);

    // Draw background
    painter.setBrush(Qt::lightGray);
    QRect back_rect(0, 0, width(), height());
    painter.drawRect(back_rect);

    // Draw image
    QRect rect = QRect( 0, 0, image_->width(), image_->height());
    painter.drawImage(rect, *image_);

    // Draw choose region
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::red);
    painter.drawRect(point_start.x(), point_start.y(),
        point_end.x()-point_start.x(), point_end.y()-point_start.y());

    painter.end();
}


void ImageWidget::mousePressEvent(QMouseEvent *mouseevent){
    if (Qt::LeftButton == mouseevent->button()){
        switch (draw_status){
            case kChoose:
                is_choosing = true;
                point_start = point_end = mouseevent->pos();
                break;

            case kPaste:{
                is_pasting = true;

                // Start point in object image
                int xpos = mouseevent->pos().rx();
                int ypos = mouseevent->pos().ry();

                // Start point in source image
                int xsourcepos = source_window->imagewidget->point_start.rx();
                int ysourcepos = source_window->imagewidget->point_start.ry();

                // Width and Height of rectangle region
                int w = source_window->imagewidget->point_end.rx()
                        -source_window->imagewidget->point_start.rx()+1;
                int h = source_window->imagewidget->point_end.ry()
                        -source_window->imagewidget->point_start.ry()+1;

                // Paste
                if ( (xpos+w<image_->width()) && (ypos+h<image_->height()) ){
                        // Restore image
                    //	*(image_) = *(image_backup_);

                        // Paste
                    for (int i=0; i<w; i++){
                        for (int j=0; j<h; j++){
                            image_->setPixel(xpos+i, ypos+j, source_window->imagewidget->image()->pixel(xsourcepos+i, ysourcepos+j));
                            cloned_point_list.push_back(QPoint(xpos+i, ypos+j));
                            source_point_list.push_back(QPoint(xsourcepos+i, ysourcepos+j));
                        }
                    }
                    cout<<"."<<endl;
                }
            }

        update();
        break;

        default:
            break;
        }
    }
}


void ImageWidget::mouseMoveEvent(QMouseEvent *mouseevent){
    switch (draw_status){
        case kChoose:
            // Store point position for rectangle region
            if (is_choosing){
                point_end = mouseevent->pos();
            }
            break;

        case kPaste:
            // Paste rectangle region to object image
            if (is_pasting){
                // Start point in object image
                int xpos = mouseevent->pos().rx();
                int ypos = mouseevent->pos().ry();

                // Start point in source image
                int xsourcepos = source_window->imagewidget->point_start.rx();
                int ysourcepos = source_window->imagewidget->point_start.ry();

                // Width and Height of rectangle region
                int w = source_window->imagewidget->point_end.rx()
                    -source_window->imagewidget->point_start.rx()+1;
                int h = source_window->imagewidget->point_end.ry()
                    -source_window->imagewidget->point_start.ry()+1;

                // Paste
                if ( (xpos>0) && (ypos>0) && (xpos+w<image_->width()) && (ypos+h<image_->height()) ){
                    // Restore image
                    *(image_) = *(image_backup);

                    // Paste
                    for (int i=0; i<w; i++){
                        for (int j=0; j<h; j++){
                            image_->setPixel(xpos+i, ypos+j, source_window->imagewidget->image()->pixel(xsourcepos+i, ysourcepos+j));
                            /** add the cloned points to cloned_point_list**/
                            //cloned_point_list.push_back(QPoint(xpos+i, ypos+j));
                        }
                    }
                }
            }

        default:
            break;
    }
    update();
}


void ImageWidget::mouseReleaseEvent(QMouseEvent *mouseevent){
    switch (draw_status){
        case kChoose:
            if (is_choosing){
                point_end = mouseevent->pos();
                is_choosing = false;
                draw_status = kNone;
            }

        case kPaste:
            if (is_pasting){
                is_pasting = false;
                draw_status = kNone;
            }
        default:
            break;
    }
    update();
}


void ImageWidget::Open(QString filename){
    // Load file
    if (!filename.isEmpty()){
        image_->load(filename);
        *(image_backup) = *(image_);
    }

//	setFixedSize(image_->width(), image_->height());
//	relate_window_->setWindowFlags(Qt::Dialog);
//	relate_window_->setFixedSize(QSize(image_->width(), image_->height()));
//	relate_window_->setWindowFlags(Qt::SubWindow);

    //image_->invertPixels(QImage::InvertRgb);
    //*(image_) = image_->mirrored(true, true);
    //*(image_) = image_->rgbSwapped();
    cout<<"image size: "<<image_->width()<<' '<<image_->height()<<endl;
    update();
}


void ImageWidget::Save(){
    SaveAs();
}

void ImageWidget::SaveAs(){
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
    if (filename.isNull()){
        return;
    }

    image_->save(filename);
}


void ImageWidget::Invert(){
    for (int i=0; i<image_->width(); i++){
        for (int j=0; j<image_->height(); j++){
            QRgb color = image_->pixel(i, j);
            image_->setPixel(i, j, qRgb(255-qRed(color), 255-qGreen(color), 255-qBlue(color)) );
        }
    }

    // equivalent member function of class QImage
    // image_->invertPixels(QImage::InvertRgb);
    update();
}


void ImageWidget::Mirror(bool ishorizontal, bool isvertical){
    QImage image_tmp(*(image_));
    int width = image_->width();
    int height = image_->height();

    if (ishorizontal){
        if (isvertical){
            for (int i=0; i<width; i++){
                for (int j=0; j<height; j++){
                    image_->setPixel(i, j, image_tmp.pixel(width-1-i, height-1-j));
                }
            }
        }
        else{
            for (int i=0; i<width; i++){
                for (int j=0; j<height; j++){
                    image_->setPixel(i, j, image_tmp.pixel(i, height-1-j));
                }
            }
        }

    }
    else{
        if (isvertical){
            for (int i=0; i<width; i++){
                for (int j=0; j<height; j++){
                    image_->setPixel(i, j, image_tmp.pixel(width-1-i, j));
                }
            }
        }
    }

    // equivalent member function of class QImage
    //*(image_) = image_->mirrored(true, true);
    update();
}


void ImageWidget::TurnGray(){
    for (int i=0; i<image_->width(); i++){
        for (int j=0; j<image_->height(); j++){
            QRgb color = image_->pixel(i, j);
            int gray_value = (qRed(color)+qGreen(color)+qBlue(color))/3;
            image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value) );
        }
    }
    update();
}

void ImageWidget::Restore(){
    *(image_) = *(image_backup);
    point_start = point_end = QPoint(0, 0);
    update();
}


