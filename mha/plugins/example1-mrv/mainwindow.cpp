#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QMetaEnum>

#define BUF_SIZE 64

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setGeometry(400, 250, 542, 390);

    // setup demo
    setupRealtimeDataDemo(ui->customPlot);
    setWindowTitle("QCustomPlot: " + demoName);
    statusBar()->clearMessage();
    ui->customPlot->replot();

    connect(this, SIGNAL(samplesReady(float * , int)), this, SLOT(realtimeDataSlot(float * , int)),
            Qt::QueuedConnection);
}

void MainWindow::setupRealtimeDataDemo(QCustomPlot *customPlot) {
    demoName = "Real Time Data Demo";

    // include this section to fully disable antialiasing for higher performance:
    /*
    customPlot->setNotAntialiasedElements(QCP::aeAll);
    QFont font;
    font.setStyleStrategy(QFont::NoAntialias);
    customPlot->xAxis->setTickLabelFont(font);
    customPlot->yAxis->setTickLabelFont(font);
    customPlot->legend->setFont(font);
    */
    customPlot->addGraph(); // blue line
    customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));

    customPlot->xAxis->setRange(0, 64);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(-1.2, 1.2);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
}

void MainWindow::realtimeDataSlot(float *buf, int size) {
    QVector<double> qbuf;
    qbuf.reserve(size); // warning: size_t->int cast
//    qbuf.insert(0, 64, 0.5); // at index 0, insert 64 values, all of which are equal to 0.5
    std::copy(buf, buf + size, std::back_inserter(qbuf));

    QVector<double> keys(64);
    for (int i = 0; i < BUF_SIZE; i++) {
        keys[i] = i;
    }
    ui->customPlot->graph(0)->setData(keys, qbuf, true);

    ui->customPlot->replot();
}

MainWindow::~MainWindow() {
    delete ui;
}
