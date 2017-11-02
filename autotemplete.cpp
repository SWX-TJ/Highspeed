#include <autotemplete.h>
using namespace autotemepfunc;
Mat autotemepfunc::Check(Mat image){
  TraversalPixels(image,greyFilter);
  SearchLine(image);
  return image;
}


Mat autotemepfunc::SearchLine(Mat img){
  int upThreshold=240;
  int downThreshold=100;
  for(int i=0;i<img.rows;i++){
      uchar* data=img.ptr<uchar>(i);
      int pNum=0;
      int lastPixel=data[0];
      for(int j=0;j<img.cols;j++){
    if(data[j]>0){
      pNum++;
    }
    //Continuity. Rising edge.
    if(data[j]>lastPixel && lastPixel==0){
      if(j>(img.cols/2)){
        for(int k=j;k<img.cols;k++){
          data[k]=0;
        }
        break;
      }
    }
    //Continuity. Falling edge.
    if(data[j]<lastPixel && data[j]==0){
      int RightUp=0;
      if(i>0){
        RightUp=img.ptr<uchar>(i-1)[j];//Top right corner
        //Perhaps checking top right corner is no use.
      }
      if(j<=(img.cols/2) && RightUp==0){
        for(int k=0;k<j;k++){
          data[k]=0;
        }
        pNum=0;
      }
    }
    lastPixel=data[j];
      }
      if(pNum>upThreshold||pNum<downThreshold){
    for(int j=0;j<img.cols;j++){
      data[j]=0;
    }
      }
  }
  return img;
}

int getVal(int x){
  return x<150?0:x;
}


Mat autotemepfunc::TraversalPixels(Mat image,int (*func)(int val)){
  MatIterator_<uchar> pix,begin,end;
  begin=image.begin<uchar>();
  end=image.end<uchar>();
  for(pix=begin;pix!=end;pix++){
    *pix=func(*pix);
  }
  return image;
}

PointList Filter(Mat image,int (*func)(int greyVal,PixelPoint pt)){
  PointList L;
  PixelPoint p={0,0};
  //L.push_back(p);
  for(int i=0;i<image.rows;i++){
      uchar* data=image.ptr<uchar>(i);
      for(int j=0;j<image.cols;j++){
    p.row=i;
    p.col=j;
    if(func(data[j],p)){
      L.push_back(p);
    }
      }
  }
  return L;
}
