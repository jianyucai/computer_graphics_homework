#include "minimal_surface.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <QString>
#include <vector>
#include <algorithm>
using namespace std;


minimal_surface::minimal_surface(Mesh3D* mesh_ptr){
    this->mesh_ptr = mesh_ptr;
    this->vertex_num = this->mesh_ptr->num_of_vertex_list();
    this->vertex_vec = this->mesh_ptr->get_vertex_list();

    compute_border_vertex();
    compute_neibour_vec();
}

void minimal_surface::compute_border_vertex(){
    for(int i=0; i<this->vertex_vec->size(); ++i){
        bool is_boundary = (*this->vertex_vec)[i]->isOnBoundary();
        if(is_boundary){
            this->border_vertex_vec.push_back(i);
        }
    }
}

void minimal_surface::compute_neibour_vec(){
    for(int i=0; i<this->vertex_vec->size(); ++i){
        this->neibour_vec.push_back((*this->vertex_vec)[i]->get_neighborIdxes());
    }
}

void minimal_surface::compute_minimal_surface_global(){
    int vertex_num = this->vertex_num;
    /* parameter matrix A */
    SparseMatrix<double> A(vertex_num, vertex_num);
    A.reserve(VectorXi::Constant(vertex_num, 50));

    /* parameter matrix b */
    MatrixXd b = MatrixXd::Zero(vertex_num, 3);

    // fill in A and b
    for(int i=0; i<this->neibour_vec.size(); ++i){
        // if the i'th point is border point
        if((*this->vertex_vec)[i]->isOnBoundary()){
            // set A
            A.insert(i, i) = 1;

            // set b
            Vec3f point = (*this->vertex_vec)[i]->position();
            b(i, 0) = point[0];
            b(i, 1) = point[1];
            b(i, 2) = point[2];
        }
        else{
            vector<size_t>::iterator iter;
            for(iter=this->neibour_vec[i].begin(); iter!=this->neibour_vec[i].end(); ++iter){
                A.insert(i, *iter) = -1;
            }

            A.insert(i, i) = this->neibour_vec[i].size();
        }
    }

    // solve equation
    /* solve the equations */
    SparseLU<SparseMatrix<double> > solver;
    solver.compute(A);

    if(solver.info()!=Success) {
      cout<<"Decomposition failed."<<endl;
    }
    this->new_vertex = solver.solve(b);;

    if(solver.info()!=Success) {
      cout<<"Solving failed."<<endl;
    }
}

void minimal_surface::compute_minimal_surface_local(){
    // initialize this->new_vertex
    this->new_vertex = MatrixXd(this->vertex_num, 3);
    for(int i=0; i<this->vertex_num; ++i){
        Vec3f point = (*this->vertex_vec)[i]->position();
        this->new_vertex(i, 0) = point[0];
        this->new_vertex(i, 1) = point[1];
        this->new_vertex(i, 2) = point[2];
    }

    int iter_num = 2000;
    double move_rate = 0.3;
    MatrixXd new_vertex;
    // iterate
    for(int i=0; i<iter_num; ++i){
        new_vertex = this->new_vertex;
        // for each vertex
        for(int j=0; j<this->vertex_num; ++j){
            // if the j'th point is not border point
            if(find(this->border_vertex_vec.begin(),
                    this->border_vertex_vec.end(),
                    j) == this->border_vertex_vec.end()){

                /* compute vector t = (v1 + v2 + ... + vn) / n */
                MatrixXd t = MatrixXd::Zero(1, 3);
                vector<size_t>::iterator iter;
                for(iter=this->neibour_vec[j].begin(); iter!=this->neibour_vec[j].end(); ++iter){
                    t(0, 0) -= this->new_vertex(j, 0) - this->new_vertex(*iter, 0);
                    t(0, 1) -= this->new_vertex(j, 1) - this->new_vertex(*iter, 1);
                    t(0, 2) -= this->new_vertex(j, 2) - this->new_vertex(*iter, 2);
                }
                t /= this->neibour_vec[j].size();

                /* r_new = r + move_rate * t */
                new_vertex.row(j) += move_rate * t.row(0);
            }
        }
        this->new_vertex = new_vertex;
    }
}


void minimal_surface::compute_minimal_surface(Method method){
    switch(method){
        case GLOBAL:{
            this->compute_minimal_surface_global();
            break;
        }
        case LOCAL:{
            this->compute_minimal_surface_local();
            break;
        }
        default:
            break;
        }

    // update current mesh_ptr
    for(int i=0; i<this->vertex_num; ++i){
        Vec3f v3(this->new_vertex(i, 0),
                 this->new_vertex(i, 1),
                 this->new_vertex(i, 2));

        this->mesh_ptr->get_vertex(i)->set_position(v3);
    }
}
