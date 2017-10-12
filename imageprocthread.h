#ifndef IMAGEPROCTHREAD_H
#define IMAGEPROCTHREAD_H
#include <QObject>
#include <QThread>
#include <opencv.hpp>
#include <QFileInfo>
#include <QFile>
#include <QCoreApplication>
#include <QDir>
#include <QImage>
#include <QMessageBox>
#include <QTextStream>
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
    float max_k;
    bool isTempleteImage;
    bool isLoadTempleteImage;
    QString SpendTime;
    Vec4f last_line;
    Vec4f new_line;
    Vec4f left_dx_line;
    Vec4f right_dx_line;
    Vec4f dx_last_line;
    double detect_dx_left_k;
    double detect_dx_left_b;
    Vec2f detect_line_param;
    Point jiechu_point;
    QFile outFile;
    QTextStream datatext;
    Point detect_point;
signals:
    void send_dispImage(QImage);
    void send_sendTime(QString);
    void send_ishaveGj(bool);
    void send_templeteImage(QImage);
    void send_singleLineInfo(int x,int y);
    void send_secondLineInofo(Point,Point);
public slots:
    void accept_MatchFileInfo(QString);
    void accept_isTempleteFile(int);
public:
    double myabsd(double a);
    void load_templeteImage();
    Mat ImageProcess(Mat &oriImage);
    QImage convertMatToQImage(Mat &mat);
    Mat RoiImageProcess(Mat &RoiImage,Rect &RoiRect);
    Point newLineDetect(Mat &grayImage,Mat &tempImage,Rect &RoiRect);
protected:
    void run();
};

#endif // IMAGEPROCTHREAD_H
