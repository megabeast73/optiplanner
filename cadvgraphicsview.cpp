#include "cadvgraphicsview.h"
#include <qpoint.h>
#include <QWheelEvent>
#include <QScrollBar>
#include "cgrid.h"
#include "celement.h"
#include "globals.h"
#include "mainwindow.h"
#include <qmenu.h>
#include "cinfraprop.h"

#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(ElementLog)
Q_LOGGING_CATEGORY(ElementLog, "GraphicsView: ")

CAdvGraphicsView::CAdvGraphicsView() :QGraphicsView ()
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pGrid = nullptr;
    m_bRotating = false;
    m_bSelecting = false;
    m_inserting = false;
    m_ActiveElement = nullptr;
    m_pPropDialog = nullptr;
    scale(1,1);


}
CAdvGraphicsView::~CAdvGraphicsView()
{
    if (m_pGrid)
        delete m_pGrid;
    if (m_ActiveElement && m_ActiveElement->Dirty())
            delete  m_ActiveElement;
    ClosePropDialog();
    if (m_pPropDialog)
        delete m_pPropDialog;
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
void CAdvGraphicsView::setSceneAA(QGraphicsScene *scene)
{

    m_SceneRect = scene->sceneRect();
    QGraphicsView::setScene(scene);
    InsertGrid();
}


void CAdvGraphicsView::paintEvent(QPaintEvent *ev)
{
    QGraphicsView::paintEvent(ev);
    if (m_bRotating)
    {
        //qCDebug(ElementLog) << "Starting paint";
        QPen pen;
        QPainter p (viewport());
        pen.setColor(Qt::yellow);
        pen.setBrush(QBrush(Qt::yellow,Qt::SolidPattern));//Dense4Pattern));
        pen.setStyle(Qt::DashLine);
        pen.setWidth(1);

        p.setPen(pen);
        int x = m_rotatePoint.x();
        int y = m_rotatePoint.y();
        p.drawLine(x - 150,y,x+150,y);
        p.drawLine(x,y,x,y-150);
        p.drawLine(x,y,x-75,y+75);
    }
    if (m_bSelecting)
        drawSelectionRect();

}


qreal CAdvGraphicsView::GetZoomPixelSize()
{
    QRect vp(viewport()->rect());
    QRectF f(mapToScene(vp).boundingRect());

    return  (f.width() / vp.width());
}

CElement *CAdvGraphicsView::ActiveElement() const
{
    return m_ActiveElement;
}

void CAdvGraphicsView::setActiveElement(CElement *ActiveElement)
{
    if (m_ActiveElement && m_ActiveElement->Dirty())
            delete  m_ActiveElement;
    m_ActiveElement = ActiveElement;
}

bool CAdvGraphicsView::inserting() const
{
    return m_inserting;
}

void CAdvGraphicsView::setInserting(bool inserting)
{
    m_inserting = inserting;
}

void CAdvGraphicsView::drawForeground(QPainter* painter, const QRectF& rect)
{
    QGraphicsView::drawForeground(painter,rect);

}

void CAdvGraphicsView::ScrollSceneTo(const QPoint &newPos)
{
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    QScrollBar *pBar;
    QPoint oldp(m_MouseOrigX, m_MouseOrigY);
    QPoint translation = newPos - oldp;

    pBar = horizontalScrollBar();
    pBar->setValue(pBar->value() - translation.x());
    pBar = verticalScrollBar();
    pBar->setValue(pBar->value() - translation.y());
    m_MouseOrigX = newPos.x();
    m_MouseOrigY = newPos.y();
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

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

     if (!f.contains(m_SceneRect))
        scale(1 / qreal(2),1 / qreal(2));

}
void CAdvGraphicsView::wheelEvent(QWheelEvent* event)
{
//    QGraphicsView::wheelEvent(event);

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
void CAdvGraphicsView::endRotation(bool bCancel)
{
    m_bRotating = false;
    setCursorShape(Arrow);

    QList<QGraphicsItem *> sel (scene()->selectedItems());
    if (bCancel)
        for (int i = sel.count()-1; i >=0; i--)
            sel.at(i)->setRotation(0);
     viewport()->update();
     setMouseTracking(false);
}
void CAdvGraphicsView::startRotation()
{
        int nItems = 0;
        //On start get the number of items to rotate.
        //This is to know where to draw the axis
        if (!m_bRotating)
        {
            nItems = scene()->selectedItems().count();
            m_bRotating = nItems > 0;
        }
        if (!m_bRotating)
            return;

        setCursorShape(Rotate);
        if (nItems > 1) //more the 1 item, axis at the mouse
            m_rotatePoint = viewport()->mapFromGlobal(QCursor::pos());
        else //one item, axis at the item
        {
            QGraphicsItem * pItem = scene()->selectedItems().at(0);
            m_rotatePoint = pItem->pos().toPoint();
           // m_rotatePoint = pItem->mapToScene(m_rotatePoint).toPoint();
            m_rotatePoint =  mapFromScene(m_rotatePoint);
            m_rotatePoint = mapToGlobal(m_rotatePoint);
            m_rotatePoint = viewport()->mapFromGlobal(m_rotatePoint);

        }
        m_MouseOrigX = mapFromGlobal(QCursor::pos()).x();
        m_MouseOrigY = mapFromGlobal(QCursor::pos()).y();
        viewport()->update();
        setMouseTracking(true);

}
void CAdvGraphicsView::rotate(const QPoint pos)
{
    // dY move by 2 pix increases angle by 1 degree
    // dX move increases angle by 45 degr. on each 100 pix
    int delta = pos.y() - m_MouseOrigY;
    delta = delta / 2;
    delta = delta + ((m_MouseOrigX + pos.x()) /100) * 45;
    QList<QGraphicsItem *> sel (scene()->selectedItems());
    for (int i = sel.count()-1; i >=0; i--)
        sel.at(i)->setRotation(delta);

}

void CAdvGraphicsView::selectItems(const QPoint &toPos)
{
    Qt::ItemSelectionMode mode;
    QRectF rect;
    if (toPos.x() < m_MouseOrigX)
    {
        mode = Qt::IntersectsItemShape;
        rect.setTopLeft(toPos);
        rect.setWidth(m_MouseOrigX - toPos.x());
        rect.setHeight(m_MouseOrigY - toPos.y());
    }
    else
    {
        mode = Qt::ContainsItemShape;
        rect.setBottomRight(toPos);
        rect.setX(m_MouseOrigX);
        rect.setY(m_MouseOrigY);
    }
    rect =  mapToScene(rect.toRect()).boundingRect();
    QList<QGraphicsItem *> items = scene()->items(rect,mode);
    for (int i = items.count() -1; i >=0; i--)
        items.at(i)->setSelected( !items.at(i)->isSelected());

}

void CAdvGraphicsView::drawSelectionRect()
{
    QPainter  painter(viewport());
    QPen pen(Qt::yellow);
    pen.setWidth(0);
    pen.setBrush(QBrush(Qt::yellow,Qt::SolidPattern));//Dense4Pattern));
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);
    painter.drawRect(m_MouseOrigX,m_MouseOrigY,m_rotatePoint.x()-m_MouseOrigX,m_rotatePoint.y()-m_MouseOrigY);
    viewport()->update();


}

void CAdvGraphicsView::keyPressEvent(QKeyEvent *event)
{
    emit keyPress(event);
    if (m_inserting)
        return;


    if (event->modifiers() & Qt::AltModifier)
    {
        startRotation();
        event->accept();
    }
    else {
        QGraphicsView::keyPressEvent(event);
    }
}

void CAdvGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    if (m_inserting)
        return;
    QGraphicsItem * p;
    if (m_bRotating && !(event->modifiers() & Qt::AltModifier))
    {
        endRotation(event->key() == Qt::Key_Escape);
        event->accept();
        return;
    }
    QList<QGraphicsItem*> sel = scene()->selectedItems();

    switch (event->key())
    {
        case Qt::Key_Delete:
            for (int i = sel.count() -1; i >= 0; i--)
            {
                p = sel.at(i);
                p->setSelected(false);
                scene()->removeItem(p);
                g_Objects.deregisterElement(dynamic_cast<CElement *>(p));
                if (p == m_ActiveElement)
                    m_ActiveElement = nullptr;
                delete p;
            }
            break;

    }
    QGraphicsView::keyReleaseEvent(event);

}

