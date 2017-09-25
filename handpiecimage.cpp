#include "handpiecimage.h"
#include "ui_handpiecimage.h"
#include<QDebug>
HandPiecImage::HandPiecImage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HandPiecImage)
{
    ui->setupUi(this);
    isLeftPoint = false;
    isRightPoint = false;
    m_makeImage = new MakeTempleteThread();
    connect(this,SIGNAL(send_LeftImageInfo(QPoint,QImage)),m_makeImage,SLOT(accept_leftargImage(QPoint,QImage)));
    connect(this,SIGNAL(send_RightImageInfo(QPoint,QImage)),m_makeImage,SLOT(accept_rightargImage(QPoint,QImage)));
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
        }
        return mat;
}

void HandPiecImage::mousePressEvent(QMouseEvent *event)
{
    if(Qt::LeftButton == event->button())
    {
        isLeftPoint = true;
    }
    if(Qt::RightButton == event->button())
    {
        isRightPoint = true;
    }
}

void HandPiecImage::mouseMoveEvent(QMouseEvent *event)
{
    if(isLeftPoint)
    {
        leftPoint.setX(event->x());
        leftPoint.setY(event->y());
        send_LeftImageInfo(leftPoint,DispImage);
    }
    if(isRightPoint)
    {
        rightPoint.setX(event->x());
        rightPoint.setY(event->y());
        send_RightImageInfo(rightPoint,DispImage);
    }
}

void HandPiecImage::mouseReleaseEvent(QMouseEvent *event)
{
    if(Qt::LeftButton == event->button())
    {
        isLeftPoint = false;
        // DispImage = tempDispImage;

    }
    if(Qt::RightButton == event->button())
    {
        isRightPoint = false;
        //DispImage = tempDispImage;
    }
}

void HandPiecImage::on_ReturnBtn_clicked()
{
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
