#include "rbfwarping.h"
#include <cmath>
#include <math.h>
#include <Eigen/Dense>
#include <iostream>

using namespace std;
using namespace Eigen;


RbfWarping::RbfWarping(QImage* image, vector<QPoint> control_point_list){
    // initialize the number of control points
    control_point_num = control_point_list.size() / 2;

    // initialize p_list and q_list
    for(int i=0; i<control_point_list.size(); i+=2){
        p_list.push_back(control_point_list[i]);
        q_list.push_back(control_point_list[i+1]);
    }

    // initialize alpha_list
    for(int i=0; i<control_point_num; ++i){
        alpha_list.push_back(0.0);
    }

    // initialize alpha_list
    for(int i=0; i<control_point_num; ++i){
        r_list.push_back(0.0);
    }

    // initialize control_point_distance
    for(int i=0; i<control_point_num; ++i){
        vector<double> tmp;
        for(int j=0; j<control_point_num-1; ++j){
            tmp.push_back(0.0);
        }
        control_point_distance.push_back(tmp);
    }

    // intialize image
    this->image = image;
}



QPoint RbfWarping::p_m(QPoint point){
    /*
     * p_m(point) returns the polynomial form of vector p
     * refer to Equation (9) in paper "Image Warping with Scattered Data Interpolation, 1995"
     * the paper adopts the identity function: p_m(p) = p
     * which behaves better than most polynomial form
     */
    return QPoint(point.x(), point.y());
}



double RbfWarping::get_distance(QPoint a, QPoint b){
    /*
     * get the distance between 2 QPoints
     */
    double distance = 0.0;
    distance = sqrt(pow(a.x()-b.x(), 2) + pow(a.y()-b.y(), 2));

    return distance;
}



void RbfWarping::compute_r(){
    /*
     * computes the r value for each control point
     * refer to Equation (11) in paper "Image Warping with Scattered Data Interpolation, 1995"
     * the r value for each control point is the nearest control point to it
     */

    for(int i=0; i<control_point_num; ++i){
        int k = 0;
        for(int j=0; j<control_point_num; ++j){
            if(i != j){
                double distance_ij = get_distance(p_list[i], p_list[j]);
                control_point_distance[i][k] = distance_ij;
                k += 1;
            }
        }
    }

    for(int i=0; i<control_point_num; ++i){
        vector<double>::iterator smallest_iter =
                min_element(begin(control_point_distance[i]), end(control_point_distance[i]));
        double smallest = *smallest_iter;

        r_list.push_back(smallest);
    }
}


double RbfWarping::radial_func(int i, QPoint point){
    /*
     * computes the radial basis function for a point
     * based on a control point p_i
     * refer to Equation (10) in paper "Image Warping with Scattered Data Interpolation, 1995"
     */
    double r_i = r_list[i];
    QPoint p_i = p_list[i];
    double d = get_distance(p_i, point);
    double result = pow(pow(d, 2) + pow(r_i, 2), mu);

    return result;
}


void RbfWarping::compute_alpha(){
    /*
     * compute alpha's value by solving linear equations
     * refer to Equation (9) in paper "Image Warping with Scattered Data Interpolation, 1995"
     * using Eigen library to solve the equations
     */
    MatrixXd A1(control_point_num, control_point_num);
    MatrixXd b1(control_point_num, 1);
    MatrixXd A2(control_point_num, control_point_num);
    MatrixXd b2(control_point_num, 1);

    // fill A1 and A2
    for(int i=0; i<control_point_num; ++i){
        for(int j=0; j<control_point_num; ++j){
            double c = radial_func(j, p_list[i]);
            A1(i, j) = c;
            A2(i, j) = c;
        }
    }

    // fill b1 and b2
    for(int i=0; i<control_point_num; ++i){
        b1(i, 0) = q_list[i].x() - p_list[i].x();
        b2(i, 0) = q_list[i].y() - p_list[i].y();
    }

    // solve the alpha's value
    VectorXd alpha_1 = A1.colPivHouseholderQr().solve(b1);
    VectorXd alpha_2 = A2.colPivHouseholderQr().solve(b2);

    for(int i=0; i<control_point_num; ++i){
        alpha_list.push_back(alpha_1(i, 0));
        alpha_list.push_back(alpha_2(i, 0));
    }
}



QPoint RbfWarping::f(QPoint point){
    /*
     * compute f_x and f_y
     * please refer to paper "Image Warping with Scattered Data Interpolation, 1995"
     * function f is defined in Equation (9)
     */
    vector<double> warped_point_list;

    double f_x = 0.0;
    double f_y = 0.0;

    for(int i=0; i<control_point_num; ++i){
        f_x += alpha_list[2*i] * radial_func(i, point);
        f_y += alpha_list[2*i+1] * radial_func(i, point);
    }

    f_x += p_m(point).x();
    f_y += p_m(point).y();

    //warped_point_list.push_back(f_x);
    //warped_point_list.push_back(f_y);

    //return warped_point_list;
    return QPoint(f_x, f_y);
}



QImage RbfWarping::warp(){
    /*
     * do the warping operation
     */

    QImage warped_image = QImage();

    vector<double> trans;
    vector<double> pixel_x;
    vector<double> pixel_y;

    // compute parameter: r and alpha
    compute_r();
    compute_alpha();

    // use f() to compute the coordinates of all points in image after warping
    for (int i=0; i<(image->width()); i++){
        for (int j=0; j<(image->height()); j++){
            QPoint warped_point = f(QPoint(i, j));
            pixel_x.push_back(warped_point.x());
            pixel_y.push_back(warped_point.y());

        }
    }


    vector<double>::iterator smallest_x_iter = min_element(begin(pixel_x), end(pixel_x));
    vector<double>::iterator smallest_y_iter = min_element(begin(pixel_y), end(pixel_y));
    vector<double>::iterator biggest_x_iter  = max_element(begin(pixel_x), end(pixel_x));
    vector<double>::iterator biggest_y_iter  = max_element(begin(pixel_y), end(pixel_y));

    double smallest_x = *smallest_x_iter;
    double smallest_y = *smallest_y_iter;
    double biggest_x  = *biggest_x_iter;
    double biggest_y  = *biggest_y_iter;

    double width_new = biggest_x - smallest_x;
    double height_new = biggest_y - smallest_y;

    for(size_t i=0; i<pixel_x.size(); ++i){
        pixel_x[i] -= smallest_x;
        pixel_y[i] -= smallest_y;
    }

    warped_image = QImage(int(width_new+2), int(height_new+2), QImage::Format(4));
    warped_image.fill(Qt::white);


    // fill the new warped image
    for (int i=0; i<(image->width()); i++){
        for (int j=0; j<(image->height()); j++){
            warped_image.setPixel(int(pixel_x[i*(image->width())+j]), int(pixel_y[i*(image->width())+j]), image->pixel(i, j));
        }
    }

    warped_image = remove_white_gap(&warped_image, pixel_x, pixel_y);

    return warped_image;
}
