#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "plotarea.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    PlotArea *area;
    QString segmentsPath = "/home/kasinski_nikita/Documents/segments.txt";
    QString polygonPath = "/home/kasinski_nikita/Documents/poly.txt";
    void ReadSegments();
    void ReadPoly();
};
#endif // MAINWINDOW_H
