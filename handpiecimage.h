#ifndef HANDPIECIMAGE_H
#define HANDPIECIMAGE_H
#include <QMainWindow>
#include <QMouseEvent>
#include <QPoint>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <QImage>
#include <opencv.hpp>
#include <iostream>
#include <maketempletethread.h>
using namespace cv;
namespace Ui {
class HandPiecImage;
}
class HandPiecImage : public QMainWindow
{
    Q_OBJECT
public:
    explicit HandPiecImage(QWidget *parent = 0);
    ~HandPiecImage();
public:
    QPoint leftPoint;
    QPoint rightPoint;
    Mat    OriImage;
    QImage DispImage;
    QImage tempDispImage;
    QString   File_INFO;
    QString   File_path;
    QString   File_name;
    QFileInfo File_info;
public:
     QImage convertMatToQImage(Mat &mat);
     Mat   convertQImageToMat(QImage &image);
     MakeTempleteThread *m_makeImage;
public:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private slots:
    void on_ReturnBtn_clicked();
    void on_OpenFlie_clicked();
public slots:
    void accept_QImage(QImage);
signals:
    void send_LeftImageInfo(QPoint,QImage);
    void send_RightImageInfo(QPoint,QImage);
    void send_return_siganl(int);
public:
    bool isLeftPoint;
    bool isRightPoint;
private:
    Ui::HandPiecImage *ui;
};

#endif // HANDPIECIMAGE_H
