#ifndef HANDPIECIMAGE_H
#define HANDPIECIMAGE_H
#include <QMainWindow>
#include <Python.h>
namespace Ui {
class HandPiecImage;
}
class HandPiecImage : public QMainWindow
{
    Q_OBJECT
public:
    explicit HandPiecImage(QWidget *parent = 0);
    ~HandPiecImage();
private slots:
    void on_ReturnBtn_clicked();
signals:
    void send_return_siganl(int);
private:
    Ui::HandPiecImage *ui;
};

#endif // HANDPIECIMAGE_H
