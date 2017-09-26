#include "handpiecimage.h"
#include "ui_handpiecimage.h"
#include<QDebug>
#include <iostream>
HandPiecImage::HandPiecImage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HandPiecImage)
{
    ui->setupUi(this);
    isLeftPoint = false;
    isRightPoint = false;
    isLeftPointFinish = false;
    isRightPointFinish = false;
     TempleteImageNum= 0;
    m_makeImage = new MakeTempleteThread();
    connect(this,SIGNAL(send_LeftImageInfo(QPoint,QString)),m_makeImage,SLOT(accept_leftargImage(QPoint,QString)));
    connect(this,SIGNAL(send_RightImageInfo(QPoint,QString)),m_makeImage,SLOT(accept_rightargImage(QPoint,QString)));
    connect(this,SIGNAL(send_LeftAndRightImageInfo(QPoint,QPoint,QString)),m_makeImage,SLOT(accept_leftAndRightImage(QPoint,QPoint,QString)));
    connect(m_makeImage,SIGNAL(send_drawImage(QImage)),this,SLOT(accept_QImage(QImage)));
    m_makeImage->start();
}

HandPiecImage::~HandPiecImage()
{
    delete ui;
}

QImage HandPiecImage::convertMatToQImage(Mat &mat)
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

Mat HandPiecImage::convertQImageToMat(QImage &image)
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
    default:
        break;
    }
    return mat;
}

void HandPiecImage::mousePressEvent(QMouseEvent *event)
{
    if(Qt::LeftButton == event->button())
    {
        isLeftPoint = true;
        isLeftPointFinish = false;
    }
    if(Qt::RightButton == event->button())
    {
        isRightPoint = true;
        isRightPointFinish = false;
    }
}

void HandPiecImage::mouseMoveEvent(QMouseEvent *event)
{
    if(isLeftPoint)
    {
        leftPoint.setX(event->x());
        leftPoint.setY(event->y());
        QString temp_leftPoint_x,temp_leftPoint_y;
        ui->right_aix_x->setText(temp_leftPoint_x.setNum(leftPoint.x()));
        ui->right_aix_y->setText(temp_leftPoint_y.setNum(leftPoint.y()-40));
        if(isRightPointFinish)
        {
        send_LeftAndRightImageInfo(leftPoint,rightFnishPoint,File_path);
        }
        else
        {
            send_LeftImageInfo(leftPoint, File_path);
        }
    }
    if(isRightPoint)
    {
        rightPoint.setX(event->x());
        rightPoint.setY(event->y());
        QString temp_rightPoint_x,temp_rightPoint_y;
        ui->left_aix_x->setText(temp_rightPoint_x.setNum(rightPoint.x()));
        ui->left_aix_y->setText(temp_rightPoint_y.setNum(rightPoint.y()-40));
        if(isLeftPointFinish)
        {
         send_LeftAndRightImageInfo(leftFnishPoint,rightPoint,File_path);
        }
        else
        {
        send_RightImageInfo(rightPoint, File_path);
        }
    }
}

void HandPiecImage::mouseReleaseEvent(QMouseEvent *event)
{
    if(Qt::LeftButton == event->button())
    {
        isLeftPoint = false;
        isLeftPointFinish = true;
        leftFnishPoint.setX(event->x());
        leftFnishPoint.setY(event->y());
    }
    if(Qt::RightButton == event->button())
    {
        isRightPoint = false;
        isRightPointFinish = true;
        rightFnishPoint.setX(event->x());
        rightFnishPoint.setY(event->y());
    }
}

void HandPiecImage::on_ReturnBtn_clicked()
{
    m_makeImage->quit();
    send_return_siganl(1);
}

void HandPiecImage::on_OpenFlie_clicked()
{
    File_INFO = QFileDialog::getOpenFileName(this,
                                             tr("Open Image"),tr("/"), tr("Image Files (*.png *.jpg *.bmp)"));
    File_info = QFileInfo(File_INFO);
    File_path = File_info.filePath().replace('/',"\\");
    std::string TempleteImage = File_path.toStdString();
    OriImage = imread(TempleteImage);
    DispImage =  convertMatToQImage(OriImage);
    ui->OriImage->setPixmap(QPixmap::fromImage(DispImage));
}

void HandPiecImage::accept_QImage(QImage oriImage)
{
    QImage  stempDispImage = oriImage;
    ui->OriImage->setPixmap(QPixmap::fromImage(oriImage));
}

void HandPiecImage::on_SaveFileBtn_clicked()
{
    cv::Point stdleftPoint,stdrightPoint;
    stdleftPoint.x =  rightFnishPoint.x();
    stdleftPoint.y =  rightFnishPoint.y()-40;
    stdrightPoint.x = leftFnishPoint.x();
    stdrightPoint.y = leftFnishPoint.y()-40;
    OriImage(Rect(stdleftPoint.x,stdleftPoint.y,(stdrightPoint.x-stdleftPoint.x),(stdrightPoint.y-stdleftPoint.y))).copyTo(TempleteImage);
    QString tempfilePath = QCoreApplication::applicationDirPath().replace('/',"\\");
    QString tempfileName = QString("\\templete\\temImage (%1).bmp").arg(TempleteImageNum);
    std::string savetempfile = (tempfilePath+tempfileName).toStdString();
    imwrite(savetempfile,TempleteImage);
    TempleteImageNum++;
    tempDispImage = convertMatToQImage(TempleteImage);
    pixmap =  QPixmap::fromImage(tempDispImage);
    ui->TempleteImage->setPixmap(pixmap);
    new_templeteFile(1);
}
