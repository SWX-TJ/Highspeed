#ifndef IMAGEPROCTHREAD_H
#define IMAGEPROCTHREAD_H
#include <QObject>
#include <QThread>
#include <opencv.hpp>
using namespace cv;
class ImageProcThread : public QThread
{
      Q_OBJECT
public:
    ImageProcThread();
public:
    Mat OriImage;
    Mat TempImage;
protected:
     void run();
};

#endif // IMAGEPROCTHREAD_H
