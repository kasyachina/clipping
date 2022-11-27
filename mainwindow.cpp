#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QGridLayout>
#include <fstream>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setStyleSheet("background-color: white;");
    QGridLayout *g = new QGridLayout(centralWidget());
    area = new PlotArea();
    g -> addWidget(area, 0, 0, 4, 4);
    centralWidget()->setLayout(g);
    QString filepath = "/home/kasinski_nikita/Documents/input.txt";
    std::ifstream fin(filepath.toStdString());
    setMinimumSize({600, 600});
    setWindowTitle("Отсечения");
}

MainWindow::~MainWindow()
{
    delete ui;
}

