#include "barycentriccoordinates.h"
#include <cmath>
#include <iostream>

using namespace std;

BarycentricCoordinates::BarycentricCoordinates(int width,
                                               int height,
                                               vector<QPoint> polygon_vertex_vec,
                                               vector<QPoint> triangle_vertex_vec){
    this->width = width;
    this->height = height;
    this->polygon_vertex_vec = polygon_vertex_vec;
    this->triangle_vertex_vec = triangle_vertex_vec;

}

void BarycentricCoordinates::set_poly_vertex_vec(vector<QPoint> polygon_vertex_vec){
    this->polygon_vertex_vec = polygon_vertex_vec;
}

vector<QPoint> BarycentricCoordinates::get_poly_vertex_vec(){
    return this->polygon_vertex_vec;
}

void BarycentricCoordinates::set_tria_vertex_vec(vector<QPoint> triangle_vertex_vec){
    this->triangle_vertex_vec = triangle_vertex_vec;
}

vector<QPoint> BarycentricCoordinates::get_tria_vertex_vec(){
    return this->triangle_vertex_vec;
}

void BarycentricCoordinates::set_method(Method method){
    this->method = method;
}

void BarycentricCoordinates::init_inner_bary_coor(){
    //this->inner_polygon_bary_coor_vec.reserve(this->triangle_vertex_vec.size());

    for(int i=0; i<triangle_vertex_vec.size(); ++i){
        vector<QPoint>::iterator iter = std::find(polygon_vertex_vec.begin(),
                            polygon_vertex_vec.end(),
                            triangle_vertex_vec[i]);

        // if a triangle vertex is not polygon vertex
        if(iter == polygon_vertex_vec.end()){
            vector<double> bary_coor = this->polygon_bary_coord(triangle_vertex_vec[i],
                                                                         polygon_vertex_vec);
            //this->inner_polygon_bary_coor_vec[i] = bary_coor;
            this->inner_polygon_bary_coor_vec.push_back(bary_coor);
        }
        else{
            this->inner_polygon_bary_coor_vec.push_back(vector<double>());
        }
    }
}

vector<vector<double> > BarycentricCoordinates::get_inner_polygon_bary_coor_vec(){
    return this->inner_polygon_bary_coor_vec;
}

double BarycentricCoordinates::triangle_area(QPoint* a, QPoint* b, QPoint* c){
    double result = double(a->x() * b->y() +
                           b->x() * c->y() +
                           c->x() * a->y() -
                           a->x() * c->y() -
                           b->x() * a->y() -
                           c->x() * b->y()) / double(2.0);
    return abs(result);
}


double BarycentricCoordinates::c(QPoint* v, QPoint* vi){
    switch (this->method) {
        case WP: {
            return 1;
        }
        case MV: {
            double r = sqrt(pow(v->x()-vi->x(), 2) + pow(v->y()-vi->y(), 2));
            return r;
        }
        case DH: {
            double r = sqrt(pow(v->x()-vi->x(), 2) + pow(v->y()-vi->y(), 2));
            return pow(r, 2);
        }

        default:
            return 1;
        }
}

vector<double> BarycentricCoordinates::polygon_bary_coord(QPoint p, vector<QPoint> vertex_vec){

    int size = vertex_vec.size();
    /* A is the vec for triangle area A[i]*/
    vector<double> A(vertex_vec.size());

    /* B is the vec for triangle area B[i] */
    vector<double> B(vertex_vec.size());

    for(int i=0; i<vertex_vec.size(); ++i){
        /* A[i] is the area of triangle of point (p, vertex[i], vertex[i+1]) */
        A[i] = this->triangle_area(&p, &vertex_vec[i], &vertex_vec[(i+1) % size]);

        /* B[i] is the area of triangle of point (p, vertex[i-1], vertex[i+1]) */
        B[i] = this->triangle_area(&p,
                                   &vertex_vec[(i-1+size) % size],
                                   &vertex_vec[(i+1) % size]);
    }

    /* coordinate_vec stores the barycentric coordinates for the point p */
    vector<double> coordinate_vec(vertex_vec.size());

    double coordinate_sum = 0;
    for(int i=0; i<vertex_vec.size(); ++i){
        coordinate_vec[i] = (c(&p, &vertex_vec[(i+1) % size]) * A[(i-1+size) % size] -
                             c(&p, &vertex_vec[i])   * B[i] +
                             c(&p, &vertex_vec[(i-1+size) % size]) * A[i]) /
                             (A[i] * A[(i-1+size) % size]);
        coordinate_sum += coordinate_vec[i];
    }

    // normalization
    for(int i=0; i<vertex_vec.size(); ++i){
        coordinate_vec[i] = coordinate_vec[i] / coordinate_sum;
    }


    return coordinate_vec;
}

