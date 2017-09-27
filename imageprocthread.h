#ifndef IMAGEPROCTHREAD_H
#define IMAGEPROCTHREAD_H
#include <QObject>
#include <QThread>
#include <opencv.hpp>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDir>
#include <QImage>
#include <QMessageBox>
using namespace cv;
class ImageProcThread : public QThread
{
      Q_OBJECT
public:
    ImageProcThread();
public:
    Mat OriImage;
    Mat TempImage;
    QImage dispTempImage;
    Mat resultImage;
    QImage dispImage;
    Mat resuImage;
    double minValue,maxValue;
    Point minLocation;
    Point maxLocation;
    Point matchLocation;
    std::vector<Mat> tempImages;//模板图片
    std::vector<Rect> MatchRects;
    QString MatchImageFilePath;
    QList<QFileInfo> *MatchImageFileInfo;
    QList<QFileInfo> *TempleteImageFileInfo;
    int TempleteImageNum;
    int MatchImageNum;
     int minMatchArea;
    bool isTempleteImage;
    bool isLoadTempleteImage;
signals:
    void send_dispImage(QImage);
    void send_sendTime(int);
    void send_ishaveGj(bool);
    void send_jcpoint(Point);
    void send_templeteImage(QImage);
    void send_lineNum(int);
public slots:
    void accept_MatchFileInfo(QString);
    void accept_isTempleteFile(int);
public:
    void load_templeteImage();
    Mat ImageProcess(Mat &oriImage);
    QImage convertMatToQImage(Mat &mat);
protected:
     void run();
};

#endif // IMAGEPROCTHREAD_H
