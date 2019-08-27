#include <qpainter.h>
#include "caxises.h"
#include "globals.h"
#include "mainwindow.h"

CAxises::CAxises() : CElement ()
{

    m_ElementType = StaticElement;
    setFlag(ItemIgnoresTransformations,true);
    setFlag(ItemIsMovable);
    setFlag(ItemIsFocusable,true);
    setFlag(ItemIsSelectable,true);
    setFlag(ItemSendsGeometryChanges,true);


    m_NormalPen.setColor(Qt::yellow);
    m_NormalPen.setBrush(QBrush(Qt::red,Qt::SolidPattern));
    m_NormalPen.setStyle(Qt::SolidLine);
    m_NormalPen.setWidth(2);

    m_PainterPath.lineTo(0,-100);

    m_PainterPath.moveTo(0,0);
    m_PainterPath.lineTo(100,0);
    m_PainterPath.moveTo(0,0);
    m_PainterPath.lineTo(-50,50);

    m_BoundingRect = m_PainterPath.boundingRect();
    m_BoundingRect.setWidth(m_BoundingRect.width() +50);
    m_BoundingRect.setHeight(m_BoundingRect.height() + 50);

    setDegr(0);


}

void CAxises::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    CElement::paint(painter,option,widget);
    QPen p (m_NormalPen);
    p.setColor(Qt::white);
    p.setWidth(2);
    painter->setPen(p);
    painter->drawText(-15,-100,"+/- 1deg");
    painter->drawText(100,5," +/- 10deg");
    painter->drawText(-60,-5,m_sDegr);


}

void CAxises::setDegr(int d)
{
    m_Degr = d;
    m_sDegr ="Rot: ";
    m_sDegr.append(QString::number(d));
    m_sDegr.append("deg");

    update();
}
