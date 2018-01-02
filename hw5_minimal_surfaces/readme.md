Minimal Surfaces
====

**Minimal Surfaces** is a simple program that constructs minimal surface with given boundaries.

## Overview
This program provides a simple GUI for users to load an *.obj* file, and compute the minimal surface with the help of *HE_Mesh* interfaces.

### Basic Idea
The basic idea of this algorithm is to generate the minimal surface given boundaries.
 
### Minimal Surface
A minimal surface is a surface that locally minimizes its area. This is equivalent to having zero mean curvature.

The formula can be found in [wiki](https://en.wikipedia.org/wiki/Minimal_surface). 

The formula is based on continous condition. The discrete condition is illustrated in reference [1], which can be explained as: each point is in the position of barycenter of its neibours. 

The minimal surface can be generated given some points in the space and a boundary with in the points.

The process of generating minimal surface can be viewed as moving the oringinal points to new places in order to satisfy the conditions of minimal surface, with the boundary remains unchanged. So, we only need to care about the coordinates of boundary points, which are constant.

There are 2 methods to generate minmal surface, global method and local method.

### Global Method
The idea of Global Method is to solve linear equations, where the coodinates all points (beside boundary points) are unknown variables. The equations are listed based on the discrete form of the formula of minimal surface.

### Local Method
The idea of Local Method is to do the iteration. In each iteration, each point moves a step towards the barycenter of its neibour points. The step size and iteration times can can be adjusted to reach optimized combination. 
 
## Dependency
**QT 5.7.1** is used for compiling and running this program.

**Eigen** is used in this program, please check the *.pro* file for right Eigen lib path.
 
 **MiniMeshFrame** is a framework to display the operations of *.obj* file, which is used in this program as GUI.
 
## Reference
[The research on the algorithm of generating Minimal Surface given boundaries](http://www.doc88.com/p-9119776101740.html)