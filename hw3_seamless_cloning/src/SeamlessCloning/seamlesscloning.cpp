#include "seamlesscloning.h"
#include <QPoint>
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/SparseLU>
#include "utils.h"

using namespace std;
using namespace Eigen;

SeamlessCloning::SeamlessCloning(QImage* source_img, QImage* cloned_img, vector<QPoint> s_list, vector<QPoint> c_list){
    this->source_image = source_img;
    this->cloned_image = cloned_img;
    this->source_point_list = s_list;
    this->cloned_point_list = c_list;

    /* initialize clone_map */
    clone_map = new int*[cloned_image->width()];

    for(int i=0; i<cloned_image->width(); ++i){
        clone_map[i] = new int[cloned_image->height()];
        for(int j=0; j<cloned_image->height(); ++j){
            clone_map[i][j] = 0;
        }
    }

    for(int i=0; i<cloned_point_list.size(); ++i){
        int x = cloned_point_list[i].x();
        int y = cloned_point_list[i].y();
        clone_map[x][y] = i+1;
    }

    /* initialize border_map */
    border_map = new bool*[cloned_image->width()];

    for(int i=0; i<cloned_image->width(); ++i){
        border_map[i] = new bool[cloned_image->height()];
    }

    for(int i=1; i<cloned_image->width()-1; ++i){
        for(int j=1; j<cloned_image->height()-1; ++j){
            border_map[i][j] = on_border(QPoint(i, j));
        }
    }

}


vector<QPoint> SeamlessCloning::get_neibours(QPoint p){
    vector<QPoint> neibours;
    int x = p.x();
    int y = p.y();

    /* left neibour */
    if(x-1 >=0 && x-1 < cloned_image->width()){
        neibours.push_back(QPoint(x-1, y));
    }

    /* right neibour */
    if(x+1 >=0 && x+1 < cloned_image->width()){
        neibours.push_back(QPoint(x+1, y));
    }

    /* top neibour */
    if(y+1 >=0 && y+1 < cloned_image->height()){
        neibours.push_back(QPoint(x, y+1));
    }

    /* bottom neibour */
    if(y-1 >=0 && y-1 < cloned_image->height()){
        neibours.push_back(QPoint(x, y-1));
    }

    return neibours;
}


bool SeamlessCloning::on_border(QPoint p){
    int x = p.x();
    int y = p.y();

    if(clone_map[x][y] != 0){ // if point in cloned zone
        return false;
    }

    if (!(clone_map[x-1][y] == 0 && clone_map[x+1][y] == 0) &&
            clone_map[x-1][y] * clone_map[x+1][y] == 0 ){
        return true;
    }

    if (!(clone_map[x][y-1] == 0 && clone_map[x][y+1] == 0) &&
            clone_map[x][y-1] * clone_map[x][y+1] == 0 ){
        return true;
    }

    return false;
}

void SeamlessCloning::seamless_cloning(){
    int cloned_point_num = cloned_point_list.size();

    /* parameter matrix A */
    SparseMatrix<double> A(cloned_point_num, cloned_point_num);
    A.reserve(VectorXi::Constant(cloned_point_num, 5));

    /* result vector b */
    VectorXd b_red(cloned_point_num, 1);
    VectorXd b_green(cloned_point_num, 1);
    VectorXd b_blue(cloned_point_num, 1);

    for(int i=0; i<cloned_point_num; ++i){
        b_red(i, 0) = 0;
        b_green(i, 0) = 0;
        b_blue(i, 0) = 0;
    }

    /* for each cloned point, fill A and b */
    for(int i=0; i<cloned_point_num; ++i){

        int px = cloned_point_list[i].x();
        int py = cloned_point_list[i].y();

        vector<QPoint> neibours = get_neibours(cloned_point_list[i]);
        A.insert(i, i) = neibours.size();

        /* for each neibour, fill corresponding A and b */
        for(int j=0; j<neibours.size(); ++j){
            int qx = neibours[j].x();
            int qy = neibours[j].y();
            int idx = clone_map[qx][qy] - 1;

            if(idx >= 0){ // if still in cloned zone
                //A(i, idx) = -1;
                A.insert(i, idx) = -1;
            }

            /* coordinates of corresponding points in source image */
            int source_px = source_point_list[i].x();
            int source_py = source_point_list[i].y();

            int source_qx = source_point_list[idx].x();
            int source_qy = source_point_list[idx].y();

            QColor p_color_source = source_image->pixelColor(source_px, source_py);
            QColor q_color_source = source_image->pixelColor(source_qx, source_qy);

            if(border_map[qx][qy] == true){ // if on the border of cloned zone
                b_red(i,0)   = b_red(i,0) + cloned_image->pixelColor(qx, qy).red();
                b_green(i,0) = b_green(i,0) + cloned_image->pixelColor(qx, qy).green();
                b_blue(i,0)  = b_blue(i,0) + cloned_image->pixelColor(qx, qy).blue();

            }

            b_red(i,0)   = b_red(i,0) + p_color_source.red() - q_color_source.red();
            b_green(i,0) = b_green(i,0) + p_color_source.green() - q_color_source.green();
            b_blue(i,0)  = b_blue(i,0) + p_color_source.blue() - q_color_source.blue();
        }
    }

    /* solve the equations */
    SparseLU<SparseMatrix<double> > solver;
    solver.compute(A);

    if(solver.info()!=Success) {
      cout<<"Decomposition failed."<<endl;
    }

    VectorXd color_r = solver.solve(b_red);
    VectorXd color_g = solver.solve(b_green);
    VectorXd color_b = solver.solve(b_blue);

    if(solver.info()!=Success) {
      cout<<"Solving failed."<<endl;

    }

    /* update the color in cloned_image */
    for(int i=0; i<cloned_point_num; ++i){
        int x = cloned_point_list[i].x();
        int y = cloned_point_list[i].y();

        int r = color_r(i, 0);
        int g = color_g(i, 0);
        int b = color_b(i, 0);

        r = std::max(0, r);
        r = std::min(255, r);
        g = std::max(0, g);
        g = std::min(255, g);
        b = std::max(0, b);
        b = std::min(255, b);

        cloned_image->setPixelColor(x, y, QColor(r, g, b));
    }

}

