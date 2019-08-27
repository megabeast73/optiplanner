#ifndef CAXISES_H
#define CAXISES_H
#include "celement.h"


class CAxises : public CElement
{
public:
    CAxises();

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setDegr(int d);
    int getDegr() { return m_Degr;}
protected:
    int m_Degr;
    QString m_sDegr;

};

#endif // CAXISES_H
