#include "globals.h"
#include "cadvgraphicsview.h"
#include <qpoint.h>
#include <QWheelEvent>
#include <QScrollBar>
#include "mainwindow.h"
#include "cadvscene.h"
#include "undocommands.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(ElementLog)
Q_LOGGING_CATEGORY(ElementLog, "GraphicsView: ")

CAdvGraphicsView::CAdvGraphicsView() :QGraphicsView ()
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setInteractive(true);

    setDragMode(RubberBandDrag);
    connect(this,&QGraphicsView::rubberBandChanged,this,&CAdvGraphicsView::ruberChanged);
    m_Axises = nullptr;
    m_UndoRotate = nullptr;




}
CAdvGraphicsView::CAdvGraphicsView(QWidget * parent) : QGraphicsView (parent)
{

}
CAdvGraphicsView::~CAdvGraphicsView()
{

}

qreal CAdvGraphicsView::GetZoomPixelSize()
{
    QRect vp(viewport()->rect());
    QRectF f(mapToScene(vp).boundingRect());

    return  (f.width() / vp.width());
}

void CAdvGraphicsView::zoomIn()
{
    qreal rPerc = GetZoomPixelSize();


    if (rPerc > 1)
        scale(qreal(2), qreal(2));
}

void CAdvGraphicsView::zoomOut()
{

     QRectF f(mapToScene(viewport()->rect()).boundingRect());

     if (!f.contains(sceneRect()))
        scale(1 / qreal(2),1 / qreal(2));

}
void CAdvGraphicsView::wheelEvent(QWheelEvent* event)
{

    if (event->delta() > 0)
    {
        QPointF s = mapToScene(event->pos());
        zoomIn();
        centerOn(s);
        viewport()->update();

    }
    else {
        zoomOut();
    }

}

void CAdvGraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (dynamic_cast<CAdvScene *>(scene())->isAdding())
    {
        QGraphicsView::mousePressEvent(event);
        return;
    }
    if ( event->button() == Qt::MiddleButton)
    {
       m_BaseMousePos = event->screenPos().toPoint();
        setCursorShape(Move);
        setMouseTracking(true);
     }
    else
        QGraphicsView::mousePressEvent(event);
}
void CAdvGraphicsView::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    if (dynamic_cast<CAdvScene *>(scene())->isAdding())
    {
        QGraphicsView::mouseReleaseEvent(mouseEvent);
        return;
    }
    setCursorShape(Arrow);
    setMouseTracking(false);
    QGraphicsView::mouseReleaseEvent(mouseEvent);



}
void CAdvGraphicsView::mouseMoveEvent(QMouseEvent* event)
{

    if (dynamic_cast<CAdvScene *>(scene())->isAdding())
    {
        QGraphicsView::mouseMoveEvent(event);
        return;
    }
    if (event->buttons() & Qt::MidButton)
    {
        ScrollSceneTo(event->screenPos().toPoint());
        return;
    }
    if (event->modifiers() & Qt::AltModifier)
    {
        rotateSelection(event->pos());
    }
    QGraphicsView::mouseMoveEvent(event);

 }

void CAdvGraphicsView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_R && event->modifiers() & Qt::ControlModifier)
    { //REFRESH
        setViewportUpdateMode(FullViewportUpdate);
        viewport()->update();
        setViewportUpdateMode(SmartViewportUpdate);
        g_MainWindow->showStatusMessage("Viewport refreshed!");
    }
    if (event->modifiers() & Qt::AltModifier  && event->key() == Qt::Key_Alt)
    {
        m_SelectedItems = scene()->selectedItems();
        event->accept();
        if (m_SelectedItems.count())
        {
            setMouseTracking(true);
            setCursorShape(Rotate);
            setViewportUpdateMode(FullViewportUpdate);
            m_BaseMousePos = QCursor::pos();
            if (m_SelectedItems.count() > 1)
                showAxises(mapToScene(mapFromGlobal(m_BaseMousePos)).toPoint());
            else
                showAxises(m_SelectedItems.at(0)->pos());
            m_UndoRotate = new CUndoRotate(m_SelectedItems);


        }

    }
    else {
        setMouseTracking(false);
        setCursorShape(Arrow);
        QGraphicsView::keyPressEvent(event);
    }
}
void CAdvGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Alt)
    {
        event->setAccepted(true);
        setMouseTracking(false);
        setCursorShape(Arrow);
        hideAxises();
        viewport()->update();
        setViewportUpdateMode(SmartViewportUpdate);
        if (m_Axises)
        {
            if (m_Axises->getDegr())
            {
                m_UndoRotate->finishPropChange();
                g_UndoStack->push(m_UndoRotate);
                for (int i =m_SelectedItems.count()-1; i>=0; i--)
                    dynamic_cast<CElement *>(m_SelectedItems.at(i))->reconnectElement();
            }
            else
                delete m_UndoRotate;
        }
    }
    else {
        QGraphicsView::keyReleaseEvent(event);
    }
}

