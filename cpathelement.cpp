#include "cpathelement.h"
#include <qpainter.h>
#include <cadvgraphicsview.h>
#include <QGraphicsSceneMouseEvent>
#include <qjsonarray.h>
#include "cadvscene.h"
#include <QMenu>
#include "undocommands.h"
#include "globals.h"
#include "ctextelement.h"

CPathElement::CPathElement() : CElement ()
{
    m_pPath = nullptr;
    AddPoint(0,0);
    AddPoint(40,40);
    preparePainterPath();
    m_Placed = false;
    m_pEditedEdge = nullptr;
}

CPathElement::~CPathElement()
{
    if (m_pPath)
        delete m_pPath;
}
void CPathElement::AddPoint(qreal x,qreal y)
{
    if (!m_pPath)
        m_pPath = new CPathPoint(x,y);
    else
        m_pPath->AddNew(x,y);
}

QRectF CPathElement::boundingRect() const
{
    return m_PainterPath.boundingRect();

}



QJsonObject& CPathElement::getSaveValue()
{
    CPathPoint *p = m_pPath;
    unsigned int cnt =0;
    QString index_name;

    QJsonArray point_arr;
    QJsonValue point_x,point_y,point_v;
    //Clear
    m_Data = QJsonObject();

//Fill the path to the object
    while (p)
    {
        index_name = "p_";
        index_name.append(QString::number(cnt,10));
        point_arr = QJsonArray();

        point_x = QJsonValue(p->point.x());
        point_y = QJsonValue(p->point.y());

        point_arr.append(point_x);
        point_arr.append(point_y);
        point_v = QJsonValue(point_arr);

        m_Data.insert(index_name,point_v);

        p=p->next;
        cnt++;
    }
    return CElement::getSaveValue();
}
void CPathElement::LoadElement(const QJsonObject& obj)
{
    CElement::LoadElement(obj);

    unsigned int cnt =0;
    QString index_name;


    QJsonValue point_x,point_y,point_v;

    delete m_pPath;
    m_pPath = nullptr;
    do {
        index_name = "p_";
        index_name.append(QString::number(cnt,10));
        point_v = obj[index_name];
        if (!point_v.isUndefined())
        {
            point_x = point_v.toArray()[0];
            point_y = point_v.toArray()[1];
            AddPoint(point_x.toDouble(),point_y.toDouble());
        }
        cnt++;
    } while (!point_v.isUndefined());
    preparePainterPath();
    repositionName();
    prepareGeometryChange();

}

void CPathElement::keyPress(QKeyEvent *keyEvent)
{
    if (keyEvent->key() == Qt::Key_Escape)
        m_pPath->RemoveLast();
    else
        CElement::keyPress(keyEvent);
}

void CPathElement::keyRelease(QKeyEvent *keyEvent)
{
    CElement::keyRelease(keyEvent);
}

void CPathElement::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (!m_Placed)
    {
        CElement::mouseMove(mouseEvent);
        return;
    }
    m_pPath->RemoveLast();

    m_pPath->AddNew(mapFromScene(mouseEvent->scenePos()));
    preparePainterPath();
    prepareGeometryChange();
    update();
}
void CPathElement::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{

    if (mouseEvent->button() == Qt::RightButton)
    {
         if (m_Placed)
            dynamic_cast<CAdvScene*>(scene())->finishAddingElement();
        else
            dynamic_cast<CAdvScene*>(scene())->cancelAddingElement();
    }
    else if (mouseEvent->button() == Qt::LeftButton)
    {
        if (!m_Placed)
        {
            m_Placed = true;
            m_pPath->RemoveLast(); //Remove last path point. At startup, there is a little line.
            //return;
        }
        m_pPath->AddNew(mouseEvent->scenePos());
        //No need to prepare path and geometry change.
        //Just add the point of the mouse, even if it is duplicated

        return;
    }
}
void CPathElement::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    CElement::mouseRelease(mouseEvent);
}


