#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include <iostream>
#include <vector>
#include <QPoint>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "barycentriccoordinates.h"
using namespace std;
using namespace cv;

class Triangulation{
public:
    Triangulation();
    /* return the delaunary triangulation given vertex of polygon
     * @param vector<QPoint>: the vec of polygon vertex
     * @param int: width of image
     * @param int: height of image
     * return: the vec of triangle vertex, 3 vectex per triangle
     */
    static vector<Point2f> delaunary(vector<QPoint> vertex_vec_qpoint, int width, int height){
        vector<Point2f> vertex_vec_point2f = Triangulation::QPoint_vec2Point2f_vec(vertex_vec_qpoint);
        vector<Point2f> triangle_vertex_vec_point2f;
        //vector<QPoint> triangle_vertex_vec_qpoint;

        Rect rect(0, 0, width, height);
        Subdiv2D subdiv(rect);

        // insert vertex points into subdiv
        for(size_t i=0; i<vertex_vec_point2f.size(); ++i){
            subdiv.insert(vertex_vec_point2f[i]);
        }

        vector<Vec6f> triangle_list;
        subdiv.getTriangleList(triangle_list);

        vector<Point2f> pt(3);
        for(size_t i=0; i<triangle_list.size(); ++i){
            Vec6f t = triangle_list[i];
            pt[0] = Point2f(cvRound(t[0]), cvRound(t[1]));
            pt[1] = Point2f(cvRound(t[2]), cvRound(t[3]));
            pt[2] = Point2f(cvRound(t[4]), cvRound(t[5]));

            // rule out the points outside the rect
            if ( rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2])){
                triangle_vertex_vec_point2f.push_back(pt[0]);
                triangle_vertex_vec_point2f.push_back(pt[1]);
                triangle_vertex_vec_point2f.push_back(pt[2]);
            }
        }

        //triangle_vertex_vec_qpoint = Triangulation::Point2f_vec2QPoint_vec(triangle_vertex_vec_point2f);
        //return triangle_vertex_vec_qpoint;
        return triangle_vertex_vec_point2f;
    }


    static vector<QPoint> triangulate(vector<QPoint> vertex_vec_qpoint, int width, int height){
        // initial triangle vertex point vec
        vector<Point2f> init_triangle_vertex_vec_point2f = Triangulation::delaunary(vertex_vec_qpoint,
                                                                                    width, height);

        // add barycentrix point of each triangle and compute again
        vector<QPoint> extended_vertex_vec_qpoint = vertex_vec_qpoint;
        for(int i=0; i<init_triangle_vertex_vec_point2f.size(); i+=3){
            int barycentric_x = (init_triangle_vertex_vec_point2f[i].x +
                                 init_triangle_vertex_vec_point2f[i+1].x +
                                 init_triangle_vertex_vec_point2f[i+2].x) / 3;

            int barycentric_y = (init_triangle_vertex_vec_point2f[i].y +
                                 init_triangle_vertex_vec_point2f[i+1].y +
                                 init_triangle_vertex_vec_point2f[i+2].y) / 3;

            extended_vertex_vec_qpoint.push_back(QPoint(barycentric_x, barycentric_y));
        }

        // do the Delaunary triangulation for extended vertex vec
        vector<Point2f> extended_triangle_vertex_vec_point2f = Triangulation::delaunary(extended_vertex_vec_qpoint,
                                                                                    width, height);
        return Triangulation::Point2f_vec2QPoint_vec(extended_triangle_vertex_vec_point2f);
    }

    // given the polygon vertex that is being dragged, do the triangulation
    static vector<QPoint> drag_triangulate_real_time(int idx,
                                                     QPoint new_point,
                                                     vector<QPoint> polygon_vertex_vec,
                                                     vector<QPoint> triangle_vertex_vec,
                                                     BarycentricCoordinates* bc){
        vector<QPoint> dragged_polygon_vertex_vec = polygon_vertex_vec;
        dragged_polygon_vertex_vec[idx] = new_point;

        vector<QPoint> dragged_triangle_vertex_vec = triangle_vertex_vec;

        // for each triangle vertex inside the polygon
        for(int i=0; i<triangle_vertex_vec.size(); ++i){
            vector<QPoint>::iterator iter = std::find(polygon_vertex_vec.begin(),
                                polygon_vertex_vec.end(),
                                triangle_vertex_vec[i]);

            // if a triangle vertex is not polygon vertex
            if(iter == polygon_vertex_vec.end()){
                vector<double> bary_coor = bc->get_inner_polygon_bary_coor_vec()[i];
                int new_x = 0;
                int new_y = 0;
                for(int j=0; j<bary_coor.size(); ++j){
                    new_x += bary_coor[j] * dragged_polygon_vertex_vec[j].x();
                    new_y += bary_coor[j] * dragged_polygon_vertex_vec[j].y();
                }

                dragged_triangle_vertex_vec[i] = QPoint(new_x, new_y);
            }

            // update all old_points in triangle vertex vec to new_point
            if(dragged_triangle_vertex_vec[i] == polygon_vertex_vec[idx]){
                dragged_triangle_vertex_vec[i] = new_point;
            }
        }

        return dragged_triangle_vertex_vec;
    }

private:
    static vector<Point2f> QPoint_vec2Point2f_vec(vector<QPoint> vertex_vec_qpoint){
        vector<Point2f> vertex_vec_point2f;
        for(int i=0; i<vertex_vec_qpoint.size(); ++i){
            vertex_vec_point2f.push_back(Point2f(vertex_vec_qpoint[i].x(), vertex_vec_qpoint[i].y()));
        }
        return vertex_vec_point2f;
    }


    static vector<QPoint> Point2f_vec2QPoint_vec(vector<Point2f> vertex_vec_point2f){
        vector<QPoint> vertex_vec_qpoint;
        for(int i=0; i<vertex_vec_point2f.size(); ++i){
            vertex_vec_qpoint.push_back(QPoint(vertex_vec_point2f[i].x, vertex_vec_point2f[i].y));
        }
        return vertex_vec_qpoint;
    }

};

#endif // TRIANGULATION_H
