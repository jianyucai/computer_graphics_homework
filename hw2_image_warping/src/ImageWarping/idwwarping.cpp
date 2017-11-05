#include "idwwarping.h"
#include <QRgb>
#include <cmath>
#include <math.h>
#include <iostream>

using namespace std;

IdwWarping::IdwWarping(QImage* image, vector<QPoint> control_point_list){
    // initialize the number of control points
    control_point_num = control_point_list.size() / 2;

    // initialize p_list and q_list
    for(size_t i=0; i<control_point_list.size(); i+=2){
        p_list.push_back(control_point_list[i]);
        q_list.push_back(control_point_list[i+1]);
    }

    // intialize image
    this->image = image;

}

double IdwWarping::sigma(QPoint p_i, QPoint p){
    /*
     * sigma function
     * please refer to paper "Image Warping with Scattered Data Interpolation, 1995"
     * sigma function is defined in Equation (3) and (4)
     *
     * p_i, the i'th control point
     *
     * p, the point as parameter
     *
     * mode: 0, refers to function in Equation (3), proposed by Sheperd
     *       1, refers to function in Equation (4), proposed by Franke and Nielson
     *
     * mode 0 considers global influence of all control points, while
     * mode 1 considers locally bounded influence
     *
     * mu: exponent in the function, usually choose 2
     *
     * return: the value of fuction sigma_i(p), refer to Equation (3)
     */
    double d = pow(pow(p.x()-p_i.x(), 2) + pow(p.y()-p_i.y(), 2), mu/2);
    double result = 0.0;

    if(mode == 0)
        result = double(double(1) / d);
    if(mode == 1){
        result = double(max(r-sqrt(d), 0.0)) / (r * sqrt(d));
        result = pow(result, mu);
    }

    return result;
}


QPoint IdwWarping::f(QPoint point){
    /*
     * compute f_x and f_y
     * please refer to paper "Image Warping with Scattered Data Interpolation, 1995"
     * function f is defined in Equation (1)
     *
     */
    vector<double> warped_point_list;
    double f_x = 0.0;
    double f_y = 0.0;

    // if param: point is one of control points in p_list,
    // just return the corresponding point in q_list
    for(int k=0; k<control_point_num; ++k){
        if(p_list[k].x() == point.x() && p_list[k].y() == point.y()){
            //warped_point_list.push_back(q_list[k].x());
            //warped_point_list.push_back(q_list[k].y());
            return q_list[k];
        }
    }

    double sigma_sum = 0.0;
    for(int j=0; j<control_point_num; ++j){
        double sigma_i = sigma(p_list[j], point);
        sigma_sum += sigma_i;
    }
    /****/
    if(mode == 1)
       sigma_sum += sigma_0;

    // compute f_x and f_y with all control points
    for(int i=0; i<control_point_num; ++i){

         double t11 = t_list[4*i];
         double t12 = t_list[4*i+1];
         double t21 = t_list[4*i+2];
         double t22 = t_list[4*i+3];

         /*
          * compute f_i.x and f_i.y
          * please refer to paper ""Image Warping with Scattered Data Interpolation, 1995"
          * the method to compute f_i.x and f_i.y is defined in Equation (7)
          */
         double f_i_x = q_list[i].x() + t11*(point.x()-p_list[i].x()) + t12*(point.y()-p_list[i].y());
         double f_i_y = q_list[i].y() + t21*(point.x()-p_list[i].x()) + t22*(point.y()-p_list[i].y());

         double w_i_p = sigma(p_list[i], point) / sigma_sum;

         f_x += w_i_p * f_i_x;
         f_y += w_i_p * f_i_y;

     }
    /****/
    if(mode == 1){
        f_x += point.x() / sigma_sum * sigma_0;
        f_y += point.y() / sigma_sum * sigma_0;
    }

    //warped_point_list.push_back(f_x);
    //warped_point_list.push_back(f_y);

   // return warped_point_list;
    return QPoint(f_x, f_y);
}