void CPathElement::preparePainterPath()
{
    m_PainterPath = QPainterPath();
    CPathPoint *p = m_pPath;
    while (p)
    {
        m_PainterPath.lineTo(p->point);
        p = p->next;
    }
}
QPointF CPathElement::intersection(const QPointF &l1A, const QPointF &l1B,
                             const QPointF &l2A, const QPointF &l2B)
{

    //Calculate intersection point of two lines
    QPointF ret(0,0);

    double a1 = l1B.y() - l1A.y();;
    double b1 = l1A.x() - l1B.x();
    double c1 = a1 * l1A.x() + b1 * l1A.y();

    // Line CD represented as a2x + b2y = c2
    double a2 = l2B.y() - l2A.y();
    double b2 = l2A.x() - l2B.x();
    double c2 = a2 * l2A.x() + b2 * l2A.y();

    double determinant = a1*b2 - a2*b1;
    if (int(determinant) == 0)
    {
        return QPointF();

    }
   double x = (b2*c1 - b1*c2)/determinant;
   double y = (a1*c2 - a2*c1)/determinant;
   ret = QPointF(x, y);

    return ret;
}

#define SELECTION_FUZZINESS  10
CPathPoint * CPathElement::pointOnPos(QPointF pos)
{
    QPointF pos1 = pos;//mapFromScene(pos);// m_pView->mapToScene(pos);
    QRectF sel(pos1.x()-SELECTION_FUZZINESS,pos1.y()-SELECTION_FUZZINESS,
               2*SELECTION_FUZZINESS,2*SELECTION_FUZZINESS); //pos1.x()+20,pos1.y()+20);
    CPathPoint * p = m_pPath;

    while ( p)
        if (sel.contains(p->point))
                return p;
        else
            p = p->next;

   return nullptr;

}


void CPathElement::configureContextMenu(QMenu *pMenu)
{
    CPathPoint * p = pointOnPos(m_MouseClickedLocal);

    setSelected(true);

    QMenu * pSub = pMenu->addMenu("Element");
    QAction *a = pSub->addAction("Add vertex");
    a->setData(QVariant(-100));
    a->setEnabled(p == nullptr);

    a = pSub->addAction("Remove vertex");
    a->setData(QVariant(-101));
    a->setEnabled(p != nullptr && m_pPath->next->next); //Enabled if clicked on edge or there is at least 2 points

    a = pSub->addAction("Split here");
    a->setData(QVariant(-102));
    if ( !p )
        a->setEnabled(true);
    else
        a->setEnabled( p->next && m_pPath != p);

    CElement::configureContextMenu(pMenu);
}

void CPathElement::contextMenuAction(QAction * pAction)
{
    int i = pAction->data().toInt();
    CPathElement *pSplitted;
    QUndoCommand * pUndo;

    switch (i)
    {
        case -100 : //Add vertex
            pUndo = new CUndoPropChange(scene()->selectedItems());
            pUndo->setText("Add element vertex");
            addVertex(m_MouseClickedLocal);
            dynamic_cast<CUndoPropChange*> (pUndo)->finishPropChange();
            g_UndoStack->push(pUndo);
            break;
        case -101 : //Remove vertex
            pUndo = new CUndoPropChange(scene()->selectedItems());
            pUndo->setText("Remove element vertex");
            removeVertex(m_MouseClickedLocal);
            dynamic_cast<CUndoPropChange*> (pUndo)->finishPropChange();
            g_UndoStack->push(pUndo);
        break;
        case -102: //split element
            pSplitted = (CPathElement*) createElement(elementType());
            assert(pSplitted); // Create fails ?
            pSplitted->LoadElement(getSaveValue()); //Set properties as current
            g_Objects.registerElement(pSplitted); //Since LoadElement pushes the new one, register it again
            g_Objects.pushElement(this); // and push the current again to avoid duplicationg the id

            pUndo = new CUndoSplitElement(this);

            splitAt(m_MouseClickedLocal,pSplitted); // Now split it, and pSplitted will have a new path

            pSplitted->setPos(mapToScene(m_pPath->GetLast()->point));
            scene()->addItem(pSplitted);
            //pSplitted->update();

            dynamic_cast<CUndoSplitElement *>(pUndo)->finishSplit(pSplitted);

            scene()->removeItem(pSplitted); // will be added with the CUndoAddElement::redo(); See CUndoSplitElement
            g_UndoStack->push(pUndo);

        break;
    }
    CElement::contextMenuAction(pAction);
}

