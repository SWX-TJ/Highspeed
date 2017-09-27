#include "imageprocthread.h"
#include <QDebug>
#include <iostream>
ImageProcThread::ImageProcThread()
{
    isTempleteImage = false;
    isLoadTempleteImage = false;
    load_templeteImage();
}

void ImageProcThread::accept_MatchFileInfo(QString filepath)
{
    MatchImageFilePath = filepath;
}

void ImageProcThread::accept_isTempleteFile(int tem)
{
    if(tem ==1)
    {
        isTempleteImage = true;
    }
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
        isLoadTempleteImage = false;
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
        isLoadTempleteImage = true;
    }
}

Mat ImageProcThread::ImageProcess(Mat &oriImage)
{
        Mat procImage = oriImage.clone();
        Mat grayImage;
        grayImage = procImage.clone();
    for(size_t i = 0;i<tempImages.size();i++)
    {
        Mat useTempImage = tempImages.at(i);
        dispTempImage = convertMatToQImage(useTempImage);
        send_templeteImage(dispTempImage);
        int resultImage_cols = grayImage.cols-useTempImage.cols+1;
        int resultImage_rows = grayImage.rows-useTempImage.rows+1;
        resultImage.create(resultImage_rows,resultImage_cols,CV_32FC1);
        matchTemplate(grayImage,useTempImage,resultImage,CV_TM_CCOEFF_NORMED);
        normalize(resultImage,resultImage,0,1,NORM_MINMAX,-1,Mat());
        minMaxLoc(resultImage,&minValue,&maxValue,&minLocation,&maxLocation,Mat());
        matchLocation = maxLocation;
        Rect matchrect = Rect(matchLocation,Point(matchLocation.x+useTempImage.cols,matchLocation.y+useTempImage.rows));
        MatchRects.push_back(matchrect);
    }
    if(MatchRects.size())
    {
        Rect tempRect = MatchRects.at(0);
        minMatchArea =tempRect.area();
        for(size_t j = 0;j<MatchRects.size();j++)
        {
            Rect matchs = MatchRects.at(j);
            if(matchs.area()<minMatchArea)
            {
                minMatchArea = matchs.area();
            }
        }
        for(size_t k = 0;k<MatchRects.size();k++)
        {
            Rect drawrect = MatchRects.at(k);
            if(drawrect.area() == minMatchArea)
            {
                rectangle(procImage,drawrect,Scalar(0,0,255),2,8,0);
                break;
            }
        }
    }
    return procImage;
}

QImage ImageProcThread::convertMatToQImage(Mat &mat)
{
    QImage img;
    int nChannel=mat.channels();
    if(nChannel==3)
    {
        cvtColor(mat,mat,CV_BGR2RGB);
        img = QImage((const unsigned char*)mat.data,mat.cols,mat.rows,QImage::Format_RGB888);
    }
    if(nChannel==4)
    {
        img = QImage((const unsigned char*)mat.data,mat.cols,mat.rows,QImage::Format_ARGB32);
    }
    if(nChannel==1)
    {
        img = QImage((const unsigned char*)mat.data,mat.cols,mat.rows,QImage::Format_Indexed8);
    }
    return img;
}

void ImageProcThread::run()
{
    while(1)
    {
        if(isTempleteImage)
        {
            load_templeteImage();
            isTempleteImage = false;
        }
        if(isLoadTempleteImage)
        {
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
                resuImage = ImageProcess(OriImage);
                dispImage = convertMatToQImage(resuImage);
                send_dispImage(dispImage);
            }
        }
    }
}
