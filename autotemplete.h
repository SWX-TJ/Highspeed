#ifndef AUTOTEMPLETE_H
#define AUTOTEMPLETE_H
#include <iostream>
#include <vector>
#include <list>
#include <opencv.hpp>
using namespace std;
using namespace cv;

typedef struct pixelpoint{
  int row;
  int col;
} PixelPoint;
namespace autotemepfunc {

typedef list<PixelPoint> PointList;
//move to head file
//for every pixel p,excute func(p).change the image.
Mat TraversalPixels(Mat image,int (*func)(int val));
//return a list. It contains' the coordinates of the points that func(point)!=0.
PointList Filter(Mat image,int (*func)(int greyVal,PixelPoint pt));
//Greyscale Filtering
const int greyThreshold =150;
int getVal(int x);

Mat SearchLine(Mat img);
Mat Check(Mat image);
//above is head file
inline int greyFilter(int greyVal){return greyVal>greyThreshold?greyVal:0;}
}
#endif // AUTOTEMPLETE_H
