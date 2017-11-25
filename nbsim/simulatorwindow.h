#ifndef SIMULATORWINDOW_H
#define SIMULATORWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTimer>
#include <QCloseEvent>

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

    void setOnCloseEvent(std::function<void()> func) { m_onCloseEvent = func; }

    void closeEvent(QCloseEvent *ev);

private:
    Ui::SimulatorWindow *ui;
    QString m_str;
    QTimer* m_timer;

    std::function<void()> m_onCloseEvent;
};

#endif // SIMULATORWINDOW_H
