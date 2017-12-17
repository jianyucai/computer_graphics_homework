#include "imagewidget.h"
#include <iostream>
#include <QFileDialog>
#include <QMouseEvent>
#include <QtMath>
#include "triangulation.h"

using namespace std;

ImageWidget::ImageWidget(){
    this->ptr_image = new QImage();
    this->ptr_image_backup = new QImage();
    this->widget_status = NORMAL;
    this->polygon = new Polygon();
    this->display_widget = new DisplayWidget(this->ptr_image);
    this->dragging_vertex_idx = -1;
}

ImageWidget::~ImageWidget(){}

int ImageWidget::get_width(){
    return this->ptr_image->width();
}

int ImageWidget::get_height(){
    return this->ptr_image->height();
}

vector<QPoint> ImageWidget::add_bias(vector<QPoint> vec){
    vector<QPoint> vec_new = vec;
    for(int i=0; i<vec.size(); ++i){
        vec_new[i] = QPoint(vec[i].x() + this->width_bias,
                            vec[i].y() + this->height_bias);
    }
    return vec_new;
}

vector<QPoint> ImageWidget::substract_bias(vector<QPoint> vec){
    vector<QPoint> vec_new = vec;
    for(int i=0; i<vec.size(); ++i){
        vec_new[i] = QPoint(vec[i].x() - this->width_bias,
                            vec[i].y() - this->height_bias);
    }
    return vec_new;
}

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

    // Draw polygon
    this->polygon->draw(painter);

    // Draw triangle edges
    if(this->triangle_vertex_vec.size() != 0){
        painter.setPen(QPen(Qt::black, 1));
        for(size_t i=0; i<triangle_vertex_vec.size(); i+=3){
            // draw 3 edges for a triangle
            painter.drawLine(triangle_vertex_vec[i],
                             triangle_vertex_vec[i+1]);
            painter.drawLine(triangle_vertex_vec[i],
                             triangle_vertex_vec[i+2]);
            painter.drawLine(triangle_vertex_vec[i+1],
                             triangle_vertex_vec[i+2]);
        }
    }

    // update its display widget
    this->display_widget->update();

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

    // add the corners of display rect to vec
    this->width_bias = (width()-ptr_image->width())/2;
    this->height_bias = (height()-ptr_image->height())/2;

    cout<<"image size: "<<ptr_image->width()<<' '<<ptr_image->height()<<endl;

    update();
}

void ImageWidget::Draw(){
    this->widget_status = DRAWING;
}

void ImageWidget::Drag(){
    this->widget_status = DRAGGING;
}


void ImageWidget::mousePressEvent(QMouseEvent *event){
    switch(this->widget_status){
    // if is drawing
        case DRAWING: {
            switch (event->button()) {
            // if left button is pressed
                case Qt::LeftButton: {
                    /* push back the current point into vec */
                    this->polygon->push_vertex(QPoint(event->pos().x(), event->pos().y()));
                    update();
                    break;
                }
                // if right button is pressed
                case Qt::RightButton: {
                    this->polygon->set_status(true);
                    update();
                    this->widget_status = NORMAL;

                    /*******/
                    /* initialize the vec for triangulation */
                    vertex_vec_param = this->substract_bias(*this->polygon->get_vertex());
                    // add 4 corners of image rect
                    //vertex_vec_param.push_back(QPoint(0, 0));
                    //vertex_vec_param.push_back(QPoint(this->ptr_image->width()-1, 0));
                    //vertex_vec_param.push_back(QPoint(0, this->ptr_image->height()-1));
                    //vertex_vec_param.push_back(QPoint(this->ptr_image->width()-1,his->ptr_image->height()-1));

                    // do triagulation

                    this->triangle_vertex_vec = Triangulation::triangulate(vertex_vec_param,
                                                                         this->get_width(),
                                                                         this->get_height());


                    this->triangle_vertex_vec = this->add_bias(triangle_vertex_vec);
                    /*******/

                    // init params
                    vector<QPoint> polygon_vertex_vec_param =
                            this->substract_bias(*this->polygon->get_vertex());
                    vector<QPoint> triangle_vertex_vec_param =
                            this->substract_bias(this->triangle_vertex_vec);

                    // update params for bc
                    this->bc = new BarycentricCoordinates(this->ptr_image->width(),
                                                          this->ptr_image->height(),
                                                          polygon_vertex_vec_param,
                                                          triangle_vertex_vec_param);
                    this->bc->set_method(Method::MV);
                    this->bc->init_inner_bary_coor();

                    update();
                    break;
                }
                default:
                    break;
            }
            break;
        }

        // if is dragging
        case DRAGGING: {
            vector<QPoint>* vertex_vec_ptr = this->polygon->get_vertex();
            // find the point that is being dragged
            for(int i=0; i<vertex_vec_ptr->size(); ++i){
                // if current point is closed enough to a vertex in polygon
                if(fabs(event->pos().x()-(*vertex_vec_ptr)[i].x())<=5
                        && fabs(event->pos().y()-(*vertex_vec_ptr)[i].y())<=5){
                    this->dragging_vertex_idx = i;
                    break;
                }
            }
            break;
        }

        // if is normal
        case NORMAL: {
            break;
        }
    }
}

