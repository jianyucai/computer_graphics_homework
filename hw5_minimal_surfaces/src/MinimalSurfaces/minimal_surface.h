#ifndef MINIMAL_SURFACE_H
#define MINIMAL_SURFACE_H

#include <string>
#include <vector>
#include <set>
#include <Eigen/Dense>
#include <Eigen/SparseLU>
#include "HE_mesh/Mesh3D.h"

using namespace Eigen;
using namespace std;

enum Method{
  GLOBAL,
  LOCAL
};

class minimal_surface{
private:
    Mesh3D* mesh_ptr;
    int vertex_num;
    // unchanged file content
    vector<string> file_content;
    // stores the vertexes
    vector<HE_vert* >* vertex_vec;
    // stores the border vectexes
    vector<int> border_vertex_vec;
    // stores the neibours of each vertex
    vector<vector<size_t> > neibour_vec;
    // store the coodinates of new vertex
    MatrixXd new_vertex;

public:
    minimal_surface(Mesh3D* mesh_ptr);
    void compute_minimal_surface(Method method);
    // compute minimal surface use global method
    void compute_minimal_surface_global();
    // compute minimal surface use local method
    void compute_minimal_surface_local();

private:
    // compute border vertex
    void compute_border_vertex();
    // compute the neibour_vec
    void compute_neibour_vec();
};

#endif // MINIMAL_SURFACE_H
