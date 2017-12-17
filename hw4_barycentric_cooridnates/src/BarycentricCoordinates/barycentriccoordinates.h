#ifndef BARYCENTRICCOORDINATES_H
#define BARYCENTRICCOORDINATES_H

#include <vector>
#include <QPoint>
using namespace std;
enum Method{
    WP,
    MV,
    DH
};
class BarycentricCoordinates{
public:
    // the vector params have been substracted biases
    BarycentricCoordinates(int width,
                           int height,
                           vector<QPoint> polygon_vertex_vec,
                           vector<QPoint> triangle_vertex_vec);

    void set_poly_vertex_vec(vector<QPoint> polygon_vertex_vec);

    vector<QPoint> get_poly_vertex_vec();

    void set_tria_vertex_vec(vector<QPoint> triangle_vertex_vec);

    vector<QPoint> get_tria_vertex_vec();

    void set_method(Method method);

    /* compute the triangle area give 3 points */
    double triangle_area(QPoint*, QPoint*, QPoint*);

    /* compute the barycentric coordinates for a point p,
     * given the polygon vertexes around it */
    vector<double> polygon_bary_coord(QPoint p, vector<QPoint> vertex_vec);

    /* do the warping, given the index of dragging point,
     * return QPoint vec, each pair is (old_coordinate, new_coordinate) */
    vector<QPoint> warp(vector<QPoint> new_triangle_vertex_vec);

    // initialize barycentric coorinates for all triangle vertex in polygon
    void init_inner_bary_coor();

    vector<vector<double> > get_inner_polygon_bary_coor_vec();

private:
    double c(QPoint* v, QPoint* vi);
    void triangle_bary_coord(QPoint*, QPoint*, QPoint*,
                             QPoint*, QPoint*, QPoint*);

private:
    Method method;
    int width;
    int height;
    vector<QPoint> polygon_vertex_vec;
    vector<QPoint> triangle_vertex_vec;
    // stores point pair (old point, new point)
    vector<QPoint> warped_point_vec;
    // stores the triangle vertex's barycentric coodinates inside the polygon
    vector<vector<double> > inner_polygon_bary_coor_vec;

};

#endif // BARYCENTRICCOORDINATES_H
