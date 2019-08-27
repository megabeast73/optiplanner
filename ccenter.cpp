#include "ccenter.h"
#include <qpainter.h>

#define CC_WIDTH 500
#define CC_HEIGHT 500
#define CC_PEN_WIDTH 20

CCenter::CCenter()
 : CElement()
{
    m_ElementType = ControlCenter;
    m_NameVisible = false;

    m_NormalPen.setColor(Qt::white);
    m_NormalPen.setBrush(QBrush(Qt::white,Qt::SolidPattern));
    m_NormalPen.setStyle(Qt::SolidLine);
    m_NormalPen.setWidth(CC_PEN_WIDTH);

    m_SelectedPen.setColor(Qt::yellow);
    m_SelectedPen.setBrush(QBrush(Qt::yellow,Qt::SolidPattern));//Dense4Pattern));
    m_SelectedPen.setStyle(Qt::DashLine);
    m_SelectedPen.setWidth(CC_PEN_WIDTH);

    m_PainterPath.addRoundedRect(0,0,CC_WIDTH,CC_HEIGHT,50,50);
    //Sliders
    qreal th = CC_WIDTH/4;
    m_PainterPath.moveTo(th,30);
    m_PainterPath.lineTo(th, CC_HEIGHT/2);

    m_PainterPath.moveTo(th*2,30);
    m_PainterPath.lineTo(th*2,CC_HEIGHT/2);

    m_PainterPath.moveTo(th*3,30);
    m_PainterPath.lineTo(th*3,CC_HEIGHT/2);

    m_PainterPath.moveTo(0,CC_HEIGHT/2 + 30);
    m_PainterPath.lineTo(CC_WIDTH,CC_HEIGHT/2+30);
    //Circles
    m_PainterPath.addEllipse(th - CC_PEN_WIDTH/2-5,CC_HEIGHT/4+30,30,30);
    m_PainterPath.addEllipse(th*2 - CC_PEN_WIDTH/2 -5,60,30,30);
    m_PainterPath.addEllipse(th*3 - CC_PEN_WIDTH/2 -5,CC_HEIGHT/4,30,30);

    m_Font.setFamily("Courrier");
    m_Font.setPointSize(m_Font.pointSize() * 4);

    m_FontPen.setColor(Qt::red);
    m_FontPen.setWidth(4);
    //m_PainterPath.addText(50,400,fnt,"Control Center");
    m_BoundingRect.setWidth(CC_WIDTH+CC_PEN_WIDTH);
    m_BoundingRect.setHeight(CC_HEIGHT+CC_PEN_WIDTH);
    m_BoundingRect.setX(-CC_PEN_WIDTH);
    m_BoundingRect.setY(-CC_PEN_WIDTH);



}

void CCenter::setNameVisibe(bool b)
{
    m_NameVisible = b;
    update();
}

CCenter::~CCenter()
{

}

void CCenter::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    CElement::paint(painter,option,widget);
    painter->setPen(m_FontPen);
    painter->setFont(m_Font);
    painter->drawText(50,CC_HEIGHT/2 + 100,"Control Center");
    if (m_NameVisible)
        painter->drawText(50,CC_HEIGHT/2 + 150,getElementName());

}

void CCenter::setElementName(const QString &ElementName)
{
    m_ElementName =ElementName;
    setToolTip(elementTypeName().append(": ").append(ElementName));
}
