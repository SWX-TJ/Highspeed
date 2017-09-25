#include "handpiecimage.h"
#include "ui_handpiecimage.h"
HandPiecImage::HandPiecImage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HandPiecImage)
{
    ui->setupUi(this);
}

HandPiecImage::~HandPiecImage()
{
    delete ui;
}

void HandPiecImage::on_ReturnBtn_clicked()
{
     send_return_siganl(1);
}
