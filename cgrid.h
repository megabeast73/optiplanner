#ifndef CGRID_H
#define CGRID_H

#include <QGraphicsItem>
#include <qpen.h>
#include <celement.h>

class CGrid : public CElement
{
public:
    CGrid();
    ~CGrid() override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual QRectF boundingRect() const override;

    qreal GetZoomPixelSize();
    void DrawVisibleGrid(QPainter * painter,const QStyleOptionGraphicsItem *option);

    void SetSceneSize(const QRectF &rect);

    const QColor getGridColor();
    void setGridColor(const QColor &c);

    const QColor getScaleColor();
    void setScaleColor(const QColor &c);

protected:

    QPen m_penGrid;
    QPen m_penScaleText;
    QPen m_penBorder;

    QRectF m_SceneSize;

};

#endif // CGRID_H
