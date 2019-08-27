#ifndef CTEXTELEMENT_H
#define CTEXTELEMENT_H
#include "celement.h"
#include <QFont>
#include <QBrush>


class CTextElement : public CElement
{
public:
    CTextElement();
    virtual ElementType elementType() override { return StaticElement; }
    virtual bool isConnectionAccepted(CElement *other) override {return false;}
    //Pure virtuals
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    virtual void setText(const QString &str);
    virtual void setFontSize(int size);
    virtual void setTextColor(const QColor &c);

protected:
    virtual void prepareText();

    QString m_text;
    QFont m_Font;
    QBrush m_brush;
    QPen m_Pen;
 };


#endif // CTEXTELEMENT_H
