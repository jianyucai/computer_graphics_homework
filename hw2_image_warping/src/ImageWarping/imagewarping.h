#ifndef IMAGEWARPING_H
#define IMAGEWARPING_H

#include <QImage>
#include <QPoint>
#include <vector>
#include <cmath>
#include <math.h>
#include <Eigen/Dense>
#include <iostream>

using namespace std;
using namespace Eigen;

double sigma(QPoint p_i, QPoint p, int mode=0, int mu=2){
    double d = pow(pow(p.x()-p_i.x(), 2) + pow(p.y()-p_i.y(), 2), mu/2);
    double result = 0.0;

    if(mode == 0)
        result = double(double(1) / d);
    if(mode == 1){
        double r = 100.0;
        result = double(max(r-sqrt(d), 0.0)) / (r * sqrt(d));
        result = pow(result, mu);

    }

    return result;
}

vector<double> f(vector<QPoint> p, vector<QPoint> q, vector<double> t, QPoint point){
    double f_x = 0.0;
    double f_y = 0.0;
    double num = int(p.size());
    vector<double> result;

    for(int k=0; k<num; ++k){
        if(p[k].x() == point.x() && p[k].y() == point.y()){
            result.push_back(q[k].x());
            result.push_back(q[k].y());
            return result;
        }
    }

    //cout<<"-------------"<<endl;
    double sigma_sum = 0.0;
    double sigma_sum_0 = 0.0;

    //cout<<"-------"<<endl;
    for(int j=0; j<num; ++j){
        double sigma_i = sigma(p[j], point, 1);
        sigma_sum += sigma_i;
        //cout<<sigma(p[j], point, 0)<<","<<sigma_i<<endl;
        sigma_sum_0 += sigma(p[j], point, 0);
    }
    //cout<<"::"<<endl;
    //cout<<sigma_sum_0<<","<<sigma_sum<<endl;
    /****/

    double sigma_0 = 0.0001;
    sigma_sum += sigma_0;

    if(sigma_sum == 0){
        //result.push_back(point.x());
        //result.push_back(point.y());
        //return result;
    }


    for(int i=0; i<num; ++i){

        double t11 = t[4*i];
        double t12 = t[4*i+1];
        double t21 = t[4*i+2];
        double t22 = t[4*i+3];

        double f_i_x = q[i].x() + t11*(point.x()-p[i].x()) + t12*(point.y()-p[i].y());
        double f_i_y = q[i].y() + t21*(point.x()-p[i].x()) + t22*(point.y()-p[i].y());


        double w_i_p = sigma(p[i], point, 1) / sigma_sum;
        //cout<<sigma(p[i], point, 0)/(sigma_sum_0)<<","<<sigma(p[i], point, 1)/sigma_sum<<endl;

        f_x += w_i_p * f_i_x;
        f_y += w_i_p * f_i_y;

    }
    //cout<<sigma_sum<<endl;

    /****/
    f_x += point.x() / sigma_sum * sigma_0;
    f_y += point.y() / sigma_sum * sigma_0;

    result.push_back(f_x);
    result.push_back(f_y);
    return result;
}

QImage idwWarping(QImage* image, vector<QPoint> pointList){
    // number of control points
    int num = pointList.size() / 2;

    vector<double> tList;

    vector<QPoint> p;
    vector<QPoint> q;

    for(size_t i=0; i<pointList.size();i+=2){
        p.push_back(pointList[i]);
        q.push_back(pointList[i+1]);
    }

    // for each control point, compute corresponding
    // t11, t12, t21, t22
    for(int i=0; i<num; ++i){
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
        for(int j=0; j<num; ++j){
            if(j != i){
                double sigma_value = sigma(p[i], p[j], 1);
                double spx = p[j].x() - p[i].x();
                double spy = p[j].y() - p[i].y();
                double sqx = q[j].x() - q[i].x();
                double sqy = q[j].y() - q[i].y();

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
                t_backup += a + e;
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
        //cout<<t11<<","<<t12<<","<<t21<<","<<t22<<endl;


        tList.push_back(t11);
        tList.push_back(t12);
        tList.push_back(t21);
        tList.push_back(t22);
    }



    vector<double> trans;
    vector<double> pixel_x;
    vector<double> pixel_y;

    for (int i=0; i<(image->width()); i++){
        for (int j=0; j<(image->height()); j++){
            trans = f(p, q, tList, QPoint(i, j));
            pixel_x.push_back(trans[0]);
            pixel_y.push_back(trans[1]);

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

    QImage image_result(int(width_new+2), int(height_new+2), QImage::Format(4));
    image_result.fill(Qt::white);


    for (int i=0; i<(image->width()); i++){
        for (int j=0; j<(image->height()); j++){
            image_result.setPixel(int(pixel_x[i*(image->width())+j]), int(pixel_y[i*(image->width())+j]), image->pixel(i, j));
        }
    }

    return image_result;
}



#endif // IMAGEWARPING_H
