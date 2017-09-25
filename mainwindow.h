#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "handpiecimage.h"
#include "imageprocthread.h"
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
public:
    HandPiecImage *m_handpiec;
    ImageProcThread *m_ImageThread;
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
