#include "cgrid.h"
#include "cadvscene.h"
#include <qpainter.h>
#include <QGraphicsView>
#include <QStyleOptionGraphicsItem>


CGrid::CGrid() : CElement ()
{

    m_ElementType = StaticElement;
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

    setFlag(ItemIsSelectable,false);
    setFlag(ItemIsMovable,false);
    setFlag(ItemSendsGeometryChanges,false);
    setFlag(ItemUsesExtendedStyleOption,true);
    setZValue(-1000);
}





CGrid::~CGrid()
{
    //Inform the scene about my death
    CAdvScene *p = dynamic_cast<CAdvScene*>(scene());
    p->m_Grid = nullptr;
}

const QColor CGrid::getGridColor()
{
    return m_penGrid.color();
}
void CGrid::setGridColor(const QColor &c)
{
    m_penGrid.setColor(c);
    update();
}

const QColor CGrid::getScaleColor()
{
    return m_penScaleText.color();
}
void CGrid::setScaleColor(const QColor &c)
{
    m_penScaleText.setColor(c);
    update();
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

    int start_x = exposed.x();
    int start_y = exposed.y();
    int stop_x = start_x + exposed.width();
    int stop_y = start_y + exposed.height();
    start_x = int(start_x/100) * 100;
    start_y = int(start_y/100) * 100;
    //Depending on the scale, draw grid on equal proportions of 100 pixels
    qreal rLineWidth = 100 * 1/ painter->transform().m11();
    qreal rLineHeight = 100 * 1/painter->transform().m22();
    painter->setPen(m_penGrid);

    for (int x = start_x; x <= stop_x; x += rLineWidth)
        painter->drawLine(x,start_y,x,stop_y);
    for (int y = start_y; y <= stop_y; y += rLineHeight)
    {
        painter->drawLine(start_x,y,stop_x,y);

    }

    //Now calculate the distance of one gap in the grid and draw it in the viewport
    QString strM;
    QPainter p (scene()->views().at(0)->viewport());
    QRect vp (scene()->views().at(0)->viewport()->rect());

    p.setPen(m_penScaleText);
    //draw scale line
    p.drawLine(0,vp.height()-1,100,vp.height()-1);
    p.drawLine(0,vp.height()-1,0,vp.height()-10);
    p.drawLine(100,vp.height()-1,100,vp.height()-10);
    rLineWidth = painter->transform().m11();
    if (rLineWidth > 0)
            rLineWidth = 100 *  1 / rLineWidth;
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
    DrawVisibleGrid(painter,option);
}

QRectF CGrid::boundingRect() const
{
    Q_ASSERT(m_SceneSize.isValid());
    return m_SceneSize;
}

void CGrid::SetSceneSize(const QRectF &rect)
{
    m_SceneSize = rect;
    prepareGeometryChange();
    update();
}
