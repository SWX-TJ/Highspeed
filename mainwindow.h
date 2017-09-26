#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <opencv.hpp>
#include <QFileDialog>
#include <QDir>
#include <QFileInfo>
#include <QList>
#include "handpiecimage.h"
#include "imageprocthread.h"
using namespace cv;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
    void on_SelectFileBtn_clicked();
    void on_StartprocessBtn_clicked();
    void on_ExitBtn_clicked();
    void on_Hand_Templete_triggered();
public slots:
    void accept_return_signal(int);
    void accept_newtempleteFile(int);
signals:
    void send_MatchImageFileInfo(QString);
    void send_istempsignals(int);
public:
    HandPiecImage *m_handpiec;
    ImageProcThread *m_ImageThread;
    QString MatchImageFilePath;
    QList<QFileInfo> *MatchImageFileInfo;
    int MatchImageNum;
    bool isStartBtnCliked;
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