void IdwWarping::compute_T(){
    /*
     * compute matrix T for each control point
     * the goal is to solve the equations obtained by solving first-order derivatives
     * define in Equation (8) in paper "Image Warping with Scattered Data Interpolation, 1995"
     * the answer to those equations is defined in Equation (13) (14) (15) (16) (17)
     * in paper "Free Form Deformation with Scattered Data Interpolation Methods, 1992"
     */

    // for each control point
    for(int i=0; i<control_point_num; ++i){
           double t11 = 0.0, t12 = 0.0,
                  t21 = 0.0, t22 = 0.0;

           // parameters for equations
           double det = 0.0;
           double det_1 = 0.0;
           double det_2 = 0.0;
           double det_3 = 0.0;

           double t11_1 = 0.0;
           double t11_2 = 0.0;
           double t11_3 = 0.0;
           double t11_4 = 0.0;

           double t12_1 = 0.0;
           double t12_2 = 0.0;
           double t12_3 = 0.0;
           double t12_4 = 0.0;

           double t21_1 = 0.0;
           double t21_2 = 0.0;
           double t21_3 = 0.0;
           double t21_4 = 0.0;

           double t22_1 = 0.0;
           double t22_2 = 0.0;
           double t22_3 = 0.0;
           double t22_4 = 0.0;

           double t_backup = 0.0;
           double t11_backup = 0.0;
           double t12_backup = 0.0;


           // update all parameters
           for(int j=0; j<control_point_num; ++j){
               if(j != i){
                   double sigma_value = sigma(p_list[i], p_list[j]);
                   double spx = p_list[j].x() - p_list[i].x();
                   double spy = p_list[j].y() - p_list[i].y();
                   double sqx = q_list[j].x() - q_list[i].x();
                   double sqy = q_list[j].y() - q_list[i].y();

                   double a = sigma_value * spx * spx;
                   double b = sigma_value * spx * spy;
                   double c = sigma_value * spx * sqx;
                   double d = sigma_value * spx * sqy;
                   double e = sigma_value * spy * spy;
                   double f = sigma_value * spy * sqx;
                   double g = sigma_value * spy * sqy;

                   // compute several parts of det
                   det_1 += a;
                   det_2 += e;
                   det_3 += b;

                   // compute all parts of t11
                   t11_1 += c;
                   t11_2 += e;
                   t11_3 += f;
                   t11_4 += b;

                   // compute all parts of t12
                   t12_1 += f;
                   t12_2 += a;
                   t12_3 += c;
                   t12_4 += b;

                   // compute all parts of t21
                   t21_1 += d;
                   t21_2 += e;
                   t21_3 += g;
                   t21_4 += b;

                   // compute all parts of t22
                   t22_1 += g;
                   t22_2 += a;
                   t22_3 += d;
                   t22_4 += b;

                   // compute backup
                   t_backup   += a + e;
                   t11_backup += c + g;
                   t12_backup += f + d;
               }
           }

           det = det_1 * det_2 - det_3 * det_3;

           if(det == 0){
               if(t_backup == 0){
                   t11 = 1.0;
                   t12 = 1.0;
                   t21 = 1.0;
                   t22 = 1.0;
               }else{
                   t11_backup /= t_backup;
                   t12_backup /= t_backup;
                   t11 = t22 = t11_backup;
                   t12 = t21 = t12_backup;
               }

           }else{
               t11 = (t11_1 * t11_2 - t11_3 * t11_4) / det;
               t12 = (t12_1 * t12_2 - t12_3 * t12_4) / det;
               t21 = (t21_1 * t21_2 - t21_3 * t21_4) / det;
               t22 = (t22_1 * t22_2 - t22_3 * t22_4) / det;
           }

           t_list.push_back(t11);
           t_list.push_back(t12);
           t_list.push_back(t21);
           t_list.push_back(t22);
       }
}

QImage IdwWarping::warp(){
    /*
     * do the warping operation
     */
    QImage warped_image = QImage();

    vector<double> trans;
    vector<double> pixel_x;
    vector<double> pixel_y;

    // compute parameter: matrix T
    compute_T();

    // use f() to compute the coordinates of all points in image after warping
    for (int i=0; i<(image->width()); i++){
        for (int j=0; j<(image->height()); j++){
            QPoint warped_point = f(QPoint(i, j));
            pixel_x.push_back(warped_point.x());
            pixel_y.push_back(warped_point.y());
            //trans = f(QPoint(i, j));
            //pixel_x.push_back(trans[0]);
            //pixel_y.push_back(trans[1]);
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

void IdwWarping::setMode(int m){
    if(m == 0 || m == 1)
        mode = m;
}