void BarycentricCoordinates::triangle_bary_coord(QPoint* point1, QPoint* point2, QPoint* point3,
                                                 QPoint* new_point1, QPoint* new_point2, QPoint* new_point3){
    double total_area = this->triangle_area(point1, point2, point3);

    QPoint p1 = *point1;
    QPoint p2 = *point2;
    QPoint p3 = *point3;

    // sort p1, p2, p3, so that p1.x < p2.x < p3.x
    if(p1.x() > p2.x()){
        swap(p1, p2);
    }
    if(p2.x() > p3.x()){
        swap(p2, p3);
    }
    if(p1.x() > p2.x()){
        swap(p1, p2);
    }

    // 2 stages to compute
    int a13 = p3.y() - p1.y();
    int b13 = p1.x() - p3.x();
    int c13 = p3.x() * p1.y() - p1.x() * p3.y();

    //1st stage: [p1.x, p2.x]
    int a12 = p2.y() - p1.y();
    int b12 = p1.x() - p2.x();
    int c12 = p2.x() * p1.y() - p1.x() * p2.y();

    for(int x=p1.x(); x<p2.x(); ++x){
        double y_top = (-a12 * x - c12) / b12;
        double y_bottom = (-a13 * x - c13) / b13;

        if(y_top > y_bottom){
            swap(y_top, y_bottom);
        }

        y_top = floor(y_top);
        y_bottom = ceil(y_bottom);

        for(int y=y_top; y<=y_bottom; ++y){
            // compute coordinate
            vector<double> bary_coordinates(3);
            bary_coordinates[0] = this->triangle_area(new QPoint(x, y), point2, point3) / total_area;
            bary_coordinates[1] = this->triangle_area(new QPoint(x, y), point1, point3) / total_area;
            bary_coordinates[2] = this->triangle_area(new QPoint(x, y), point1, point2) / total_area;

            // compute the coordinate after warping for point (x, y)
            int new_x = bary_coordinates[0] * new_point1->x() +
                        bary_coordinates[1] * new_point2->x() +
                        bary_coordinates[2] * new_point3->x();
            int new_y = bary_coordinates[0] * new_point1->y() +
                        bary_coordinates[1] * new_point2->y() +
                        bary_coordinates[2] * new_point3->y();

            // push back the (old point, new point) pair
            this->warped_point_vec.push_back(QPoint(x, y));
            this->warped_point_vec.push_back(QPoint(new_x, new_y));
        }
    }

    //2st stage: [p2.x, p3.x]
    int a23 = p3.y() - p2.y();
    int b23 = p2.x() - p3.x();
    int c23 = p3.x() * p2.y() - p2.x() * p3.y();

    for(int x=p2.x(); x<p3.x(); ++x){
        double y_top = (-a23 * x - c23) / b23;
        double y_bottom = (-a13 * x - c13) / b13;

        if(y_top > y_bottom){
            swap(y_top, y_bottom);
        }

        y_top = floor(y_top);
        y_bottom = ceil(y_bottom);

        for(int y=y_top; y<=y_bottom; ++y){
            // compute coordinate
            vector<double> bary_coordinates(3);
            bary_coordinates[0] = this->triangle_area(new QPoint(x, y), point2, point3) / total_area;
            bary_coordinates[1] = this->triangle_area(new QPoint(x, y), point1, point3) / total_area;
            bary_coordinates[2] = this->triangle_area(new QPoint(x, y), point1, point2) / total_area;

            // compute the coordinate after warping for point (x, y)
            int new_x = bary_coordinates[0] * new_point1->x() +
                        bary_coordinates[1] * new_point2->x() +
                        bary_coordinates[2] * new_point3->x();
            int new_y = bary_coordinates[0] * new_point1->y() +
                        bary_coordinates[1] * new_point2->y() +
                        bary_coordinates[2] * new_point3->y();

            // push back the (old point, new point) pair
            this->warped_point_vec.push_back(QPoint(x, y));
            this->warped_point_vec.push_back(QPoint(new_x, new_y));
        }
    }
}



vector<QPoint> BarycentricCoordinates::warp(vector<QPoint> new_triangle_vertex_vec){

    for(int i=0; i<new_triangle_vertex_vec.size(); i+=3){
        int idx = i / 3;
        this->triangle_bary_coord(&triangle_vertex_vec[idx],
                                  &triangle_vertex_vec[idx+1],
                                  &triangle_vertex_vec[idx+2],
                                  &new_triangle_vertex_vec[idx],
                                  &new_triangle_vertex_vec[idx+1],
                                  &new_triangle_vertex_vec[idx+2]);
    }

    return this->warped_point_vec;
}
