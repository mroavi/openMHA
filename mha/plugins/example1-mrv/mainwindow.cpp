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
    setWindowTitle("openMHA + Julia: Noise reduction using Bayesian inference");
    statusBar()->clearMessage();
    ui->customPlot->replot();

    connect(this,
            SIGNAL(samplesReady(mha_wave_t *, float *, float *, unsigned int)),
            this,
            SLOT(realtimeDataSlot(mha_wave_t *, float *, float *, unsigned int)),
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
    auto *subLayoutTop = new QCPLayoutGrid;

    // Setup left sub axis
    auto *axisTopLeft = new QCPAxisRect(customPlot, false); // false means to not setup default axes
    axisTopLeft->setupFullAxesBox(true);
    axisTopLeft->axis(QCPAxis::atTop)->setLabel("Input signal"); // add top label to the subplot
    axisTopLeft->axis(QCPAxis::atLeft)->ticker()->setTickCount(4);
    axisTopLeft->axis(QCPAxis::atBottom)->ticker()->setTickCount(4);
    axisTopLeft->axis(QCPAxis::atLeft)->setRange(-1.2, 1.2);
    axisTopLeft->axis(QCPAxis::atBottom)->setRange(0, 63.0/44.100); // todo: accept num_frames and sampling freq as argument
    axisTopLeft->axis(QCPAxis::atLeft)->grid()->setVisible(true);
    axisTopLeft->axis(QCPAxis::atBottom)->grid()->setVisible(true);
    axisTopLeft->axis(QCPAxis::atLeft)->setLabel("Amplitude");
    axisTopLeft->axis(QCPAxis::atBottom)->setLabel("Time (ms)");
    subLayoutTop->addElement(0, 0, axisTopLeft);
    QCPGraph *graphTopLeft = customPlot->addGraph(axisTopLeft->axis(QCPAxis::atBottom),
                                                  axisTopLeft->axis(QCPAxis::atLeft));
    QColor colorTopLeft(0x00, 0x30, 0x49);
    graphTopLeft->setPen(QPen(colorTopLeft));

    // Setup right sub axis
    auto *axisTopRight = new QCPAxisRect(customPlot, false); // false means to not setup default axes
    axisTopRight->setupFullAxesBox(true);
    axisTopRight->axis(QCPAxis::atTop)->setLabel("Output signal"); // add top label to the subplot
    axisTopRight->axis(QCPAxis::atLeft)->setTickLabels(false); // left axis' tick labels are set by default
    axisTopRight->axis(QCPAxis::atRight)->setTickLabels(true); // right axis' tick labels are not set by default
    axisTopRight->axis(QCPAxis::atRight)->ticker()->setTickCount(4);
    axisTopRight->axis(QCPAxis::atBottom)->ticker()->setTickCount(4);
    axisTopRight->axis(QCPAxis::atRight)->setRange(-1.2, 1.2);
    axisTopRight->axis(QCPAxis::atBottom)->setRange(0, 63.0/44.100); // todo: accept num_frames and sampling freq as argument
    axisTopRight->axis(QCPAxis::atRight)->grid()->setVisible(true);
    axisTopRight->axis(QCPAxis::atBottom)->grid()->setVisible(true);
    axisTopRight->axis(QCPAxis::atLeft)->setLabel("Amplitude");
    axisTopRight->axis(QCPAxis::atBottom)->setLabel("Time (ms)");
    subLayoutTop->addElement(0, 1, axisTopRight);
    QCPGraph *graphTopRight = customPlot->addGraph(axisTopRight->axis(QCPAxis::atBottom),
                                                   axisTopRight->axis(QCPAxis::atRight));
    QColor colorTopRight(0xd6, 0x28, 0x28);
    graphTopRight->setPen(QPen(colorTopRight));

    //-----------------------------------
    // Setup bottom part of the custom plot:
    //-----------------------------------
    auto *subLayoutBottom = new QCPLayoutGrid;

    // Setup left sub axis
    auto *axisBottomLeft = new QCPAxisRect(customPlot);
    axisBottomLeft->setupFullAxesBox(true);
    axisBottomLeft->axis(QCPAxis::atTop)->setLabel("Spectrum of input signal"); // add top label to the subplot
    axisBottomLeft->axis(QCPAxis::atRight, 0)->setTickLabels(true);
    axisBottomLeft->axis(QCPAxis::atLeft)->setRange(0, 30);
    axisBottomLeft->axis(QCPAxis::atBottom)->setRange(0, 10); // todo: accept spectrum_size
    axisBottomLeft->axis(QCPAxis::atLeft)->setLabel("Power (a.u.)");
    axisBottomLeft->axis(QCPAxis::atBottom)->setLabel("Frequency (kHz)");
    axisBottomLeft->axis(QCPAxis::atRight)->grid()->setVisible(true);
    axisBottomLeft->axis(QCPAxis::atBottom)->grid()->setVisible(true);
    subLayoutBottom->addElement(0, 0, axisBottomLeft);
    QCPGraph *graphBottomLeft = customPlot->addGraph(axisBottomLeft->axis(QCPAxis::atBottom),
                                                  axisBottomLeft->axis(QCPAxis::atLeft));
    QColor colorBottomLeft(0x00, 0x30, 0x49);
    graphBottomLeft->setLineStyle(QCPGraph::lsLine);
    graphBottomLeft->setPen(QPen(colorBottomLeft.lighter(200)));
    graphBottomLeft->setBrush(QBrush(colorBottomLeft));

    // Setup right sub axis
    auto *axisBottomRight = new QCPAxisRect(customPlot, false); // false means to not setup default axes

    axisBottomRight->setupFullAxesBox(true);
    axisBottomRight->axis(QCPAxis::atTop)->setLabel("Spectrum of output signal"); // add top label to the subplot
    axisBottomRight->axis(QCPAxis::atRight, 0)->setTickLabels(true);
    axisBottomRight->axis(QCPAxis::atLeft)->setRange(0, 30);
    axisBottomRight->axis(QCPAxis::atBottom)->setRange(0, 10); // todo: accept spectrum_size
    axisBottomRight->axis(QCPAxis::atLeft)->setLabel("Power (a.u.)");
    axisBottomRight->axis(QCPAxis::atBottom)->setLabel("Frequency (kHz)");
    axisBottomRight->axis(QCPAxis::atRight)->grid()->setVisible(true);
    axisBottomRight->axis(QCPAxis::atBottom)->grid()->setVisible(true);
    subLayoutBottom->addElement(0, 1, axisBottomRight);
    QCPGraph *graphBottomRight = customPlot->addGraph(axisBottomRight->axis(QCPAxis::atBottom),
                                                   axisBottomRight->axis(QCPAxis::atRight));
    QColor colorBottomRight(0xd6, 0x28, 0x28);
    graphBottomRight->setLineStyle(QCPGraph::lsLine);
    graphBottomRight->setPen(QPen(colorBottomRight.lighter(200)));
    graphBottomRight->setBrush(QBrush(colorBottomRight));

    //--------------------------------------------
    // Add top and bottom parts to the custom plot
    //--------------------------------------------
    customPlot->plotLayout()->addElement(0, 0, subLayoutTop); // sub-layout in first row
    customPlot->plotLayout()->addElement(1, 0, subLayoutBottom); // wide axis rect in second row

    // synchronize the left and right margins of the top and bottom axis rects:
    auto *marginGroup = new QCPMarginGroup(customPlot);
    axisTopLeft->setMarginGroup(QCP::msLeft, marginGroup);
    axisTopRight->setMarginGroup(QCP::msRight, marginGroup);
    axisBottomLeft->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);
    // move newly created axes on "axes" layer and grids on "grid" layer:
            foreach(QCPAxisRect *rect, customPlot->axisRects()) {
                    foreach(QCPAxis *axis, rect->axes()) {
                    axis->setLayer("axes");
                    axis->grid()->setLayer("grid");
                }
        }
}

