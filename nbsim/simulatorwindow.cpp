#include "simulatorwindow.h"
#include "ui_simulatorwindow.h"
#include "hexdisplaylabel.h"

#include <QString>

#include <iostream>

SimulatorWindow::SimulatorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SimulatorWindow)
{
    ui->setupUi(this);

    for (int i = 0; i < 4; i ++)
    {
        m_hexLabels[i] = new HexDisplayLabel;
        ui->LayoutForHex->addWidget(m_hexLabels[i]);
        m_hexLabels[i]->show();
    }

    m_timer = new QTimer();

    connect(m_timer, SIGNAL(timeout()), this, SLOT(update()));

    m_timer->start(10);
}

SimulatorWindow::~SimulatorWindow()
{
    delete ui;
    delete m_timer;
}

void SimulatorWindow::onBlitToGfxRam(nbSoC& nanobrain)
{

}

void SimulatorWindow::onLedGreenWrite(uint16_t val)
{
    m_ledGreen = val;
}

void SimulatorWindow::onLedRedWrite(uint16_t val)
{
    m_ledRed = val;
}

void SimulatorWindow::closeEvent(QCloseEvent *ev)
{
    m_onCloseEvent();
}

void SimulatorWindow::update()
{
    QLabel* greenLEDs[] = {ui->greenLed0, ui->greenLed1, ui->greenLed2, ui->greenLed3,
                           ui->greenLed4, ui->greenLed5, ui->greenLed6, ui->greenLed7};


    std::uint16_t gled = m_ledGreen;
    for (int i = 0; i < 8; i++)
    {
        if ((gled & 1))
        {
            greenLEDs[i]->setStyleSheet("QLabel { background: url(:/images/assets/LED-green.png) no-repeat };");
        }
        else
        {
            greenLEDs[i]->setStyleSheet("QLabel { background: url(:/images/assets/LED.png) no-repeat };");
        }
        gled >>= 1;
    }

    QLabel* redLEDs[] = {ui->redLed0, ui->redLed1, ui->redLed2, ui->redLed3,
                         ui->redLed4, ui->redLed5, ui->redLed6, ui->redLed7,
                         ui->redLed8, ui->redLed9 };


    std::uint16_t rled = m_ledRed;
    for (int i = 0; i < 10; i++)
    {
        if ((rled & 1))
        {
            redLEDs[i]->setStyleSheet("QLabel { background: url(:/images/assets/LED-red.png) no-repeat };");
        }
        else
        {
            redLEDs[i]->setStyleSheet("QLabel { background: url(:/images/assets/LED.png) no-repeat };");
        }
        rled >>= 1;
    }

    // Update hex widgets
    for (int i = 0; i < 4; i ++)
    {
        m_hexLabels[i]->setValue(m_hexVals[i]);
        m_hexLabels[i]->update();
    }
}

void SimulatorWindow::onHexWrite(int hex, std::uint16_t val)
{
    if (hex >= 0 && hex < 4)
        m_hexVals[3 - hex] = val;
}

