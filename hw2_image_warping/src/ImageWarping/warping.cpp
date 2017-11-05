#include "warping.h"

Warping::Warping()
{

}

QImage Warping::remove_white_gap(QImage* warped_image, vector<double> pixel_x, vector<double> pixel_y){
    QImage image_backup = *warped_image;

    int width = image_backup.width();
    int height = image_backup.height();

    // store the pixel which has values and doesn't
    // use 1 and 0 separately
    bool **image_mat = new bool*[width];
    for(int i=0; i<width; ++i){
        image_mat[i] = new bool[height];
        for(int j=0; j<height; ++j){
            image_mat[i][j] = 0;
        }
    }

    // store the border points (left border and right border)
    // for each line
    int **border = new int*[width];
    for(int i=0; i<width; ++i){
        border[i] = new int[2];
        for(int j=0; j<2; ++j){
            border[i][j] = 0;
        }
    }

    // use pixel_x and pixel_y to fill the image_mat
    for(int i=0; i<pixel_x.size(); ++i){
        int x = pixel_x[i];
        int y = pixel_y[i];

        image_mat[x][y] = 1;
    }
    /*
    for(int i=0; i<width; ++i){
        for(int j=0; j<height; ++j){
            cout<<image_mat[i][j];
        }
        cout<<endl;
    }
    */

    // for each line, find left border and right border
    for(int i=0; i<width; ++i){
        int left = 0;
        int right = height-1;

        // find left border
        while(image_mat[i][left] == 0 && left < height){
            left += 1;
        }

        // find right border
        while(image_mat[i][right] == 0 && right >=0){
            right -= 1;
        }

        border[i][0] = left;
        border[i][1] = right;
        //cout<<left<<","<<right<<endl;
    }


    // for all points that are in white gap
    // find the nearest point
    vector<QPoint> filled_point_list;
    for(int i=0; i<width; ++i){
        int left = border[i][0];
        int right = border[i][1];

        for(int j=left; j<=right; ++j){

            if(image_mat[i][j] == 0){
                bool hasFound = false;
                int range = 1;

                while (!hasFound) {
                    // x = [i-range ~ i+range+1]
                    // y = j-range
                    for(int x=i-range, y=j-range; x<i+range+1; ++x){
                        if(x >= 0 && y >=0 && x < width && y < height
                                && image_mat[x][y] == 1){
                            filled_point_list.push_back(QPoint(i, j));
                            filled_point_list.push_back(QPoint(x, y));
                            hasFound = true;
                            break;
                        }
                    }
                    if(hasFound)
                        break;

                    // x = [i-range ~ i+range+1]
                    // y = j+range+1
                    for(int x=i-range, y=j+range+1; x<i+range+1; ++x){
                        if(x >= 0 && y >=0 && x < width && y < height
                                && image_mat[x][y] == 1){
                            filled_point_list.push_back(QPoint(i, j));
                            filled_point_list.push_back(QPoint(x, y));
                            hasFound = true;
                            break;
                        }
                    }
                    if(hasFound)
                        break;

                    // x = i-range
                    // y = [j-range ~ j+range+1]
                    for(int y=j-range, x=i-range; y<j+range+1; ++y){
                        if(x >= 0 && y >=0 && x < width && y < height
                                && image_mat[x][y] == 1){
                            filled_point_list.push_back(QPoint(i, j));
                            filled_point_list.push_back(QPoint(x, y));
                            hasFound = true;
                            break;
                        }
                    }
                    if(hasFound)
                        break;

                    // x = i+range+1
                    // y = [j-range ~ j+range+1]
                    for(int y=j-range, x=i+range+1; y<j+range+1; ++y){
                        if(x >= 0 && y >=0 && x < width && y < height
                                && image_mat[x][y] == 1){
                            filled_point_list.push_back(QPoint(i, j));
                            filled_point_list.push_back(QPoint(x, y));
                            hasFound = true;
                            break;
                        }
                    }
                    if(hasFound)
                        break;

                    range += 1;
                }
            }
        }
    }


    for(int i=0; i<filled_point_list.size(); i+=2){
        QPoint point  = filled_point_list[i];
        QPoint refer_point = filled_point_list[i+1];

        image_backup.setPixel(point.x(), point.y(), warped_image->pixel(refer_point.x(), refer_point.y()));
    }


    return image_backup;
}