void CAdvGraphicsView::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
   // bool bTmp = m_inserting || m_bRotating;
//    mouseEvent->setAccepted(false);
    emit mouseRelease(mouseEvent);

    if (m_inserting)
        return;
    if (m_bRotating)
    {
        if (mouseEvent->button() == Qt::RightButton)
            endRotation(true);
        return;
    }

    QGraphicsView::mouseReleaseEvent(mouseEvent);
    m_MouseBState = 0;
    CElement * pEl;
    int nItems;
    switch( mouseEvent->button())
    {
    case Qt::MiddleButton:
        setCursorShape(Arrow);
        setMouseTracking(false);
        break;
    case Qt::LeftButton:
        setViewportUpdateMode(MinimalViewportUpdate);
        setCursorShape(Arrow);
        setMouseTracking(false);
        if (m_bSelecting)
        {
            m_bSelecting = false;
            selectItems(mouseEvent->pos());
        }
        nItems = scene()->selectedItems().count();
        if (nItems == 1)
        {
            pEl = (CElement *) scene()->selectedItems().at(0);
            g_MainWindow->setSelectedElement(pEl);
        }
        else
            g_MainWindow->clearSelectedElement();

        break;
    }


}
void CAdvGraphicsView::mousePressEvent(QMouseEvent* event)
{

    bool bTmp = m_bRotating || m_inserting; //save state
    emit mousePress(event);
    if (m_bRotating || m_inserting) //m_inserting)
        return;

    // Store original position.
    m_MouseOrigX = event->x();
    m_MouseOrigY = event->y();
    event->setAccepted(bTmp);
    QGraphicsView::mousePressEvent(event);


    m_MouseBState =event->buttons();
    if (m_MouseBState & Qt::MidButton)
    {
        setCursorShape(Move);
        return;
    };

    switch (event->button())
    {
    case Qt::LeftButton:


//            m_bSelecting = true;
//            setCursorShape(Precision);
            setMouseTracking(true);
//            setViewportUpdateMode(FullViewportUpdate);

        break;

    case Qt::RightButton:
        if (bTmp)
            return;
        m_ActiveElement = dynamic_cast<CElement*>(itemAt(event->pos()));
        if (m_ActiveElement == m_pGrid)
            m_ActiveElement = nullptr; //Do not select the grid
        if (m_ActiveElement)
            m_ActiveElement->ShowContexMenu();
        break;
    }

}

