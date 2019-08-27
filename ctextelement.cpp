#include "ctextelement.h"
#include <qfontmetrics.h>
#include <qpainter.h>


CTextElement::CTextElement() : CElement ()
  , m_Font ("System")
{

    m_text = "";
   m_Pen.setColor(Qt::white);
   m_Pen.setStyle(Qt::SolidLine);

   m_Font.setPointSize(10);
   setFlag(ItemIsSelectable,false);
   setFlag(ItemIsMovable,false);
   setFlag(ItemSendsGeometryChanges,true);
    prepareText();
   setZValue(-999);
}
void CTextElement::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(m_Pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawText(0,0,m_text);
}

void CTextElement::setText(const QString &str)
{
    m_text = str;
    prepareText();
}

void CTextElement::setFontSize(int size)
{
    m_Font.setPointSize(size);
    prepareText();
}
void CTextElement::prepareText()
{

    QFontMetrics fm(m_Font);
    m_BoundingRect = fm.boundingRect(m_text);

    prepareGeometryChange();
    update();
}
void CTextElement::setTextColor(const QColor &c)
{
    m_Pen.setColor(c);
}
