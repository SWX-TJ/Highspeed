#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    isStartBtnCliked = false;
    m_handpiec = new HandPiecImage();
    m_ImageThread = new ImageProcThread();
    connect(m_handpiec,SIGNAL(send_return_siganl(int)),this,SLOT(accept_return_signal(int)));
    connect(this,SIGNAL(send_MatchImageFileInfo(QString)),m_ImageThread,SLOT(accept_MatchFileInfo(QString)));
    connect(this,SIGNAL(send_istempsignals(int)),m_ImageThread,SLOT(accept_isTempleteFile(int)));
    connect(m_handpiec,SIGNAL(new_templeteFile(int)),this,SLOT(accept_newtempleteFile(int)));
    connect(m_ImageThread,SIGNAL(send_dispImage(QImage)),this,SLOT(accept_dispImage(QImage)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_SelectFileBtn_clicked()
{
  MatchImageFilePath = QFileDialog::getExistingDirectory();
  send_MatchImageFileInfo(MatchImageFilePath);
}

void MainWindow::on_StartprocessBtn_clicked()
{
    isStartBtnCliked = !isStartBtnCliked;
    if(isStartBtnCliked)
    {
        ui->StartprocessBtn->setText("停止");
        m_ImageThread->start();
    }
    else
    {
        ui->StartprocessBtn->setText("开始");
        m_ImageThread->terminate();
    }
}

void MainWindow::on_ExitBtn_clicked()
{
     exit(EXIT_SUCCESS);
}

void MainWindow::on_Hand_Templete_triggered()
{
    m_handpiec->show();
    this->close();
}

void MainWindow::accept_return_signal(int windowNum)
{
    switch(windowNum)
    {
    case 1:
        this->show();
        m_handpiec->close();
        break;
    default:
        break;

    }
}

void MainWindow::accept_newtempleteFile(int tem)
{
    if(tem ==1)
    {
        send_istempsignals(1);
    }
}

void MainWindow::accept_dispImage(QImage resuImage)
{
    ui->OriImage_Disp->setPixmap(QPixmap::fromImage(resuImage));
}

void MainWindow::accept_sendTime(int time)
{

}

void MainWindow::accept_ishaveGj(bool isgj)
{
    ishaveGj = isgj;
    if(ishaveGj)
    {
        ui->gong_is->setText(QString(tr("有")));
    }
    else
    {
         ui->gong_is->setText(QString(tr("无")));
    }
}

void MainWindow::accept_jcpoint(Point)
{

}

void MainWindow::accept_templeteImage(QImage dispTempImage)
{
    ui->TempImage_Disp->setPixmap(QPixmap::fromImage(dispTempImage));
}

void MainWindow::accept_lineNum(int)
{

}
