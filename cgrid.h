#ifndef CGRID_H
#define CGRID_H

#include <QGraphicsItem>
#include <qpen.h>
#include <celement.h>

class CGrid : public CElement
{
public:
    CGrid();
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual QRectF boundingRect() const override;
    qreal GetZoomPixelSize();
    void DrawVisibleGrid(QPainter * painter,const QStyleOptionGraphicsItem *option);

    virtual ElementType elementType() override { return StaticElement;}

protected:
    QVector<qreal> m_Dashes;
    QPen m_penGrid;
    QPen m_penScaleText;
    QPen m_penBorder;
    bool m_addedtoscene;
    qreal m_stopx,m_stopy;
};

#endif // CGRID_H
