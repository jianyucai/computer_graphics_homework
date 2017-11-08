Image Warping with IDW and RBF
====
**ImageWarping** is a simple program that uses simple algorithms like **IDW(Inverse Distance Weighting)** and **RBF(Radial Basis Function)** to achieve image warping.


## Overview
This program provides a simple GUI for users to load a image, and choose corresponding algorithm to do the wariping operation. This document only provides a simple description for the algorithms, refer to paper [1] [2] if you want more detail.

### Image Deformation
Image Deformation (same as Image Warping, to some extent) means do the deformation operation on a image. The deformation opration is defined by some control points pairs (p\_i, q\_i). A control point p\_i in the original image moves to position q\_i in the warped image. The movement of all other points is decided by those control points.

### Scattered Data Interpolation
Scattered Data Interpolation is a method to solve Image Deformation Problem. It defines an interpolation function to do the interpolations for control points.

The algorithms used in this project are all based on Scattered Data Interpolation.


#### Sheperd Interpolation
Sheperd Interpolation, or called Inverse Distance weighting, uses **inverse distance** to evaluate influences of diffrent control points.

There are 2 sigma functions proposed in paper [1], both are realized in my code.

The first sigma function uses global distance influence, which means all points in a image will be influenced by any control point.

The second sigma function uses local distance influence, which means one control point only influence the points which is close to it. Users can set the distance threshold.

#### Radial Basis Functions
Radial Basis Functions uses linear combination of basis functions to do the scattered data interpolation.


### White Gap Problem
when a warping operation tends to strecth an image (which is to enlarge it), some white gaps will occur, because the number of original pixels is not enough to fill the new one.

So, we need to fill in those white gaps. I choose a very simple algorithm, for each "white"(or unmatched) pixel in the new image, fill it with the color of the nearest pixel from it.

## Dependencies
**QT 5.7.1** is used for compiling and running this program.

**Eigen** is used for solving linear equations, it is already included in the project (./Eigen)

## Experiment
A simple experiment is implemented on the famous painting: MonaLisa.

![](images/Monalisa_composed.png)

(a) is the original image.
(b) shows the control points (marked in red color). (c) is the warped image using IDW algorithm. (d) is the warped image using RBF algorithm.

From the images, wo may infer that IDW is more balanced in warping, while RBF may be more wild. However, the image using IDW appears to have more unhandled white gaps than RBF, this may be compensated by using more advanced algorithms to handle White Gap Problem, rather than just looking for the nearest point.

 

## Reference
 
[1] *Image Warping with Scattered Data Interpolation, 1995*

[2] *Free Form Deformation with Scattered Data Interpolation Methods, 1992*


