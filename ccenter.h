#ifndef CCENTER_H
#define CCENTER_H

#include "celement.h"
#include <qfont.h>



class CCenter : public CElement
{
public:
    CCenter();
    virtual ~CCenter() override;

    virtual QString elementTypeName() override { return "Control Center"; }
    virtual void setElementName(const QString &ElementName) override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual void setNameVisibe(bool b) override;
    bool getNameVisible() override { return m_NameVisible;}
protected:
    QFont m_Font;
    QPen m_FontPen;
    bool m_NameVisible;

};

#endif // CCENTER_H