void ImageWidget::mouseMoveEvent(QMouseEvent *event){
    switch (this->widget_status) {
        case DRAGGING:{
            if(this->dragging_vertex_idx != -1){
                this->polygon->set_vertex(dragging_vertex_idx,
                    QPoint(event->pos().x(), event->pos().y()));

                QPoint p = QPoint(event->pos().x()-width_bias, event->pos().y()-height_bias);
                this->vertex_vec_param[dragging_vertex_idx] = p;

                // do triagulation
                this->triangle_vertex_vec = Triangulation::drag_triangulate_real_time(this->dragging_vertex_idx,
                                                                                      p,
                                                                                      this->bc->get_poly_vertex_vec(),
                                                                                      this->bc->get_tria_vertex_vec(),
                                                                                      this->bc);

                this->triangle_vertex_vec = this->add_bias(triangle_vertex_vec);
                update();
            }
            break;
        }
        default:
            break;
        }
}

void ImageWidget::mouseReleaseEvent(QMouseEvent *event){
    switch (this->widget_status) {
        case DRAGGING:{
            //this->widget_status = NORMAL;
            // find which triangle that this point is in
            if(this->dragging_vertex_idx != -1){

                QPoint p = QPoint(event->pos().x()-width_bias, event->pos().y()-height_bias);
                this->triangle_vertex_vec = Triangulation::drag_triangulate_real_time(this->dragging_vertex_idx,
                                                                                      p,
                                                                                      this->bc->get_poly_vertex_vec(),
                                                                                      this->bc->get_tria_vertex_vec(),
                                                                                      this->bc);
                // the (old point, new point) pair
                vector<QPoint> warp_point_vec = bc->warp(this->triangle_vertex_vec);
                // warp each pixel in the zone
                for(int i=0; i<warp_point_vec.size(); i+=2){
                    int old_x = warp_point_vec[i].x();
                    int old_y = warp_point_vec[i].y();
                    int new_x = warp_point_vec[i+1].x();
                    int new_y = warp_point_vec[i+1].y();

                    if(new_x >= 0 && new_x < this->ptr_image->width() &&
                            new_y >= 0 && new_y < this->ptr_image->height()){
                        this->ptr_image->setPixel(new_x, new_y, this->ptr_image_backup->pixel(old_x, old_y));
                    }
                }

                this->triangle_vertex_vec = this->add_bias(triangle_vertex_vec);
                this->dragging_vertex_idx = -1;
                // update bc
                this->bc->set_poly_vertex_vec(substract_bias(*this->polygon->get_vertex()));
                this->bc->set_tria_vertex_vec(substract_bias(this->triangle_vertex_vec));

                update();
            }
            break;
        }
        default:
            break;
        }
}


DisplayWidget::DisplayWidget(QImage* img){
    this->ptr_image = img;
}

DisplayWidget::~DisplayWidget(){}

void DisplayWidget::paintEvent(QPaintEvent *e){
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

    painter.end();
}


