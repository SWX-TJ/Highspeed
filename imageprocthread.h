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
#include <opencv2/imgproc.hpp>
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
    QImage dispImage;
    Mat resuImage;
    double minValue,maxValue;
    Point minLocation;
    Point maxLocation;
    Point matchLocation;
    std::vector<Mat> tempImages;//模板图片
    std::vector<Rect> MatchRects;
    std::vector<float>line_k;
    QString MatchImageFilePath;
    QList<QFileInfo> *MatchImageFileInfo;
    QList<QFileInfo> *TempleteImageFileInfo;
    int TempleteImageNum;
    int MatchImageNum;
    int minMatchArea;
    bool isTempleteImage;
    bool isLoadTempleteImage;
    QString SpendTime;
signals:
    void send_dispImage(QImage);
    void send_sendTime(QString);
    void send_ishaveGj(bool);
    void send_templeteImage(QImage);
    void send_singleLineInfo(Point);
    void send_secondLineInofo(Point,Point);
public slots:
    void accept_MatchFileInfo(QString);
    void accept_isTempleteFile(int);
public:
    void load_templeteImage();
    Mat ImageProcess(Mat &oriImage);
    QImage convertMatToQImage(Mat &mat);
    Mat RoiImageProcess(Mat &RoiImage,Rect &RoiRect);
    Mat newLineDetect(Mat &grayImage,Mat &tempImage,Rect &RoiRect);
protected:
    void run();
};

#endif // IMAGEPROCTHREAD_H
