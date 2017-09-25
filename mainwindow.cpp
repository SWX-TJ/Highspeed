#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

}

void MainWindow::on_ExitBtn_clicked()
{
    exit(EXIT_SUCCESS);
}
