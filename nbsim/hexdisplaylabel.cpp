#include "hexdisplaylabel.h"
#include <QPainter>
#include <QCoreApplication>
#include <QtGui>

HexDisplayLabel::HexDisplayLabel(QWidget *parent) : QWidget(parent)
{
    resize(30, 49);

    setMinimumSize(QSize(30,49));

    for (int i = 0; i < 8; i++)
        m_seg[i] = 1;
}

void HexDisplayLabel::paintEvent(QPaintEvent* event)
{
    QPixmap pixmap(":/images/assets/hex-display.png");

    QPainter painter(this);
    painter.drawPixmap(0,0,30,49, pixmap);

    QColor segColor(255,0,0);

    painter.setPen(Qt::NoPen);
    painter.setBrush(segColor);

    painter.setRenderHint(QPainter::Antialiasing);

    if (m_seg[0] == 0)
    {
        //8,11 22,11 19,14 11,14
        static const QPoint seg0[] = {
            QPoint(7,11),
            QPoint(22,11),
            QPoint(19,15),
            QPoint(10,15)
        };

        painter.drawConvexPolygon(seg0,4);
    }

    if (m_seg[1] == 0)
    {
        //19,16 22,13 22,26 19,23
        static const QPoint seg0[] = {
            QPoint(19,16),
            QPoint(23,12),
            QPoint(22,26),
            QPoint(19,23)
        };

        painter.drawConvexPolygon(seg0,4);
    }

    if (m_seg[2] == 0)
    {
        //18,30 21,27 20,39 17,36
        static const QPoint seg0[] = {
            QPoint(19,29),
            QPoint(22,27),
            QPoint(21,39),
            QPoint(18,36)
        };

        painter.drawConvexPolygon(seg0,4);
    }

    if (m_seg[3] == 0)
    {
        //8,37 16,37 19,40 5,40
        static const QPoint seg0[] = {
            QPoint(8,37),
            QPoint(17,37),
            QPoint(20,41),
            QPoint(5,41)
        };

        painter.drawConvexPolygon(seg0,4);
    }

    if (m_seg[4] == 0)
    {
        //4,38 7,35 8,29 5,26
        static const QPoint seg0[] = {
            QPoint(4,39),
            QPoint(7,36),
            QPoint(8,29),
            QPoint(5,26)
        };

        painter.drawConvexPolygon(seg0,4);
    }

    if (m_seg[5] == 0)
    {
        //6,24 9,21 9,14 7,12
        static const QPoint seg0[] = {
            QPoint(6,24),
            QPoint(9,21),
            QPoint(10,15),
            QPoint(7,12)
        };

        painter.drawConvexPolygon(seg0,4);
    }

    if (m_seg[6] == 0)
    {
        //7,25 8,24 18,24 19,25 19,26 18,27 9,27 7,26
        static const QPoint seg0[] = {
            QPoint(7,25),
            QPoint(8,24),
            QPoint(18,24),
            QPoint(19,25),
            QPoint(20,26),
            QPoint(19,28),
            QPoint(9,28),
            QPoint(7,26)
        };

        painter.drawConvexPolygon(seg0,8);
    }

    if (m_seg[7] == 0)
    {
        //23,38 25,38 25,40 23,40
        static const QPoint seg0[] = {
            QPoint(23,38),
            QPoint(25,38),
            QPoint(25,40),
            QPoint(23,40),
        };

        painter.drawConvexPolygon(seg0,4);
    }

    //printf("Rendering\n");
}

void HexDisplayLabel::setSegment(int segment, int onoff)
{
    if (segment >= 0 && segment < 8)
        m_seg[segment] = onoff;

}

void HexDisplayLabel::setValue(int value)
{
    for (int i = 0; i < 8; i++)
    {
        m_seg[i] =  (value & 0x1);
        value >>= 1;
    }
}