void CPathElement::removeVertex(QPoint &pos)
{
    CPathPoint *p = pointOnPos(pos); //Get where the user clicked
    //Find the parrent
    CPathPoint *pPath = m_pPath;
    if (m_pPath == p) //first point
    {
        m_pPath = m_pPath->next;
        p->next = nullptr;
        delete  p;
        return;
     }
    while (pPath)
        if (pPath->next == p)
        {
            pPath->next = p->next;
            p->next = nullptr;
            delete p;
            return;
        }
        else pPath = pPath->next;
    preparePainterPath();
    prepareGeometryChange();
    update();
}

void CPathElement::splitAt(QPoint point, CPathElement * newelement)
{
    if (!newelement)
        return;
    bool bOnVertex = false;
    CPathPoint * pPath = pointOnPos(point);
    if (!pPath)
    { //User didn't clicked on vertex. Find the line clicked, and split there
        pPath = m_pPath;
        while (pPath->next && !pointBelongsTo(point,pPath) )
            pPath = pPath->next;
    }
    else bOnVertex = true;
    if (!pPath->next)
        return; //no next point. Outside the parth, or at the last vertex

    //get delta x and delta y, to decrease the second parth of the path
    qreal dx =  point.x() - m_pPath->point.x();
    qreal dy =  point.y ()- m_pPath->point.y();

    //Compose new path and add 0,0 as a first coordinate
    CPathPoint *pTmp = new CPathPoint(0,0);
    pTmp->next = pPath->next;
    newelement->setPath(pTmp);
    pTmp = pTmp->next;
    while (pTmp)
    { //decrease coordinates of the new element
        pTmp->point.setX(pTmp->point.x() - dx);
        pTmp->point.setY(pTmp->point.y() - dy);
        pTmp = pTmp->next;
    }
    if (!bOnVertex)
    {
        pTmp = new CPathPoint(point);
        pPath->next = pTmp;
    }
    else pPath->next = nullptr;
    //At last, update geometry changes on both elements
    newelement->preparePainterPath();
    newelement->prepareGeometryChange();

    preparePainterPath();
    prepareGeometryChange();
    update();
    setSelected(false);

}
bool CPathElement::pointBelongsTo(const QPoint point, CPathPoint *pPath)
{

        QPointF leftPoint;
        QPointF rightPoint;

        // Normalize start/end to left right to make the offset calc simpler.
        if (pPath->point.x() <= pPath->next->point.x())
        {
            leftPoint   = pPath->point;
            rightPoint  = pPath->next->point;
        }
        else
        {
            leftPoint   = pPath->next->point;
            rightPoint  = pPath->point;
        }

        // If point is out of bounds, no need to do further checks.
        if (point.x() + SELECTION_FUZZINESS < leftPoint.x() || rightPoint.x()< point.x() - SELECTION_FUZZINESS)
            return false;
        else if (point.y() + SELECTION_FUZZINESS < qMin(leftPoint.y(), rightPoint.y())
                 || qMax(leftPoint.y(), rightPoint.y()) < point.y() - SELECTION_FUZZINESS)
            return false;

        double deltaX = rightPoint.x() - leftPoint.x();
        double deltaY = rightPoint.y() - leftPoint.y();

        // If the line is straight, the earlier boundary check is enough to determine that the point is on the line.
        // Also prevents division by zero exceptions.
        if (deltaX == 0 || deltaY == 0)
            return true;

        double slope        = deltaY / deltaX;
        double offset       = leftPoint.y() - leftPoint.x() * slope;
        double calculatedY  = point.x() * slope + offset;

        return point.y() - SELECTION_FUZZINESS <= calculatedY
                            && calculatedY <= point.y() + SELECTION_FUZZINESS;
}

CPathPoint * CPathElement::path()
{
    return  m_pPath;
}
void CPathElement::setPath(CPathPoint *pPath)
{
    if (m_pPath)
        delete m_pPath;
    m_pPath = pPath;
}

