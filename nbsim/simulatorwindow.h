#ifndef SIMULATORWINDOW_H
#define SIMULATORWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTimer>

#include <functional>

#include "nbsoc.h"

namespace Ui {
class SimulatorWindow;
}

class SimulatorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SimulatorWindow(QWidget *parent = 0);
    ~SimulatorWindow();

    void onBlitToGfxRam(nbSoC& nanobrain);
    void onLedGreenWrite(uint16_t val);

private:
    Ui::SimulatorWindow *ui;

    QString m_str;

    QTimer* m_timer;
};

#endif // SIMULATORWINDOW_H
