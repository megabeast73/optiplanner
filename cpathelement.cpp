#include "cpathelement.h"
#include <qpainter.h>
#include <cadvgraphicsview.h>
#include <QGraphicsSceneMouseEvent>
#include <qjsonarray.h>

CPathElement::CPathElement() : CElement ()
{
    m_pPath = nullptr;
    m_pCurPoint = nullptr;
    m_EditingEdge = false;

}

CPathElement::~CPathElement()
{
    if (m_pPath)
        delete m_pPath;
}


void CPathElement::AddThis(CAdvGraphicsView * pView)
{
    CElement::AddThis(pView);
    //pView->scene()->addItem(this);
    setPos(0,0);
    pView->setCursorShape(CAdvGraphicsView::Precision);
}
void CPathElement::AddPoint(qreal x,qreal y)
{
    if (!m_pPath)
        m_pPath = new CPathPoint(x,y);
    else
        m_pPath->AddNew(x,y);
}


void CPathElement::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!m_pPath)
        return;

    painter->setPen(m_SelectedPen);
    CPathPoint * ppoint = m_pPath;
    while (ppoint->next)
    {
        painter->drawLine(ppoint->point,ppoint->next->point);
        ppoint = ppoint->next;
    }
}
void CPathElement::keyPress(QKeyEvent *event)
{

    if (event->key() == Qt::Key_Escape && m_Dirty)
    {
        if (m_pPath && m_pPath->next == nullptr) //Cease element insertion
        {
           CElement::keyPress(event);
            m_EditingEdge = false;
            m_pCurPoint = nullptr;

        }
        else {
            if (m_EditingEdge)
            { // return saved point
                m_pCurPoint->point = m_SavedPoint;
                CElement::keyPress(event);
                m_pCurPoint = nullptr;
                m_EditingEdge = false;
                m_Dirty = false; //set to false, when editing edge
            }
            else {
                m_pPath->RemoveLast();
                m_pCurPoint = m_pPath->GetLast();
            }
        }
        prepareGeometryChange();
        update();

    }

}
void CPathElement::mousePress(QMouseEvent *event)
{
    QPointF pos = event->pos();
            pos = m_pView->mapToScene(pos.toPoint());
    switch( event->button() )
    {
        case Qt::LeftButton:
        if (!m_pPath)
        {
            setPos(pos); //move the element where is the first click
            m_startx = pos.x(); //keep the offset, required when adding next points
            m_starty = pos.y();
            m_pPath = new CPathPoint(0,0);
            m_pView->setMouseTracking(true);
            m_pView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        }
        if (!m_EditingEdge)
            m_pCurPoint = m_pPath->AddNew(pos.x() - m_startx,pos.y() - m_starty);
        break;


//        case Qt::RightButton :

//            m_Dirty = m_pPath == nullptr;
//            m_Dirty = !m_Dirty && m_pPath->next == nullptr; //Not dirty, if we have at leat two points
//            m_pView->setMouseTracking(false);
//            disconnectSignals();
//            m_pView->setCursorShape(CAdvGraphicsView::Arrow);
//            m_pView->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
//            m_pCurPoint = nullptr;
//            m_EditingEdge = false;
//            setSelected(false);
    } //switch
    prepareGeometryChange();
    update();
}
void CPathElement::mouseRelease(QMouseEvent *event)
{
    QPointF pos = event->pos();
            pos = m_pView->mapToScene(pos.toPoint());
    switch( event->button() )
    {
    case Qt::RightButton :

        m_Dirty = m_pPath == nullptr;
        m_Dirty = !m_Dirty && m_pPath->next == nullptr; //Not dirty, if we have at leat two points
        m_pView->setMouseTracking(false);
        disconnectSignals();
        m_pView->setCursorShape(CAdvGraphicsView::Arrow);
        m_pView->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
        m_pCurPoint = nullptr;
        m_EditingEdge = false;
        event->setAccepted(true);
    }
    prepareGeometryChange();
    update();
}

void CPathElement::mouseMove(QMouseEvent *event)
{
    if (!m_pPath)
        return;
    QPointF pos = m_pView->mapToScene(event->pos());
    if (m_pCurPoint)
    {
        m_pCurPoint->point.setX(pos.x() - m_startx);
        m_pCurPoint->point.setY(pos.y() - m_starty);
    }

    prepareGeometryChange();
    update();
}


void CPathElement::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    CElement::mousePressEvent(event);
     switch  (event->button())
    {
        case Qt::LeftButton :
            if (!m_Dirty && ! m_EditingEdge)
                m_pCurPoint = clickedOnEdge(event->pos());
            if (m_pCurPoint)
            {
                m_SavedPoint = m_pCurPoint->point;
                m_EditingEdge =  true; //m_pCurPoint->next != nullptr; // Clicked on last?
                m_Dirty = true;

            };
            if(m_Dirty)
            { //Clicked on edge. Put the item into editing mode
                m_startx = pos().x();
                m_starty = pos().y();
                connectSignals();
                m_pView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
                m_pView->setMouseTracking(true);
                //save position, if the user preses esc key
                prepareGeometryChange();
                event->setAccepted(true);
            }
            break;
        default :
            ;
    }
}

CPathPoint *CPathElement::Path() const
{
    return m_pPath;
}

void CPathElement::setPath(CPathPoint *pPath)
{
    m_pPath = pPath;
}


