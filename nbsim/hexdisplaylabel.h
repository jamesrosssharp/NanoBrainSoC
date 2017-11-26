#pragma once

#include <QWidget>
#include <QLabel>

class HexDisplayLabel : public QWidget
{
    Q_OBJECT
public:
    explicit HexDisplayLabel(QWidget *parent = 0);
    void    setSegment(int segment, int onoff);
    void    setValue(int value);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

signals:

private:
    int m_seg[8];
};
