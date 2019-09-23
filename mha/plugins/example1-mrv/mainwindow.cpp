#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QMetaEnum>

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

    connect(this, SIGNAL(samplesReady(mha_wave_t *)), this, SLOT(realtimeDataSlot(mha_wave_t *)),
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
    customPlot->addGraph(); // red line
    customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));

    customPlot->xAxis->setRange(0, 64); // todo: accept num_frames as argument
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(-1.2, 1.2);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
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
