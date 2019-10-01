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

    connect(this, SIGNAL(samplesReady(mha_wave_t *, float *, unsigned int)), this, SLOT(realtimeDataSlot(mha_wave_t *, float *, unsigned int)),
            Qt::QueuedConnection);
}

void MainWindow::setupRealtimeDataDemo(QCustomPlot *customPlot) {

    // Color palette 28 from:
    // https://digitalsynopsis.com/design/beautiful-color-schemes-combinations-palettes/

    demoName = "Real Time Data Demo";

    // configure layout
    customPlot->plotLayout()->clear(); // clear default axis rect so we can start from scratch

    //-----------------------------------
    // Setup top part of the custom plot:
    //-----------------------------------
    auto *subLayout = new QCPLayoutGrid;

    // Setup left sub axis
    auto *axisTopLeft = new QCPAxisRect(customPlot, false); // false means to not setup default axes
    axisTopLeft->setupFullAxesBox(true);
    axisTopLeft->axis(QCPAxis::atTop)->setLabel("Left Channel"); // add top label to the subplot
    axisTopLeft->axis(QCPAxis::atLeft)->ticker()->setTickCount(4);
    axisTopLeft->axis(QCPAxis::atBottom)->ticker()->setTickCount(5);
    axisTopLeft->axis(QCPAxis::atLeft)->setRange(-1.2, 1.2);
    axisTopLeft->axis(QCPAxis::atBottom)->setRange(0, 63); // todo: accept num_frames as argument
    axisTopLeft->axis(QCPAxis::atLeft)->grid()->setVisible(true);
    axisTopLeft->axis(QCPAxis::atBottom)->grid()->setVisible(true);
    subLayout->addElement(0, 0, axisTopLeft);
    QCPGraph *graphTopLeft = customPlot->addGraph(axisTopLeft->axis(QCPAxis::atBottom),
                                                  axisTopLeft->axis(QCPAxis::atLeft));
    QColor colorTopLeft(0x00, 0x30, 0x49);
    graphTopLeft->setPen(QPen(colorTopLeft));

    // Setup right sub axis
    auto *axisTopRight = new QCPAxisRect(customPlot, false); // false means to not setup default axes
    axisTopRight->setupFullAxesBox(true);
    axisTopRight->axis(QCPAxis::atTop)->setLabel("Right Channel"); // add top label to the subplot
    axisTopRight->axis(QCPAxis::atLeft)->setTickLabels(false); // left axis' tick labels are set by default
    axisTopRight->axis(QCPAxis::atRight)->setTickLabels(true); // right axis' tick labels are not set by default
    axisTopRight->axis(QCPAxis::atRight)->ticker()->setTickCount(4);
    axisTopRight->axis(QCPAxis::atBottom)->ticker()->setTickCount(5);
    axisTopRight->axis(QCPAxis::atRight)->setRange(-1.2, 1.2);
    axisTopRight->axis(QCPAxis::atBottom)->setRange(0, 63); // todo: accept num_frames as argument
    axisTopRight->axis(QCPAxis::atRight)->grid()->setVisible(true);
    axisTopRight->axis(QCPAxis::atBottom)->grid()->setVisible(true);
    subLayout->addElement(0, 1, axisTopRight);
    QCPGraph *graphTopRight = customPlot->addGraph(axisTopRight->axis(QCPAxis::atBottom),
                                                   axisTopRight->axis(QCPAxis::atRight));
    QColor colorTopRight(0xd6, 0x28, 0x28);
    graphTopRight->setPen(QPen(colorTopRight));

    //-------------------------------------
    // Setup bottom part of the custom plot
    //-------------------------------------
    auto *axisBottom = new QCPAxisRect(customPlot);
    axisBottom->setupFullAxesBox(true);
    axisBottom->axis(QCPAxis::atTop)->setLabel("Spectrum"); // add top label to the subplot
    axisBottom->axis(QCPAxis::atRight, 0)->setTickLabels(true);
    axisBottom->axis(QCPAxis::atLeft)->setRange(0, 20);
    axisBottom->axis(QCPAxis::atBottom)->setRange(0, 30); // todo: accept spectrum_size
    QCPGraph *graphBottom = customPlot->addGraph(axisBottom->axis(QCPAxis::atBottom),
                                                 axisBottom->axis(QCPAxis::atRight));

    QColor colorBottom(0xf7, 0x7f, 0x00);
    graphBottom->setLineStyle(QCPGraph::lsLine);
    graphBottom->setPen(QPen(colorBottom.lighter(200)));
    graphBottom->setBrush(QBrush(colorBottom));

    //--------------------------------------------
    // Add top and bottom parts to the custom plot
    //--------------------------------------------
    customPlot->plotLayout()->addElement(0, 0, subLayout); // sub-layout in first row
    customPlot->plotLayout()->addElement(1, 0, axisBottom); // wide axis rect in second row

    // synchronize the left and right margins of the top and bottom axis rects:
    auto *marginGroup = new QCPMarginGroup(customPlot);
    axisTopLeft->setMarginGroup(QCP::msLeft, marginGroup);
    axisTopRight->setMarginGroup(QCP::msRight, marginGroup);
    axisBottom->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);
    // move newly created axes on "axes" layer and grids on "grid" layer:
            foreach(QCPAxisRect *rect, customPlot->axisRects()) {
                    foreach(QCPAxis *axis, rect->axes()) {
                    axis->setLayer("axes");
                    axis->grid()->setLayer("grid");
                }
        }
}

void MainWindow::realtimeDataSlot(mha_wave_t *wave, float *spectrum, unsigned int spectrum_size) {

    QVector<double> buf_channel0(wave->num_frames);
    QVector<double> buf_channel1(wave->num_frames);
    QVector<double> buf_keys(wave->num_frames);
    QVector<double> spec(spectrum_size);
    QVector<double> spec_keys(spectrum_size);

    for (unsigned int frame = 0; frame < wave->num_frames; frame++) {
        // Waveform channels are stored interleaved.
        buf_keys[frame] = frame;
        buf_channel0[frame] = wave->buf[wave->num_channels * frame + 0];
        buf_channel1[frame] = wave->buf[wave->num_channels * frame + 1];
    }

    for (unsigned int i = 0; i < spectrum_size; i++) {
        spec_keys[i] = i;
        spec[i] = spectrum[i];
    }

    ui->customPlot->graph(0)->setData(buf_keys, buf_channel0);
    ui->customPlot->graph(1)->setData(buf_keys, buf_channel1);
    ui->customPlot->graph(2)->setData(spec_keys, spec);

    ui->customPlot->replot();
}

MainWindow::~MainWindow() {
    delete ui;
}
