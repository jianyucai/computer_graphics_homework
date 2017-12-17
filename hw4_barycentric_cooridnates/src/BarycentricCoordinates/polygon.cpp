#include "polygon.h"

Polygon::Polygon(){
    this->is_complete = false;
}

Polygon::~Polygon(){}

vector<QPoint>* Polygon::get_vertex(){
    return &this->vertex_vec;
}

void Polygon::draw(QPainter &painter){
    painter.setPen(QPen(Qt::red, 2));
    if(vertex_vec.size() == 0){
        return;
    }
    if(is_complete == false){
        if(vertex_vec.size() <= 1){
            painter.drawLine(vertex_vec.front(), vertex_vec.front());
        }
        else{
            for(size_t i=0; i<vertex_vec.size()-1; ++i){
                painter.drawLine(vertex_vec[i], vertex_vec[i+1]);
            }
        }
    }
    else{
        for(int i=0; i<vertex_vec.size()-1; ++i){
            painter.drawLine(vertex_vec[i], vertex_vec[i+1]);
        }
        painter.drawLine(vertex_vec.front(), vertex_vec.back());
    }
}

void Polygon::set_status(bool status){
    this->is_complete = status;
}

void Polygon::push_vertex(QPoint vertex){
    this->vertex_vec.push_back(vertex);
}

void Polygon::set_vertex(int idx, QPoint p){
    this->vertex_vec[idx] = p;
}
