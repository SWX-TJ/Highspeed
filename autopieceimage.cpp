#include "autopieceimage.h"
#include "ui_autopieceimage.h"
#include <QDebug>
autoPieceImage::autoPieceImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::autoPieceImage)
{
    ui->setupUi(this);
}

autoPieceImage::~autoPieceImage()
{
    delete ui;
}

void autoPieceImage::on_openFileBtn_clicked()
{
    File_INFO = QFileDialog::getOpenFileName(this,
                                             tr("Open Image"),tr("/"), tr("Image Files (*.png *.jpg *.bmp)"));
    File_info = QFileInfo(File_INFO);
    File_path = File_info.filePath().replace('/',"\\");
    std::string TempleteImage = File_path.toStdString();
    OriImage = imread(TempleteImage,0);
    DispImage =  convertMatToQImage(OriImage);
    ui->dispImage_label->setPixmap(QPixmap::fromImage(DispImage));
}

void autoPieceImage::on_ExtiBtn_clicked()
{
    send_return_siganl(2);
}

QImage autoPieceImage::convertMatToQImage(Mat &mat)
{
    QImage img;
    int nChannel=mat.channels();
    qDebug()<<"channel:"<<nChannel<<endl;
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

void autoPieceImage::on_autoTmmpletBtn_clicked()
{
     //Mat tranimage = TraversalPixels(OriImage,greyFilter);
     Mat image=Check(OriImage);
     imshow("Pantographss",image);
}
