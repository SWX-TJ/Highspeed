#include "imageprocthread.h"
#include <QDebug>
#include <iostream>
#include <math.h>
ImageProcThread::ImageProcThread()
{
    isTempleteImage = false;
    isLoadTempleteImage = false;
    last_line[0] =120.0;
    last_line[1] =6.0;
    last_line[2] =80.0;
    last_line[3] =6.0;
     max_k = 0;
     outFile.setFileName("image_point.txt");
     if(!outFile.open(QFile::WriteOnly|QFile::Truncate))
     {

     }


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

double ImageProcThread::myabsd(double a)
{
    if(a<0)
    {
        return 0-a;
    }
    else
        return a;
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
                detect_point = newLineDetect(procImage, ssImage,tempRects);
                line(procImage,Point(detect_point.x-20,detect_point.y),Point(detect_point.x+20,detect_point.y),Scalar(0,0,255),3,8,0);
                line(procImage,Point(detect_point.x,detect_point.y-20),Point(detect_point.x,detect_point.y+20),Scalar(0,0,255),3,8,0);
                send_singleLineInfo(detect_point.x,detect_point.y);
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
    waitKey(100);
    return binsobelImage;
}

Point ImageProcThread::newLineDetect(Mat &grayImage, Mat &tempImage, Rect &RoiRect)
{
    Mat procImage = grayImage.clone();
    Mat dxprocImage = procImage(Rect(Point(0,0),Point(214,109)));
    Mat gray_dxprocImage;
    cvtColor(dxprocImage,gray_dxprocImage,COLOR_BGR2GRAY);
    Mat sobel_x;
    Sobel(gray_dxprocImage,sobel_x,gray_dxprocImage.depth(),1,0,3);
    Mat abs_sobel_x;
    convertScaleAbs(sobel_x,abs_sobel_x);
    Mat sobel_y;
    Sobel(gray_dxprocImage, sobel_y,gray_dxprocImage.depth(), 0, 1, 3);
    Mat abs_sobel_y;
    convertScaleAbs(sobel_y, abs_sobel_y);
    Mat sobelImage;
    addWeighted(abs_sobel_x,1,abs_sobel_y,1,0,sobelImage);
    Mat binsobelImage;
    threshold(sobelImage, binsobelImage,0, 255, THRESH_OTSU);
    std::vector<Point>pt1;
    int nrows = binsobelImage.rows;
    int ncols = binsobelImage.cols;
    for(int i =0;i<nrows-2;i++)
    {
        for (int j = 0;j<ncols-2;j++)
        {
            if(j >=100)
            {
                if(((int)(binsobelImage.at<uchar>(i,j)) ==255)&&((int)(binsobelImage.at<uchar>(i+2,j+2)) ==255))
                {
                    Point temppt1;
                    temppt1.x = j;
                    temppt1.y = i;
                    pt1.push_back(temppt1);
                   break;
                }
            }
        }
    }
    Ptr<LineSegmentDetector> lsd = createLineSegmentDetector(LSD_REFINE_STD);
    Vec4f l2_lines;
    fitLine((Mat)pt1,l2_lines,DIST_L2,0,0.01,0.01);
    line(dxprocImage,pt1.at(0),Point(l2_lines[2],l2_lines[3]),Scalar(0,255,0),3,8,0);
    detect_dx_left_k = (l2_lines[3]-pt1.at(0).y)/(l2_lines[2]-pt1.at(0).x);
    detect_dx_left_b = l2_lines[3]-detect_dx_left_k*l2_lines[2];
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
        double dists = sqrt(((sline[3]-sline[1])*(sline[3]-sline[1])+(sline[2]-sline[0])*(sline[2]-sline[0])));
        float x_biao = RoiRect.x+RoiRect.height/2;
        float y_biao = RoiRect.y +RoiRect.width/2;
        float x_test = (sline[2]+sline[0])/2;
        float y_test = (sline[3]+sline[1])/2;
        if(k<0.04&&k>(-0.04))
        {
            if((x_test-x_biao)>(-30)&&(x_test-x_biao)<10)
            {
                if((y_test+RoiRect.y)<y_biao)
                {
                    if(sline[1]<10)
                    {
                        if(dists>30&&dists<70)
                        {
                            if(sline[2]>sline[0])
                            {
                                float temp_x;
                                temp_x = sline[2];
                                sline[2] = sline[0];
                                sline[0] = temp_x;
                                temp_x = sline[1];
                                sline[1] = sline[3];
                                sline[3] = temp_x;
                            }
                            sline[0] = (last_line[0]+sline[0])/2;
                            sline[1] = (last_line[1]+sline[1])/2;
                            sline[2] = (last_line[2]+sline[2])/2;
                            sline[3] = (last_line[3]+sline[3])/2;
                            last_line[0] = sline[0];
                            last_line[1] = sline[1];
                            last_line[2] = sline[2];
                            last_line[3] = sline[3];
                            line_k.push_back(k);
                            select_vecLines.push_back(sline);
                        }
                    }
                }
            }
        }
    }
    new_line[0] =  last_line[0]+RoiRect.x;
    new_line[1] =  last_line[1]+RoiRect.y;
    new_line[2] =  last_line[2]+RoiRect.x;
    new_line[3] =  last_line[3]+RoiRect.y;
    if(( new_line[2]- new_line[0])!= 0)
        detect_line_param[0] = ( new_line[3]- new_line[1])/( new_line[2]- new_line[0]);
    else
        detect_line_param[0] = 0;
    detect_line_param[1] = new_line[1]-detect_line_param[0]*new_line[0];
    jiechu_point.x = (detect_dx_left_b-detect_line_param[1])/(detect_line_param[0]-detect_dx_left_k);
    jiechu_point.y = detect_dx_left_k*(detect_dx_left_b-detect_line_param[1])/(detect_line_param[0]-detect_dx_left_k)+detect_dx_left_b;
    return jiechu_point;
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
             QTextStream dataFileout(&outFile);
            for(i = 2;i<MatchImageNum;i++)
            {
                QString filepath =  MatchImageFileInfo->at(i).filePath().replace('/',"\\");
                std::string stdmatchfilename = filepath.toStdString();
                OriImage = imread(stdmatchfilename);
                resuImage = ImageProcess(OriImage);
                dataFileout<<"The "<<(i-2)<<" Image "<<QObject::tr("Point:")<<" ("<<detect_point.x<<QObject::tr(",")<<detect_point.y<<")"<<endl;
                dispImage = convertMatToQImage(resuImage);
                send_dispImage(dispImage);
            }
        }
        while(1);
    }
}