void CAdvGraphicsView::mouseMoveEvent(QMouseEvent* event)
{
    emit mouseMove(event);
    if (m_inserting)
        return;
    if (m_bRotating)
    {
        rotate(event->pos());
        return;
    }

    if (m_MouseBState & Qt::MidButton)
    {
        QPoint pos(event->pos());
        ScrollSceneTo(pos);
    }

    if (event->buttons() & Qt::LeftButton && scene()->selectedItems().count() < 1)
    {
        m_bSelecting = true;
        setCursorShape(Precision);
                    //setMouseTracking(true);
        setViewportUpdateMode(FullViewportUpdate);
        m_rotatePoint.setX(event->pos().x());
        m_rotatePoint.setY(event->pos().y());
        viewport()->update();
    }


     QGraphicsView::mouseMoveEvent(event);

}

void CAdvGraphicsView::InsertGrid()
{
    RemoveGrid();

    m_pGrid = new CGrid;
    m_pGrid->setPos(0,0);
    scene()->addItem(m_pGrid);
    m_SceneRect = scene()->sceneRect();
}

void CAdvGraphicsView::RemoveGrid()
{
    if (m_pGrid)
    {
        scene()->removeItem(m_pGrid);
        delete m_pGrid;
        m_pGrid = nullptr;
    }
}

void CAdvGraphicsView::ClearScene()
{
    scene()->clear();
    m_pGrid = nullptr;
}

void CAdvGraphicsView::setSaveObject(const QJsonObject &obj)
{

    QRectF sz;
    sz.setWidth(obj["width"].toDouble());
    sz.setHeight(obj["height"].toDouble());
    QColor bg;
    bg.setNamedColor(obj["bgcolor"].toString());
    setBackColor(bg);
    g_MainWindow->setDiagramName(obj["name"].toString());
    scene()->setSceneRect(sz);
    InsertGrid();
}

QJsonObject CAdvGraphicsView::getSaveObject()
{
    QJsonObject ret;
    ret.insert("width",QJsonValue(scene()->sceneRect().width()));
    ret.insert("height",QJsonValue(scene()->sceneRect().height()));
    ret.insert("name",QJsonValue(g_MainWindow->DiagramName()));
    ret.insert("bgcolor",QJsonValue(getBackColor().name(QColor::HexArgb)));
    return ret;
}

void CAdvGraphicsView::contextMenuEvent(QContextMenuEvent *event)
{

//    if (m_bSelecting || m_inserting || m_bRotating)
//        return;
//    if (event->isAccepted())
//        return;
//    QPoint pos = QCursor::pos();
//    QMenu * pMenu = new QMenu("Infrastructure diagram");
//    QAction *a = pMenu->addAction("Diagram Properties");
//    a->setData(QVariant(1));

//    a = pMenu->exec(pos);
//    int sel = a ? a->data().toInt() : 0;
//    delete a;
//    delete pMenu;
//    if (!sel)
//        return; //no selection
//    switch (sel) {
//        case 1:
//            if (!m_pPropDialog)
//                m_pPropDialog = new CInfraProp;
//            m_pPropDialog->open();
//            break;

//    }
}

void CAdvGraphicsView::Closing(QCloseEvent *e)
{
    ClosePropDialog();
    return;
}
void CAdvGraphicsView::ClosePropDialog()
{
    if (m_pPropDialog)
        m_pPropDialog->close();

}

void CAdvGraphicsView::ShowPropDialog()
{
    if (!m_pPropDialog)
        m_pPropDialog = new CInfraProp;
   m_pPropDialog->open();
}

const QColor& CAdvGraphicsView::getBackColor() const
{
    return backgroundBrush().color();
}
void CAdvGraphicsView::setBackColor(const QColor& c)
{
    setBackgroundBrush(c);
}
