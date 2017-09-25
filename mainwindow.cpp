#include "mainwindow.h"
#include "ui_mainwindow.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_handpiec = new HandPiecImage();
    m_ImageThread = new ImageProcThread();
    connect(m_handpiec,SIGNAL(send_return_siganl(int)),this,SLOT(accept_return_signal(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_SelectFileBtn_clicked()
{

}

void MainWindow::on_StartprocessBtn_clicked()
{
 m_ImageThread->start();
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