void CAdvGraphicsView::showAxises(const QPointF pos)
{
    if (!m_Axises)
    {
        m_Axises = new CAxises;
        scene()->addItem(m_Axises);
    }
    m_Axises->setPos(pos);
    m_Axises->setVisible(true);
}

void CAdvGraphicsView::hideAxises()
{
    if (!m_Axises)
        return;
    m_Axises->setVisible(false);
}

const QBrush CAdvGraphicsView::getBackBrush() const
{
    return backgroundBrush();
}
void CAdvGraphicsView::setBackBrush(const QBrush& c)
{
    setBackgroundBrush(c);
}



void CAdvGraphicsView::ruberChanged(QRect rubberBandRect, QPointF fromScenePoint, QPointF toScenePoint)
{
    if ( fromScenePoint.x() > toScenePoint.x())
    {
        if (rubberBandSelectionMode() != Qt::IntersectsItemShape)
            setRubberBandSelectionMode(Qt::IntersectsItemShape);
    }
    else
        if (rubberBandSelectionMode() != Qt::ContainsItemShape)
            setRubberBandSelectionMode(Qt::ContainsItemShape);
}


void CAdvGraphicsView::setCursorShape(MouseShape sh)
{
    QString res;

    switch (sh) {
        case Arrow :
        viewport()->setCursor(Qt::ArrowCursor);
        return;

        case Rotate :
        res = ":/IMAGES/CURSOR_ROTATE";
        break;

        case Move :
        res = ":/IMAGES/CURSOR_MOVE_SCENE";
        break;

        case Precision :
        res = ":/IMAGES/CURSOR_PRECISION";
        break;

    default:
        return;
    }
    QCursor cursor(QPixmap(res,nullptr,Qt::AutoColor));
    viewport()->setCursor(cursor);
}


void CAdvGraphicsView::ScrollSceneTo(const QPoint newPos)
{
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    QScrollBar *pBar;

    QPoint translation = newPos - m_BaseMousePos;

    pBar = horizontalScrollBar();
    pBar->setValue(pBar->value() - translation.x());
    pBar = verticalScrollBar();
    pBar->setValue(pBar->value() - translation.y());
    m_BaseMousePos = newPos;
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

}

void CAdvGraphicsView::rotateSelection(const QPoint pos)
{
    // dY move by 5 pix increases angle by 1 degree
    // dX move increases angle by 10 degr. on each 10 pix

    int delta =  pos.y()- m_BaseMousePos.y();
    int angle = m_SelectedItems.at(0)->rotation();;
    if (abs(delta) >5)
    {
        m_BaseMousePos = pos;
        angle = angle + (1 * delta /5);

    }
    else
    {
        delta = pos.x() - m_BaseMousePos.x();
        if (abs(delta) > 10)
        {
            m_BaseMousePos = pos;
            angle = angle + (10 * delta /10);
        }
    }

    if (angle > 360)
        angle = angle -360;
    if (angle < 0)
        angle = 360 +angle;

    for (int i = m_SelectedItems.count()-1; i >=0; i--)
        m_SelectedItems.at(i)->setRotation(angle);
    m_Axises->setDegr(angle);

}