QRectF CPathElement::boundingRect() const
{
    qreal x =0,y=0,x1=1,y1=1;
    qreal spWidth = m_SelectedPen.width();

    CPathPoint * ppoint = m_pPath;//->next;
    if (ppoint)
    {
        x = 0;
        y = 0;
        x1= x;
        y1 = x;
        while (ppoint)
        {
            if (ppoint->point.x() < x)
                x = ppoint->point.x();
            if (ppoint->point.x()>x1)
                x1 = ppoint->point.x();

            if (ppoint->point.y() < y)
                y = ppoint->point.y();
            if (ppoint->point.y()>y1)
                y1 = ppoint->point.y();

            ppoint = ppoint->next;
        }
    }
    return QRectF(x - spWidth,y - spWidth,x1 + abs(x) + spWidth*2,y1 + abs(y) + spWidth*2);
}


CPathPoint * CPathElement::clickedOnEdge(QPointF pos)
{
    QPointF pos1 = pos;//mapFromScene(pos);// m_pView->mapToScene(pos);
    QRectF sel(pos1.x()-10,pos1.y()-10,20,20); //pos1.x()+20,pos1.y()+20);
    CPathPoint * p = m_pPath;
    m_pCurPoint = nullptr;
    while ((m_pCurPoint == nullptr) && p)
        if (sel.contains(p->point))
                return p;
        else
            p = p->next;

   return nullptr;

}
QPointF CPathElement::intersection(const QPointF &l1A, const QPointF &l1B,
                             const QPointF &l2A, const QPointF &l2B)
{
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

        // Check calculated Y matches the points Y coord with some easing.
        return point.y() - SELECTION_FUZZINESS <= calculatedY
                            && calculatedY <= point.y() + SELECTION_FUZZINESS;
}

void CPathElement::addVertex (QPoint &atItemPos)
{

    CPathPoint * p = m_pPath;
    CPathPoint * pTmp;

    while (p->next && !pointBelongsTo(atItemPos,p) )
        p = p->next;

    if (!p->next)
        return; //not found
    //insert the point
    pTmp = p->next;
    p->next = new CPathPoint(atItemPos);
    p->next->next = pTmp;
    //Start moving point
    QGraphicsSceneMouseEvent event;
    event.setPos(atItemPos);
    event.setButton(Qt::LeftButton);
    mousePressEvent(&event);
}

void CPathElement::removeVertex(QPoint &pos)
{
    CPathPoint *p = clickedOnEdge(pos); //Get where the user clicked
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
}

void CPathElement::splitAt(QPoint point, CPathElement * newelement)
{
    if (!newelement)
        return;
    bool bOnVertex = false;
    CPathPoint * pPath = clickedOnEdge(point);
    if (!pPath)
    {
        pPath = m_pPath;
        while (pPath->next && !pointBelongsTo(point,pPath) )
            pPath = pPath->next;
    }
    else bOnVertex = true;
    if (!pPath->next)
        return; //not found or at the end

    qreal dx = /*pPath->next->point.x()*/ point.x() - m_pPath->point.x();
    qreal dy = /*pPath->next->point.y()*/ point.y ()- m_pPath->point.y();

    CPathPoint *pTmp = new CPathPoint(0,0);
    pTmp->next = pPath->next;
    newelement->setPath(pTmp);
    pTmp = pTmp->next;
    while (pTmp)
    {
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

    setSelected(false);

}
void CPathElement::connectWith(CPathElement * pOther)
{
    CPathPoint * pOtherPoint = pOther->Path();
    CPathPoint * pThisLast = m_pPath->GetLast();
    CPathPoint * pOtherLast = pOtherPoint->GetLast();
    CPathPoint * pTmp;

    QPointF startMe(mapToScene(m_pPath->point));
    QPointF endMe(mapToScene(pThisLast->point));
    QPointF startO(pOther->mapToScene(pOtherPoint->point));
    QPointF endO(pOther->mapToScene(pOtherLast->point));
    qreal diff1 = abs(startO.x() - endMe.x());
    qreal diff2 = abs(endO.x() - startMe.x());

    if (diff2 < diff1)
    { //Other is at the left
        pTmp =m_pPath;
        while (pTmp)
        {
            pOtherPoint->AddNew(pOther->mapFromScene(mapToScene(pTmp->point)));
            pTmp = pTmp->next;
        }

        delete m_pPath;
        m_pPath = pOtherPoint;
        setPos(pOther->pos());
        pOther->setPath(nullptr);
    }
    else {
            pTmp = pOtherPoint;
            while (pTmp)
            {
                m_pPath->AddNew(mapFromScene(pOther->mapToScene(pTmp->point)));
                pTmp = pTmp->next;
            }
        }


}

void CPathElement::connectWithSelected()
{
    QList<QGraphicsItem *> pSelection = m_pView->scene()->selectedItems();
    CElement * p;
    for (int i = pSelection.count() -1; i >=0; i--)
    {
        p = dynamic_cast<CElement *>(pSelection.at(i));
        if ( p != this && p->elementType() == elementType())
            connectWith((CPathElement *) p);
    }
    for (int i = pSelection.count() -1; i >=0; i--)
    {
        p = dynamic_cast<CElement *>(pSelection.at(i));
        if ( p != this && p->elementType() == elementType())
        {
            p->setSelected(false);
            m_pView->scene()->removeItem(p);
            delete p;
        }
    }
}


QJsonObject& CPathElement::getSaveValue()
{
    CPathPoint *p = m_pPath;
    unsigned int cnt =0;
    QString index_name;

    QJsonArray point_arr;
    QJsonValue point_x,point_y,point_v;


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

  //  QJsonArray point_arr;
    QJsonValue point_x,point_y,point_v;

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


