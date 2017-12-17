Image Warping Based On Barycentric Coordinates
====

**BaryCentric Coordinates** is a simple program that uses Barycentric Coordinates to realize Image Warping.

## Overview
This program provides a simple GUI for users to load an image, and draw a **polygon** area in the image, then **drag** a vertex of this polygon to do Image Warping.

### Basic Idea
The basic Idea of this algorithm is to split the image or the selected region of the image into triangle area and do the image warping on each triangle area rather than on the whole image. The following contents will include all stages of this algorithm.

### Triangulation
Triangulation is the process of splitting a certain area into small triangles. 

In this program, the user draw a polygon on the image, and the vertex of this polygon is used to do triangulation. One of most popular algorithm to do triangulation is **Delaunay Triangulation**. 

OpenCV provide a set of APIs for Delaunay Triangulation. However, Delaunay Triangulation can only generate very few number of triangles, if we only provide the vertex of a polygon. So, I do a second triangulation based on the result of  Delaunay Triangulation. I devide each triangle in the result of Delaunay Triangulation into 3 triangles based on its barycentric. You can iterate more times to generate more triangles if you like.

### Barycentric Coordinates

Barycentric Coodinates of a point in a polygon, can be viewd as the influence of each vertex of the polygon exerted on the point, or say weights. 

After the triangulation is done, we compute barycentric coodinates for all triangle vertex inside the polygon based on the vertex of the polygon. Then, compute barycentric coodinates for all points inside each triangle based on the vertex of that triangle.

The computation of barycentric coodinates for points a polygon is rather complicated. Following algorithms are used in this program:

- Wachspress (WP) coordinates- mean value (MV) coordinates- discrete harmonic (DH) coordinates

All those algorithms can be reduced to a very simple form, where they only differs in a function. Please refer to paper [1] for details of those algorithms.

### Image Warping
Image Warping is based on the results of the computation of barycentric coodinates.

If point *p* is a triangle vertex in polygon, then let *w1, w2,..., wn* be the barycentric coodinates of a *p*, the coodinates of *p* can be expressed as *p=w1\*v1+w2\*v2+...+wn\*vn*, when the coodinates of polygon vertex changes, the coordinate of point *p* will also change accordingly.

The same for points inside a triangle.  
 
## Dependency
**QT 5.7.1** is used for compiling and running this program.

**OpenCV** is used in this program, please check the *.pro* file for right opencv lib path.
 
## Reference
[1] *A General Construction of Barycentric Coordinates over Convex Polygons*