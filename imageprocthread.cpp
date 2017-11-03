#include "imageprocthread.h"
#include <iostream>
#include <math.h>
#include <QDebug>
ImageProcThread::ImageProcThread()
{
    isTempleteImage = false;
    isLoadTempleteImage = false;
    last_line[0] =120.0;
    last_line[1] =6.0;
    last_line[2] =80.0;
    last_line[3] =6.0;
    max_k = 0;
    label = 0;
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

Vec4f ImageProcThread::coordiExchanged(Vec4f &pts)
{
    if(pts[2]>pts[0])
    {
        float temp_x;
        temp_x = pts[2];
        pts[2] = pts[0];
        pts[0] = temp_x;
        temp_x = pts[1];
        pts[1] = pts[3];
        pts[3] = temp_x;
    }
    return pts;
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
        QMessageBox::information(NULL,"Error","there is no TempleteImage!");
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

Vec2d ImageProcThread::pantographLineDetect(Mat &proImage,Rect &RoiRect)
{
    Mat procImage = proImage.clone();
    Vec2d final_pantograhLine_info;
    Ptr<LineSegmentDetector> lsd = createLineSegmentDetector(LSD_REFINE_STD);
    std::vector<Vec4f> vecLines;
    std::vector<Vec4f> select_vecLines;
    lsd->detect(procImage,vecLines);
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
                            sline = coordiExchanged(sline);
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
        final_pantograhLine_info[0] = ( new_line[3]- new_line[1])/( new_line[2]- new_line[0]);
    else
        final_pantograhLine_info[0] = 0;
    final_pantograhLine_info[1] = new_line[1]-final_pantograhLine_info[0]*new_line[0];
    return final_pantograhLine_info;
}

Vec2d ImageProcThread::pantographDetect(Mat &proImage)
{
    Mat procImage = proImage.clone();
    Vec4f l2_lines;
    Vec2d final_pantograph_info;
    Mat dxprocImage = procImage(Rect(Point(0,0),Point(300,100)));
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
    imshow("binsobel",binsobelImage);
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
    fitLine((Mat)pt1,l2_lines,DIST_L2,0,0.01,0.01);
    line(procImage,pt1.at(0),Point(l2_lines[2],l2_lines[3]),Scalar(0,255,0),1,8);
    imshow("line_proc",procImage);
    final_pantograph_info[0] = (l2_lines[3]-pt1.at(0).y)/(l2_lines[2]-pt1.at(0).x);
    final_pantograph_info[1] = l2_lines[3]-final_pantograph_info[0]*l2_lines[2];
    return final_pantograph_info;
}


Point ImageProcThread::newLineDetect(Mat &grayImage, Mat &tempImage, Rect &RoiRect)
{
    Point cacu_final_point;
    detect_pantgraph_line_info  = pantographDetect(grayImage);
    detect_pantgraph_info       = pantographLineDetect(tempImage,RoiRect);
    cacu_final_point.x = (detect_pantgraph_line_info[1]-detect_pantgraph_info[1])/(detect_pantgraph_info[0]- detect_pantgraph_line_info[0]);
    cacu_final_point.y = detect_pantgraph_line_info[0]*(detect_pantgraph_line_info[1]-detect_pantgraph_info[1])/(detect_pantgraph_info[0]-detect_pantgraph_line_info[0])+detect_pantgraph_line_info[1];
    return  cacu_final_point;
}

Vec2d ImageProcThread::m_nLineDetect(Mat &procImage)
{
    Mat proceImage = procImage.clone();
    Mat grayImage;
    cvtColor(proceImage,grayImage,COLOR_BGR2GRAY);
    Mat sstestImage = Check(grayImage);
    Mat binImage;
    threshold(sstestImage,binImage,180,255,THRESH_BINARY);
    std::vector<Point>pt1;
    Vec4f l2_lines;
    for(int i = 0;i<binImage.cols;i++)
    {
        for(int j = 0;j<binImage.rows;j++)
        {
            if(i>150&&i<260)
            {
                if(binImage.at<uchar>(j,i) ==255)
                {
                    Point temppt1;
                    temppt1.x = i;
                    temppt1.y = j;
                    pt1.push_back(temppt1);
                    break;
                }
            }
        }
    }
    fitLine((Mat)pt1,l2_lines,DIST_L2,0,0.01,0.01);
    line(proceImage,pt1.at(1),Point(l2_lines[2],l2_lines[3]),Scalar(0,255,0),3,8);
    Vec2d line_info;
    line_info[0] = (l2_lines[3]-pt1.at(1).y)/(l2_lines[2]-pt1.at(1).x);
    line_info[1] = l2_lines[3]-line_info[0]*l2_lines[2];
    // qDebug()<<"k"<<line_info[0]<<endl;
    imshow("line_proc",proceImage);
    waitKey(30);
    return line_info;
}

Vec2d ImageProcThread::m_npantLineDetect(Mat &procImage)
{
    Mat procesImage = procImage.clone();
    Mat ROIImage = procesImage(Rect(100,0,250,50));
    Mat grayImage;
    cvtColor(ROIImage,grayImage,COLOR_BGR2GRAY);
    Mat sobel_x;
    Sobel(grayImage,sobel_x,grayImage.depth(),1,0,3);
    Mat abs_sobel_x;
    convertScaleAbs(sobel_x,abs_sobel_x);
    Mat sobel_y;
    Sobel(grayImage, sobel_y,grayImage.depth(), 0, 1, 3);
    Mat abs_sobel_y;
    convertScaleAbs(sobel_y, abs_sobel_y);
    Mat sobelImage;
    addWeighted(abs_sobel_x,0.5,abs_sobel_y,0.5,0,sobelImage);
    imshow("sobelImage",sobelImage);
    Mat binsobelImage;
    threshold(sobelImage,binsobelImage,120,255,THRESH_BINARY);
    imshow("binsobel",binsobelImage);
    Ptr<LineSegmentDetector> lsd = createLineSegmentDetector(LSD_REFINE_STD);
    std::vector<Vec4f> vecLines;
    lsd->detect(binsobelImage,vecLines);
    std::vector<Vec4f>std_lines;
    for(size_t i = 0;i<vecLines.size();i++)
    {
        Vec4f temp_lines = vecLines[i];
        if((temp_lines[3]-temp_lines[1])*(temp_lines[3]-temp_lines[1])>40)
        {
            if(temp_lines[1]<10)
            {
                line( procesImage,Point(temp_lines[0]-5+100,temp_lines[1]),Point(temp_lines[0]+5+100,temp_lines[1]),Scalar(100,255,0),3,8,0);
                line( procesImage,Point(temp_lines[0]+100,temp_lines[1]-5),Point(temp_lines[0]+100,temp_lines[1]+5),Scalar(100,255,0),3,8,0);
                temp_lines[0] =  temp_lines[0]+100;
                temp_lines[2] = temp_lines[2]+100;
                //line(procesImage,Point(temp_lines[0],temp_lines[1]),Point(temp_lines[2],temp_lines[3]),Scalar(0,0,255),3,8,0);
                std_lines.push_back(temp_lines);
            }
        }
    }

    if(std_lines.size() ==0)
    {
        qDebug()<<"std_size is zero"<<endl;
    }
    else
    {
        float min_lines_x = std_lines.at(0)[0];
        for(size_t i = 0;i<std_lines.size();i++)
        {
            if(std_lines.at(i)[0]<=min_lines_x)
            {
                min_lines_x = std_lines.at(i)[0];
            }
        }
        for(size_t i = 0;i<std_lines.size();i++)
        {
            if(std_lines.at(i)[0]==min_lines_x)
            {
                line(procesImage,Point( std_lines.at(i)[0],std_lines.at(i)[1]),Point(std_lines.at(i)[2],std_lines.at(i)[3]),Scalar(0,0,255),3,8,0);
            }
        }
        qDebug()<<"std_size is: "<<std_lines.size()<<endl;

    }
    //lsd->drawSegments( procesImage,std_lines);
    imshow("ROIimage", procesImage);
    waitKey(30);
}

Mat ImageProcThread::doubleThreshhold(Mat &InputImage, Mat &OutImage, int minValue, int maxValue)
{
    OutImage = InputImage.clone();
    for(int i =0;i<OutImage.rows;i++)
    {
        for(int j  = 0;j<OutImage.cols;j++)
        {
            if(((OutImage.at<uchar>(i,j)>minValue))&&(OutImage.at<uchar>(i,j)<maxValue))
            {
                OutImage.at<uchar>(i,j) = 255;
            }
            else
            {
                OutImage.at<uchar>(i,j) = 0;
            }
        }
    }
    return OutImage;
}

void ImageProcThread::eight_neborhood(Mat &InputImage)
{
    Mat labelImage = Mat::zeros(InputImage.rows,InputImage.cols,CV_8UC1);
    for(int i  = 0;i<InputImage.rows-1;i++)
    {
        for(int j = 0;j<InputImage.cols-1;j++)
        {
            if(InputImage.at<uchar>(i,j) ==255)
            {
                if((InputImage.at<uchar>(i,j+1)==255))
                {
                    Nerborhood_Point_Info[0] = i;
                    Nerborhood_Point_Info[1] = j+1;
                    Nerborhood_Point_Info[2] = label;
                    label = label;
                    for(size_t d = 0;d<N_Point_infos.size();d++)
                    {
                        if(N_Point_infos.at(d)[0] ==  Nerborhood_Point_Info[0]&&N_Point_infos.at(d)[1] ==  Nerborhood_Point_Info[1])
                        {
                            break;
                        }
                        else
                        {
                            N_Point_infos.push_back(Nerborhood_Point_Info);
                            break;
                        }
                    }

                    Nerborhood_Point_Info[0] = i;
                    Nerborhood_Point_Info[1] = j;
                    Nerborhood_Point_Info[2] = label;
                    label = label;
                    for(size_t d = 0;d<N_Point_infos.size();d++)
                    {
                        if(N_Point_infos.at(d)[0] ==  Nerborhood_Point_Info[0]&&N_Point_infos.at(d)[1] ==  Nerborhood_Point_Info[1])
                        {
                            break;
                        }
                        else
                        {
                            N_Point_infos.push_back(Nerborhood_Point_Info);
                            break;
                        }
                    }

                }
                else if((InputImage.at<uchar>(i+1,j+1)==255))
                {
                    Nerborhood_Point_Info[0] = i+1;
                    Nerborhood_Point_Info[1] = j+1;
                    Nerborhood_Point_Info[2] = label;
                    label = label;
                    for(size_t d = 0;d<N_Point_infos.size();d++)
                    {
                        if(N_Point_infos.at(d)[0] ==  Nerborhood_Point_Info[0]&&N_Point_infos.at(d)[1] ==  Nerborhood_Point_Info[1])
                        {
                            break;
                        }
                        else
                        {
                            N_Point_infos.push_back(Nerborhood_Point_Info);
                            break;
                        }
                    }
                    Nerborhood_Point_Info[0] = i;
                    Nerborhood_Point_Info[1] = j;
                    Nerborhood_Point_Info[2] = label;
                    label = label;
                    for(size_t d = 0;d<N_Point_infos.size();d++)
                    {
                        if(N_Point_infos.at(d)[0] ==  Nerborhood_Point_Info[0]&&N_Point_infos.at(d)[1] ==  Nerborhood_Point_Info[1])
                        {
                            break;
                        }
                        else
                        {
                            N_Point_infos.push_back(Nerborhood_Point_Info);
                            break;
                        }
                    }
                }
                else
                {
                    label =  label+1;
                    Nerborhood_Point_Info[0] = i;
                    Nerborhood_Point_Info[1] = j;
                    Nerborhood_Point_Info[2] = label;
                    N_Point_infos.push_back(Nerborhood_Point_Info);
                }
            }
        }
    }
    label = 0;
    for(size_t t = 0;t<N_Point_infos.size();t++)
    {
        if(N_Point_infos.at(t)[2]>50&&N_Point_infos.at(t)[2]<90)
        {
            labelImage.at<uchar>(N_Point_infos.at(t)[0],N_Point_infos.at(t)[1]) =255;
        }
        //qDebug()<<"Label "<<N_Point_infos.at(t)[2]<<endl;
    }
    N_Point_infos.clear();
    imshow("labelImage",labelImage);

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
            QString filepath = MatchImageFilePath.replace('/',"\\");
            for(int i = 19;i<MatchImageNum-2;i++)
            {
                // qDebug()<<"Image:"<<i<<endl;
                QString filename = filepath+QString("\\1 (%1).bmp").arg(i);
                std::string stdmatchfilename = filename.toStdString();
                OriImage = imread(stdmatchfilename);
                detect_pantgraph_info = m_nLineDetect(OriImage);
                m_npantLineDetect(OriImage);
                // Mat proccImage = ImageProcess(OriImage);
                // dispImage = convertMatToQImage(proccImage);
                //send_dispImage(dispImage);
            }
        }
        while(1);
    }
}
