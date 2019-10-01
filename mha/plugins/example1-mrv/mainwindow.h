#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "qcustomplot.h" // the header file of QCustomPlot. Don't forget to add it to your project, if you use an IDE, so it gets compiled.
#include "mha.hh"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  void setupRealtimeDataDemo(QCustomPlot *customPlot);

private slots:
  void realtimeDataSlot(mha_wave_t *wave, float *spectrum, unsigned int spectrum_size);

signals:
    void samplesReady(mha_wave_t *wave, float *spectrum, unsigned int spectrum_size);

private:
  Ui::MainWindow *ui;
  QString demoName;
};

#endif // MAINWINDOW_H
