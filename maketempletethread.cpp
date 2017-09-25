#include "maketempletethread.h"
MakeTempleteThread::MakeTempleteThread()
{

     isdrawleft = false;
     isdrawright  = false;
}

QImage MakeTempleteThread::convertMatToQImage(Mat &mat)
{
    QImage img;
    int nChannel=mat.channels();
    if(nChannel==3)
    {
        cvtColor(mat,mat,CV_BGR2RGB);
        img = QImage((const unsigned char*)mat.data,mat.cols,mat.rows,QImage::Format_RGB888);
    }
    else if(nChannel==4)
    {
        img = QImage((const unsigned char*)mat.data,mat.cols,mat.rows,QImage::Format_ARGB32);
    }
    else if(nChannel==1)
    {
        img = QImage((const unsigned char*)mat.data,mat.cols,mat.rows,QImage::Format_Indexed8);
    }
    return img;
}

Mat MakeTempleteThread::convertQImageToMat(QImage &image)
{
    cv::Mat mat;
    switch (image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    }
    return mat;
}

void MakeTempleteThread::accept_leftargImage(QPoint temppoint, QImage oriImage)
{
  LeftPoint  = temppoint;
  OriImage  = convertQImageToMat(oriImage);
  isdrawleft  = true;
}

void MakeTempleteThread::accept_rightargImage(QPoint temppoint, QImage oriImage)
{
     RightPoint  = temppoint;
     OriImage  = convertQImageToMat(oriImage);
     isdrawright  = true;
}

void MakeTempleteThread::run()
{
  while(1)
  {
     if(isdrawleft)
     {
         isdrawleft = false;
         rectangle(OriImage,Rect(0,0,LeftPoint.x(),LeftPoint.y()-40),Scalar(0,255,0),1,8);
         send_drawImage(convertMatToQImage(OriImage));
     }
     else if(isdrawright)
     {
         isdrawright = false;
         rectangle(OriImage,Rect(Point(OriImage.cols,OriImage.rows),Point(RightPoint.x(),RightPoint.y()-40)),Scalar(255,0,0),1,8);
         send_drawImage(convertMatToQImage(OriImage));
     }
  }
}
