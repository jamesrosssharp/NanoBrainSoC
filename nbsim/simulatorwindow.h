#ifndef SIMULATORWINDOW_H
#define SIMULATORWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTimer>
#include <QCloseEvent>

#include <functional>

#include <cstdint>

#include "nbsoc.h"
#include "hexdisplaylabel.h"


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
    void onLedGreenWrite(std::uint16_t val);
    void onLedRedWrite(std::uint16_t val);
    void onHexWrite(int hex, std::uint16_t val);

    void onResetButtonPressed(std::function<void (bool)> func) { m_onResetButton = func; }
    void setOnCloseEvent(std::function<void()> func) { m_onCloseEvent = func; }
    void onDebugButtonPressed(std::function<void()> func) { m_onDebuggerPressed = func; }

    void closeEvent(QCloseEvent *ev);

public slots:
    void update();

    void onResetPressed();
    void onResetReleased();
    void onDebugPressed();

private:
    Ui::SimulatorWindow *ui;
    QString m_str;
    QTimer* m_timer;

    std::uint16_t m_ledGreen;
    std::uint16_t m_ledRed;

    HexDisplayLabel* m_hexLabels[4];
    std::uint16_t m_hexVals[4];

    std::function<void()>       m_onCloseEvent;
    std::function<void (bool)>  m_onResetButton;

    std::function<void()>       m_onDebuggerPressed;
};

#endif // SIMULATORWINDOW_H
