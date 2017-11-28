Seamless Image Cloning
====
**SeamlessCloning** is a simple program that uses **Poisson Equations** to achieve Seamless Image Cloning.

## Overview
This program provides a simple GUI for users to load an image, and choose certain rectangle area in **source image**, then clone this area to a certain place in a background image, finally use alogrithms to achieve seamless cloning effects.

Please refer to paper [1] for complete algorithm and details in applications. Image Cloning is only one achievement in this paper.

### Image Cloning Problem
When we copy an area of source image to a certain place in background image, the mixed image is unatual and strange for us human beings. For there lacks smooth transition between cloned zone and background zone. Besides, the tones in the cloned zone is likely to unmatch that in background zone. Thus, the The task is to achieve **Seamless Image Cloning** to solve those problems.

### Idea of algorithm
The math formulas in tha paper are so complicated, which needs not my description. Here, I only introduce the basic ideas of this algorithm to help you understand the thougths.

#### Divergence
Divergence is used in this algorithm, an image can be viewed as 3 separate vector fileds in color value(red, green, blue). Divergence is used to represent some features in a color vector filed.

Divergence can be computed with **Laplace Convolutional Kernel**, which is a 3*3 matrix [0, 1, 0; 1, -4, 1; 0, 1, 0]. It means that the divergence of a pixel can be computed with its 4 neibours.

#### Poisson Image Reconstruction
Since we have extracted the features(divergence) of each pixel in a image, can we rebuild the origial image using the divergences? The answer is yes, as long as some pixel color values are provided.

For example, we have a 4*4 image, using the divergence formula ahead, 4 linear equations canbe listed, which means the divergence for then central 4 pixels. However, these 4 equations involve 16 variables, because the outer pixels are all on boundary, so there is no definition of divregence for them. 

Thus, we need to make some restrictions on the outer pixels, in order to solve the linear equations. The restrictions are, the outer pixels values remains unchanged (or we can say the outer pixels' value are given beforehead). Thus, there are 16 equations with 16 variables, the equations have unique solutions.

#### Poisson Image Cloning
With the idea ahead, the basic idea of Poisson Image Cloning is, First, stores the divergence features of source image. Second, copy a part of source image into background image. Third, make use of the border values of cloned zone in background image as restrictions, *n* linear equations canbe listed (*n* is the number of pixels in cloned zone). Solutions to the *n* linear equations is the new pixel values of the cloned zone.

## Dependency
**QT 5.7.1** is used for compiling and running this program.

**Eigen** is used for solving linear equations, it is already included in the project (./Eigen). Sparse Matrix is used in this project.

## Experiment
For the limitation in time, I only realized selecting cloned zone with rectangles. 

The computation in seamless cloning can be devided into 3 states. 

1. compute the divergence of selected zone when copy button is clicked.
2. fill in the parameter matrix A and vector b when paste button is clicked.
3. solve the linear equations when clone button is clicked.

I initially chose to compute all those stages at one time when clone button is clicked, which is stupid. But I have no deisre to modify the code. Just mark here to illustrtate that I know this stuff.

The experiment result is the same in Image Cloning part of paper [1].

## Reference
[1] *Poisson Image Editing, Microsoft Search UK* 