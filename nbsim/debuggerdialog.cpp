#include "debuggerdialog.h"
#include "ui_debuggerdialog.h"

#include <iostream>

DebuggerDialog::DebuggerDialog(nbSoC* nb, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebuggerDialog),
    m_nanobrain(nb)
{
    ui->setupUi(this);
}

DebuggerDialog::~DebuggerDialog()
{
    delete ui;
}

void DebuggerDialog::onRun()
{
    m_nanobrain->getCPU()->run();
}

void DebuggerDialog::onPause()
{
    m_nanobrain->getCPU()->pause();

    updateDisassembly();
}

void DebuggerDialog::onStep()
{
    m_nanobrain->getCPU()->singleStep();

    updateDisassembly();
}

void DebuggerDialog::updateDisassembly()
{
    std::string registers = m_nanobrain->getCPU()->dumpRegisters();

    ui->textWidget->clear();
    ui->textWidget->insertPlainText(QString(registers.c_str()));

    std::string disas = m_nanobrain->getCPU()->dumpDisas();

    ui->disasWidget->clear();
    ui->disasWidget->insertPlainText(QString(disas.c_str()));

}

void DebuggerDialog::keyPressEvent(QKeyEvent * e)
{
    switch (e->key())
    {
        case Qt::Key_S:
            onStep();
            break;
        case Qt::Key_P:
            onPause();
            break;
        case Qt::Key_R:
            onRun();
            break;
    }
}

void DebuggerDialog::onReset()
{
    m_nanobrain->getCPU()->hardReset();
    updateDisassembly();
}