void  CPathElement::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_pEditedEdge)
    {
        m_pEditedEdge->point = mapFromScene(event->scenePos());
        preparePainterPath();
        prepareGeometryChange();
        update();
        return;
    }
    CElement::mouseMoveEvent(event);
}

void CPathElement::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (isSelected())
    { // Move vertices only and when an item is selected
        m_pEditedEdge = pointOnPos(mapFromScene(event->scenePos()));
        QGraphicsScene * pScene = scene();
        CAdvGraphicsView * pView = dynamic_cast<CAdvGraphicsView *>(pScene->views()[0]);
        if (m_pEditedEdge)
        { //and when is clicked on vertex
            pView->setMouseTracking(true);
            pView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
            pView->setCursorShape(CAdvGraphicsView::Precision);
            m_pUndo = new CUndoPropChange(scene()->selectedItems());
            m_pUndo->setText("Edit vertex position");
            m_MouseClickedLocal = mapFromScene(event->scenePos()).toPoint();
            return;
        }
    }
    CElement::mousePressEvent(event);
}

void  CPathElement::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_pEditedEdge)
    {
        CAdvGraphicsView * pView = dynamic_cast<CAdvGraphicsView *>(scene()->views()[0]);
        pView->setMouseTracking(false);
        pView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
        pView->setCursorShape(CAdvGraphicsView::Arrow);
        if (mapFromScene(event->scenePos()).toPoint() != m_MouseClickedLocal)
        { //Create undo, only if a vertex is moved
            dynamic_cast<CUndoPropChange*>(m_pUndo)->finishPropChange();
            g_UndoStack->push(m_pUndo);
        }
        else {
            delete  m_pUndo;
            m_pUndo = nullptr;
        }
        m_pEditedEdge = nullptr;
        return;
    }
    CElement::mouseReleaseEvent(event);
}
void CPathElement::addVertex (QPoint atItemPos)
{

    CPathPoint * p = m_pPath;
    CPathPoint * pTmp;

    while (p->next && !pointBelongsTo(atItemPos,p) )
        p = p->next;

    if (!p->next)
        return; //not found
    //insert the point
    atItemPos.setX(atItemPos.x() -10);
    atItemPos.setY(atItemPos.y() -10);
    pTmp = p->next;
    p->next = new CPathPoint(atItemPos);
    p->next->next = pTmp;
    preparePainterPath();
    prepareGeometryChange();
    update();
}
void CPathElement::repositionName()
{
    if (m_TextElement)
        m_TextElement->setPos(m_pPath->point.x()+5,m_pPath->point.y()-10);
}
void CPathElement::setNameVisibe(bool b)
{
    CElement::setNameVisibe(b);
    repositionName();
}


CPathPoint::CPathPoint (qreal x,qreal y)
{
    next = nullptr;
    point.setX(x);
    point.setY(y);

}
CPathPoint::CPathPoint (const QPointF &p)
{
    next = nullptr;
    point = p;
}

CPathPoint::~CPathPoint()
{
    if (next)
        delete next;
}
CPathPoint * CPathPoint::AddNew(const QPointF &p)
{
    CPathPoint *pnext  = GetLast();
    pnext->next = new CPathPoint(p);
    return  pnext->next;

}

CPathPoint * CPathPoint::AddNew(qreal x,qreal y)
{
    CPathPoint *pnext  = GetLast();
    pnext->next = new CPathPoint(x,y);
    return  pnext->next;

}
bool CPathPoint::RemoveLast()
{

    if (next)
    {
        if (!next->RemoveLast())
        {
            delete next;
            next = nullptr;
            return true;
        }
        return true; // I am not the last
    }
    //no next, so I am the last
    return false;
}
void CPathPoint::SetLast(const QPointF &p)
{
    CPathPoint *pnext  = GetLast();
    pnext->point = p;
}

void CPathPoint::SetLast(qreal x,qreal y)
{
    CPathPoint *pnext  = GetLast();

    pnext->point.setX(x);
    pnext->point.setY(y);
}

CPathPoint * CPathPoint::GetLast()
{
    CPathPoint *pnext = this;
    while (pnext->next)
        pnext = pnext->next;
    return pnext;
}


