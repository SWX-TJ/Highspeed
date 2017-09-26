#ifndef IMAGEPROCTHREAD_H
#define IMAGEPROCTHREAD_H
#include <QObject>
#include <QThread>
#include <opencv.hpp>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDir>
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
    Mat resultImage;
    std::vector<Mat> tempImages;//模板图片
    QString MatchImageFilePath;
    QList<QFileInfo> *MatchImageFileInfo;
    QList<QFileInfo> *TempleteImageFileInfo;
    int TempleteImageNum;
    int MatchImageNum;
    bool isTempleteImage;
public slots:
    void accept_MatchFileInfo(QString);
    void accept_isTempleteFile(int);
public:
    void load_templeteImage();
    Mat ImageProcess(Mat &oriImage);
protected:
     void run();
};

#endif // IMAGEPROCTHREAD_H
