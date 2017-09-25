#ifndef MAKETEMPLETETHREAD_H
#define MAKETEMPLETETHREAD_H
#include <QObject>
#include <QThread>
#include <QPoint>
#include <opencv.hpp>
#include <QImage>
using namespace cv;
class MakeTempleteThread : public QThread
{
    Q_OBJECT
public:
    MakeTempleteThread();
public:
    Mat OriImage;
    QPoint LeftPoint;
    QPoint RightPoint;
    bool isdrawleft;
    bool isdrawright;
public:
     QImage convertMatToQImage(Mat &mat);
      Mat   convertQImageToMat(QImage &image);
signals:
    void send_drawImage(QImage);
public slots:
    void accept_leftargImage(QPoint,QImage);
    void accept_rightargImage(QPoint,QImage);

    //Mat func(int left_x,int left_y,int right_x,int right_y,Mat &OriImage);
protected:
    void run();
};

#endif // MAKETEMPLETETHREAD_H
