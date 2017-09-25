#ifndef IMAGEPROCTHREAD_H
#define IMAGEPROCTHREAD_H
#include <QObject>
#include <QThread>
#include <Python.h>
class ImageProcThread : public QThread
{
      Q_OBJECT
public:
    ImageProcThread();
protected:
     void run();
};

#endif // IMAGEPROCTHREAD_H
