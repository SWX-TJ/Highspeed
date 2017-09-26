#include "imageprocthread.h"
#include <QDebug>
#include <iostream>
ImageProcThread::ImageProcThread()
{
    isTempleteImage = false;
    load_templeteImage();
}

void ImageProcThread::accept_MatchFileInfo(QString filepath)
{
    MatchImageFilePath = filepath;
}

void ImageProcThread::load_templeteImage()
{
    QString templeteImagepath = QCoreApplication::applicationDirPath()+QString("/templete");
    QDir *dir = new QDir(templeteImagepath);
    QStringList filter;
    TempleteImageFileInfo = new QList<QFileInfo>(dir->entryInfoList(filter));
    TempleteImageNum = TempleteImageFileInfo->count();
    if(TempleteImageNum==2)
    {
        QMessageBox::information(NULL,"Erroe","there is no TempleteImage!");
        isTempleteImage = false;
    }
    else
    {
        isTempleteImage = true;
        int i;
        for(i = 2;i<TempleteImageNum ;i++)
        {
            QString filepath =  TempleteImageFileInfo->at(i).filePath().replace('/',"\\");
            std::string stdmatchfilename = filepath.toStdString();
            TempImage = imread(stdmatchfilename);
            tempImages.push_back(TempImage);
        }
    }
}

Mat ImageProcThread::ImageProcess(Mat &oriImage)
{
    Mat procImage = oriImage.clone();
    for(int i = 0;i<tempImages.size();i++)
    {
        Mat useTempImage = tempImages.at(i);
        int resultImage_cols = procImage.cols-useTempImage.cols+1;
        int resultImage_rows = procImage.rows-useTempImage.rows+1;
        resultImage.create(resultImage_rows,resultImage_cols,CV_32FC1);
        matchTemplate(procImage,useTempImage,resultImage,CV_TM_CCOEFF_NORMED);
        normalize(resultImage,resultImage,0,1,NORM_MINMAX,-1,Mat());
        double minValue,maxValue;
        Point minLocation;
        Point maxLocation;
        Point matchLocation;
        minMaxLoc(resultImage,&minValue,&maxValue,&minLocation,&maxLocation,Mat());
        matchLocation = maxLocation;
        rectangle(procImage,matchLocation,Point(matchLocation.x+useTempImage.cols,matchLocation.y+useTempImage.rows),Scalar(0,0,255),2,8,0);
    }
    return procImage;
}

void ImageProcThread::run()
{
    while(1)
    {
    if(isTempleteImage)
    {
        isTempleteImage = false;
        QDir *dir = new QDir(MatchImageFilePath);
        QStringList filter;
        MatchImageFileInfo = new QList<QFileInfo>(dir->entryInfoList(filter));
        MatchImageNum = MatchImageFileInfo->count();
        int i;
        for(i = 2;i<MatchImageNum;i++)
        {
            QString filepath =  MatchImageFileInfo->at(i).filePath().replace('/',"\\");
            std::string stdmatchfilename = filepath.toStdString();
            OriImage = imread(stdmatchfilename);
            Mat resuImage = ImageProcess(OriImage);
        imshow("match",resuImage);
        waitKey(100);
        }
    }
    }
}