void MainWindow::realtimeDataSlot(mha_wave_t *wave, float *input_spectrum, float *output_spectrum, unsigned int spectrum_size) {

    QVector<double> buf_channel0(wave->num_frames);
    QVector<double> buf_channel1(wave->num_frames);
    QVector<double> buf_keys(wave->num_frames);
    QVector<double> input_spec(spectrum_size);
    QVector<double> output_spec(spectrum_size);
    QVector<double> spec_keys(spectrum_size);

    for (unsigned int frame = 0; frame < wave->num_frames; frame++) {
        // Waveform channels are stored interleaved.
        buf_keys[frame] = frame/44.100; // todo: accept sampling freq as parameter
        buf_channel0[frame] = wave->buf[wave->num_channels * frame + 0];
        buf_channel1[frame] = wave->buf[wave->num_channels * frame + 1];
    }

    for (unsigned int i = 0; i < spectrum_size; i++) {
        spec_keys[i] = i/(10.0/16.0);
        input_spec[i] = input_spectrum[i];
        output_spec[i] = output_spectrum[i];
    }

    ui->customPlot->graph(0)->setData(buf_keys, buf_channel0);
    ui->customPlot->graph(1)->setData(buf_keys, buf_channel1);
    ui->customPlot->graph(2)->setData(spec_keys, input_spec);
    ui->customPlot->graph(3)->setData(spec_keys, output_spec);

    ui->customPlot->replot();
}

MainWindow::~MainWindow() {
    delete ui;
}
