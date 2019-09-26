#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QMetaEnum>

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
//    setGeometry(400, 250, 542, 390); // original
    setGeometry(400, 250, 1000, 600);

    // setup demo
    setupRealtimeDataDemo(ui->customPlot);
    setWindowTitle("QCustomPlot: " + demoName);
    statusBar()->clearMessage();
    ui->customPlot->replot();

    connect(this, SIGNAL(samplesReady(mha_wave_t * )), this, SLOT(realtimeDataSlot(mha_wave_t * )),
            Qt::QueuedConnection);
}

void MainWindow::setupRealtimeDataDemo(QCustomPlot *customPlot) {
    demoName = "Real Time Data Demo";

    // configure layout
    customPlot->plotLayout()->clear(); // clear default axis rect so we can start from scratch

    // Setup top part of the custom plot:
    auto *subLayout = new QCPLayoutGrid;

    // Setup left sub axis
    auto *subRectLeft = new QCPAxisRect(customPlot, false); // false means to not setup default axes
    subRectLeft->setupFullAxesBox(true);
    subRectLeft->axis(QCPAxis::atTop)->setLabel("Left Channel"); // add top label to the subplot
    subRectLeft->axis(QCPAxis::atLeft)->ticker()->setTickCount(4);
    subRectLeft->axis(QCPAxis::atBottom)->ticker()->setTickCount(5);
    subRectLeft->axis(QCPAxis::atLeft)->setRange(-1.2, 1.2);
    subRectLeft->axis(QCPAxis::atBottom)->setRange(0, 63); // todo: accept num_frames as argument
    subRectLeft->axis(QCPAxis::atLeft)->grid()->setVisible(true);
    subRectLeft->axis(QCPAxis::atBottom)->grid()->setVisible(true);
    subLayout->addElement(0, 0, subRectLeft);

    // Setup right sub axis
    auto *subRectRight = new QCPAxisRect(customPlot, false); // false means to not setup default axes
    subRectRight->setupFullAxesBox(true);
    subRectRight->axis(QCPAxis::atTop)->setLabel("Right Channel"); // add top label to the subplot
    subRectRight->axis(QCPAxis::atLeft)->setTickLabels(false); // left axis' tick labels are set by default
    subRectRight->axis(QCPAxis::atRight)->setTickLabels(true); // right axis' tick labels are not set by default
    subRectRight->axis(QCPAxis::atRight)->ticker()->setTickCount(4);
    subRectRight->axis(QCPAxis::atBottom)->ticker()->setTickCount(5);
    subRectRight->axis(QCPAxis::atRight)->setRange(-1.2, 1.2);
    subRectRight->axis(QCPAxis::atBottom)->setRange(0, 63); // todo: accept num_frames as argument
    subRectRight->axis(QCPAxis::atRight)->grid()->setVisible(true);
    subRectRight->axis(QCPAxis::atBottom)->grid()->setVisible(true);
    subLayout->addElement(0, 1, subRectRight);

    // Setup bottom part of the custom plot
    auto *wideAxisRect = new QCPAxisRect(customPlot);
    wideAxisRect->setupFullAxesBox(true);
    wideAxisRect->axis(QCPAxis::atTop)->setLabel("Spectrum"); // add top label to the subplot
    wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabels(true);

    // Add top and bottom parts to the custom plot
    customPlot->plotLayout()->addElement(0, 0, subLayout); // sub-layout in first row
    customPlot->plotLayout()->addElement(1, 0, wideAxisRect); // wide axis rect in second row

    // synchronize the left and right margins of the top and bottom axis rects:
    auto *marginGroup = new QCPMarginGroup(customPlot);
    subRectLeft->setMarginGroup(QCP::msLeft, marginGroup);
    subRectRight->setMarginGroup(QCP::msRight, marginGroup);
    wideAxisRect->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);
    // move newly created axes on "axes" layer and grids on "grid" layer:
            foreach(QCPAxisRect *rect, customPlot->axisRects()) {
                    foreach(QCPAxis *axis, rect->axes()) {
                    axis->setLayer("axes");
                    axis->grid()->setLayer("grid");
                }
        }

    QCPGraph *subGraphLeft = customPlot->addGraph(subRectLeft->axis(QCPAxis::atBottom),
                                                  subRectLeft->axis(QCPAxis::atLeft));
    subGraphLeft->setPen(QPen(QColor(40, 110, 255))); // blue

    QCPGraph *subGraphRight = customPlot->addGraph(subRectRight->axis(QCPAxis::atBottom),
                                                   subRectRight->axis(QCPAxis::atRight));
    subGraphRight->setPen(QPen(QColor(255, 110, 40))); // red
}

void MainWindow::realtimeDataSlot(mha_wave_t *wave) {

    QVector<double> buf_channel0(wave->num_frames);
    QVector<double> buf_channel1(wave->num_frames);
    QVector<double> keys(wave->num_frames);

    for (unsigned int frame = 0; frame < wave->num_frames; frame++) {
        // Waveform channels are stored interleaved.
        buf_channel0[frame] = wave->buf[wave->num_channels * frame + 0];
        buf_channel1[frame] = wave->buf[wave->num_channels * frame + 1];
        keys[frame] = frame;
    }

    ui->customPlot->graph(0)->setData(keys, buf_channel0);
    ui->customPlot->graph(1)->setData(keys, buf_channel1);

    ui->customPlot->replot();
}

MainWindow::~MainWindow() {
    delete ui;
}
