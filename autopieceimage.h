#ifndef AUTOPIECEIMAGE_H
#define AUTOPIECEIMAGE_H
#include <QDialog>
#include <opencv.hpp>
#include <iostream>
#include <vector>
#include <list>
#include <QFileDialog>
#include <QFileInfo>
#include <QFile>
#include <autotemplete.h>
using namespace std;
using namespace cv;
using namespace autotemepfunc;
namespace Ui {
class autoPieceImage;
}

class autoPieceImage : public QDialog
{
    Q_OBJECT

public:
    explicit autoPieceImage(QWidget *parent = 0);
    ~autoPieceImage();
signals:
      void send_return_siganl(int);
private slots:
    void on_openFileBtn_clicked();
    void on_ExtiBtn_clicked();
    void on_autoTmmpletBtn_clicked();

public:
    //打开文件
    QString   File_INFO;
    QString   File_path;
    QString   File_name;
    QFileInfo File_info;
    Mat OriImage;
    QImage DispImage;
public:
 QImage convertMatToQImage(Mat &mat);
private:
    Ui::autoPieceImage *ui;
};

#endif // AUTOPIECEIMAGE_H
