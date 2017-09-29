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
       double tim0 = static_cast<double>(getTickCount());
    for(size_t i = 0;i<tempImages.size();i++)
    {
        Mat useTempImage = tempImages.at(i);
        dispTempImage = convertMatToQImage(useTempImage);
        send_templeteImage(dispTempImage);

        cvtColor(procImage,grayImage,COLOR_BGR2GRAY);
        Mat grayTempImage;
        cvtColor(useTempImage,grayTempImage,COLOR_BGR2GRAY);
        Mat grayresultImage;
        matchTemplate(grayImage,grayTempImage,grayresultImage,CV_TM_CCOEFF);
        normalize(grayresultImage,grayresultImage,0,1,NORM_MINMAX,-1,Mat());
        minMaxLoc(grayresultImage,&minValue,&maxValue,&minLocation,&maxLocation,Mat());
       qDebug()<<"Match maxvalue"<<maxValue<<endl;
        matchLocation = maxLocation;
        Rect matchrect = Rect(matchLocation,Point(matchLocation.x+useTempImage.cols,matchLocation.y+useTempImage.rows));
        MatchRects.push_back(matchrect);
    }
    if(MatchRects.size())
    {
        Rect minRect = MatchRects.at(0);
        int minRectArea = minRect.area();
        for(size_t j = 0;j<MatchRects.size();j++)
        {
            Rect tempRect = MatchRects.at(j);
            if(tempRect.area()<minRectArea)
            {
                minRectArea = tempRect.area();
            }
        }
        tim0 = (static_cast<double>(getTickCount()) - tim0) / getTickFrequency();
        QString Danwei = QString("ms");
        QString send_time = SpendTime.setNum(tim0*1000)+Danwei;
        send_sendTime(send_time);
        for(size_t k = 0;k<MatchRects.size();k++)
        {
            Rect tempRects = MatchRects.at(k);
            if(tempRects.area() == minRectArea)
            {
                Mat tempImage = tempImages.at(k);
                Mat grayTempImage;
                cvtColor(tempImage,grayTempImage,COLOR_BGR2GRAY);
                Mat ssImage = RoiImageProcess(procImage,tempRects);
                Mat  sprocImage = newLineDetect(grayImage, ssImage,tempRects);
                rectangle(procImage,tempRects,Scalar(0,255,0),2,8,0);
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

Mat ImageProcThread::RoiImageProcess(Mat &RoiImage,Rect &RoiRect)
{
    Mat procImage =RoiImage(RoiRect);
    Mat grayprocImage;
    cvtColor( procImage,grayprocImage,COLOR_BGR2GRAY);
    Mat sobel_x;
    Sobel(grayprocImage,sobel_x,grayprocImage.depth(),1,0,3);
    Mat abs_sobel_x;
    convertScaleAbs(sobel_x,abs_sobel_x);
    Mat sobel_y;
    Sobel(grayprocImage, sobel_y, grayprocImage.depth(), 0, 1, 3);
    Mat abs_sobel_y;
    convertScaleAbs(sobel_y, abs_sobel_y);
    Mat sobelImage;
    addWeighted(abs_sobel_x,1,abs_sobel_y,1,0,sobelImage);
    Mat binsobelImage;
    threshold(sobelImage, binsobelImage,0, 255, THRESH_OTSU);
    imshow("ostuRoiImage",binsobelImage);
    waitKey(100);
    return binsobelImage;
}

Mat ImageProcThread::newLineDetect(Mat &grayImage, Mat &tempImage, Rect &RoiRect)
{
    Mat procImage = grayImage.clone();
    Ptr<LineSegmentDetector> lsd = createLineSegmentDetector(LSD_REFINE_STD);
    std::vector<Vec4f> vecLines;
    std::vector<Vec4f> select_vecLines;
    lsd->detect(tempImage,vecLines);
    if(vecLines.size()!=0)
        send_ishaveGj(true);
    for(size_t  i = 0;i<vecLines.size();i++)
    {
        float k ;
        Vec4f sline = vecLines.at(i);
        if((sline[2]-sline[0])!= 0)
            k = (sline[3]-sline[1])/(sline[2]-sline[0]);
        else
            k = 0;
        if(k<0.04&&k>(-0.04))
        {

            sline[0] = sline[0]+RoiRect.x;
            sline[1] = sline[1]+RoiRect.y;
            sline[2] = sline[2]+RoiRect.x;
            sline[3] = sline[3]+RoiRect.y;
            line_k.push_back(k);
            //line(procImage,Point(sline[0],sline[1]),Point(sline[2],sline[3]),Scalar(0,0,255),3,8,0);
            select_vecLines.push_back(sline);
        }
    }
    for(size_t l = 0;l<select_vecLines.size();l++)
    {

    }
    imshow("lineImage",procImage);
    waitKey(30);
    //lsd->drawSegments(procImage,select_vecLines);
    return procImage;
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
