#include "simulatorwindow.h"
#include "ui_simulatorwindow.h"

#include <QString>

SimulatorWindow::SimulatorWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SimulatorWindow)
{
    ui->setupUi(this);

    //m_timer = new QTimer();

    //connect(m_timer, SIGNAL(timeout()), this, SLOT(updatePreview()));

    //m_timer->start();
}

SimulatorWindow::~SimulatorWindow()
{
    delete ui;
    //delete m_timer;
}

void SimulatorWindow::onBlitToGfxRam(nbSoC& nanobrain)
{

}


void SimulatorWindow::onLedGreenWrite(uint16_t val)
{

}

void SimulatorWindow::closeEvent(QCloseEvent *ev)
{
    m_onCloseEvent();
}
