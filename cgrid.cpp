#include "cgrid.h"
#include <qgraphicsscene.h>
#include <qpainter.h>
#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>


CGrid::CGrid() : CElement ()
{
    m_penGrid.setColor(Qt::black);
    m_penGrid.setWidth(0);
    m_penGrid.setStyle(Qt::DotLine);

    QVector<qreal> dashes;
    dashes << 1 << 5;
    m_penGrid.setDashPattern(dashes);

    m_penScaleText.setStyle(Qt::SolidLine);
    m_penScaleText.setColor(QColor(Qt::yellow));
    m_penScaleText.setWidth(0);

    m_penBorder.setColor(Qt::red);
    m_penBorder.setWidth(0);
    m_penBorder.setStyle(Qt::SolidLine);

    m_addedtoscene = false;
    m_Dirty = false;

    setFlag(ItemIsSelectable,false);
    setFlag(ItemIsMovable,false);
    setFlag(ItemSendsGeometryChanges,false);
    setZValue(-1);
}


qreal CGrid::GetZoomPixelSize()
{
    QRect vp(scene()->views().at(0)->viewport()->rect());
    QRectF f(mapToScene(vp).boundingRect());

    return  (f.width() / vp.width());
}
void CGrid::DrawVisibleGrid(QPainter * painter,const QStyleOptionGraphicsItem *option)
{
    QRect exposed (option->exposedRect.toAlignedRect());
    QRect rScene(boundingRect().toAlignedRect());

    //Draw border
    painter->setPen(m_penBorder);

    painter->drawLine(rScene.topLeft(),rScene.bottomLeft());
    painter->drawLine(rScene.bottomLeft(),rScene.bottomRight());
    painter->drawLine(rScene.bottomRight(),rScene.topRight());
    painter->drawLine(rScene.topLeft(),rScene.topRight());

    int start_x =int(exposed.x() /100) * 100;
    int start_y = int(exposed.y()/100) * 100;
    int stop_x = start_x + exposed.width();
    int stop_y = start_y + exposed.height();
    //Depending on the scale, draw draw grid on equal proportions of 100 pixels
    qreal rLineWidth = 100 * 1/ painter->transform().m11();
    qreal rLineHeight = 100 * 1/painter->transform().m22();
    painter->setPen(m_penGrid);

    for (int x = start_x; x < stop_x; x += rLineWidth)
        painter->drawLine(x,start_y,x,stop_y);
    for (int y = start_y; y < stop_y; y += rLineHeight)
    {
        painter->drawLine(start_x,y,stop_x,y);

    }

    //Now calculate the distance of one gap in the grid and draw it in the viewport
    QString strM;
    QPainter p (scene()->views().at(0)->viewport());
    QRect vp (scene()->views().at(0)->viewport()->rect());

    p.setPen(m_penScaleText);

    p.drawLine(0,vp.height()-1,100,vp.height()-1);
    p.drawLine(0,vp.height()-1,0,vp.height()-10);
    p.drawLine(100,vp.height()-1,100,vp.height()-10);

    rLineWidth = 100 *  1 / painter->transform().m11();
    if (rLineWidth <= 100)
        strM = " cm";
    else if (rLineWidth <= 100000)
    {
        rLineWidth = rLineWidth / 100;
        strM = " m";
    }
    else {
        rLineWidth = rLineWidth / 100000;
        strM = " km";
    }
    strM.prepend(QString::number(rLineWidth,'f', 2)).prepend("1d = ");
    p.drawText(3,vp.height()-3,strM);



}


void CGrid::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!m_addedtoscene)
    { //Get the scene size on firs paint
        QRectF size = scene()->sceneRect();
         m_stopy = size.height();
         m_stopx = size.width();
        m_addedtoscene = true;
    }

    DrawVisibleGrid(painter,option);



}

QRectF CGrid::boundingRect() const
{

    return scene()->sceneRect();
}
