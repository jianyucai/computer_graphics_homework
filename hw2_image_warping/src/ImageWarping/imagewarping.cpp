#include <QImage>
#include <QPoint>
#include <vector>
#include <cmath>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

double sigma(QPoint p_i, QPoint p, int mu=2){
    double d = pow(pow(p.x()-p_i.x(), 2) + pow(p.y()-p_i.y(), 2), mu/2);
    double result = double(1.0) / d;
    return result;
}

double* f(vector<QPoint> p, vector<QPoint> q, vector<double> t, QPoint point){
    double f_x = 0.0;
    double f_y = 0.0;
    double num = p.size();
    for(int i=0; i<num; ++i){
        double t11 = t[4*i];
        double t12 = t[4*i+1];
        double t21 = t[4*i+2];
        double t22 = t[4*i+3];

        double f_i_x = q[i].x() + t11*(point.x()-p[i].x()) + t12*(point.y()-p[i].y());
        double f_i_y = q[i].y() + t21*(point.x()-p[i].x()) + t22*(point.y()-p[i].y());

        double sigma_sum = 0.0;
        for(int j=0; j<num; ++j)
            sigma_sum += sigma(p[j], point);

        double w_i_p = sigma(p[i], point) / sigma_sum;

        f_x += w_i_p * f_i_x;
        f_y += w_i_p * f_i_y;
    }
    double *result = new double[2];
    result[1] = f_x;
    result[2] = f_y;

    return result;
}

extern QImage* idwWarping(QImage* image, vector<QPoint> pointList){
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
        double a11 = 0.0, a12 = 0.0, a13 = 0.0;
        double a21 = 0.0, a22 = 0.0, a23 = 0.0;
        double a31 = 0.0, a32 = 0.0, a33 = 0.0;
        double a41 = 0.0, a42 = 0.0, a43 = 0.0;


        // update all parameters
        for(int j=0; j<num; ++j){
            if(j != i){
                double sigma_value = sigma(p[i], p[j]);
                double a = p[j].x() - p[i].x();
                double b = p[j].y() - p[i].y();
                double c = q[i].x() - q[j].y();
                double d = q[i].y() - q[j].y();

                a11 += sigma_value * a * a;
                a12 += sigma_value * a * b;
                a13 += sigma_value * a * c;

                a21 += sigma_value * a * b;
                a22 += sigma_value * b * b;
                a23 += sigma_value * b * c;

                a31 += sigma_value * a * a;
                a32 += sigma_value * a * b;
                a33 += sigma_value * a * d;

                a41 += sigma_value * a * b;
                a42 += sigma_value * b * b;
                a43 += sigma_value * b * d;
            }
        }

        // solve equations
        Matrix2d A1;
        A1 << a11, a12,
              a21, a22;

        Matrix2d A2;
        A2 << a31, a32,
              a41, a42;

        Vector2d B1(-a13, -a23);
        Vector2d B2(-a33, -a43);

        Vector2d x1 = A1.colPivHouseholderQr().solve(B1);
        Vector2d x2 = A2.colPivHouseholderQr().solve(B1);

        t11 = x1(1);
        t12 = x1(2);
        t21 = x2(1);
        t22 = x2(2);

        tList.push_back(t11);
        tList.push_back(t12);
        tList.push_back(t21);
        tList.push_back(t22);
    }

    QImage image_result(*(image));

    for (int i=0; i<image->width(); i++){
        for (int j=0; j<image->height(); j++){
            double* trans = f(p, q, tList, QPoint(i, j));
            image_result.setPixel(trans[0], trans[1], image->pixel(i, j));
        }
    }

    return &image_result;
}


