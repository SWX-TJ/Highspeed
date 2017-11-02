#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDataStream>
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile useFile("use.dat");
    useFile.open(QIODevice::ReadOnly);
    QDataStream useDatFile(&useFile);
    int usetime;
    useDatFile>>usetime;
    if(usetime ==0)
    {
      useFile.close();
    //  exit(EXIT_SUCCESS);
    }
    else
    {
        QFile usFile("use.dat");
        usFile.open(QIODevice::WriteOnly|QIODevice::Truncate);
        QDataStream usDatFile(&usFile);
        usDatFile<<(usetime-1);
        usFile.close();
    }
//    int usetime = 10;
//            QFile usFile("use.dat");
//            usFile.open(QIODevice::WriteOnly|QIODevice::Truncate);
//            QDataStream usDatFile(&usFile);
//            usDatFile<<usetime;
//            usFile.close();
    MainWindow w;
    w.show();
    return a.exec();
}
