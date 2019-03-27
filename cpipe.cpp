#include "cpipe.h"
//#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>
#include "cadvgraphicsview.h"
#include <qgraphicsscene.h>
#include <QGraphicsSceneMouseEvent>
#include <qmenu.h>

#include <qloggingcategory.h>
Q_DECLARE_LOGGING_CATEGORY(Pipe)
Q_LOGGING_CATEGORY(Pipe, "pipe ")


CPipe::CPipe() :CPathElement ()
{

    m_NormalPen.setColor(Qt::white);
    m_NormalPen.setBrush(QBrush(Qt::white,Qt::SolidPattern));
    m_NormalPen.setStyle(Qt::SolidLine);
    m_NormalPen.setWidth(2);

    m_SelectedPen.setColor(Qt::yellow);
    m_SelectedPen.setBrush(QBrush(Qt::yellow, Qt::SolidPattern));//Dense4Pattern));
    m_SelectedPen.setStyle(Qt::DashLine);
    m_SelectedPen.setWidth(10);

    setPipeType(Pipef110);

}

CPipe::~CPipe()
{
}
void CPipe::AddThis(CAdvGraphicsView * pView)
{
    CPathElement::AddThis(pView);
    pView->scene()->addItem(this);
}

QRectF CPipe::boundingRect() const
{
    if (m_Dirty || isSelected())
        return CPathElement::boundingRect();
    else
        return m_PipePath.controlPointRect();
}
void CPipe::calcPar(QPointF &A, QPointF &B,int offset)
{
    qreal l;
    l = pow (B.x() - A.x(),2);
    l = l + pow(B.y() - A.y(),2);
    l = sqrt(l);
    qreal v = B.y()-A.y();
    qreal h = A.x()-B.x();

    A.setX(A.x() + offset * v/l);
    B.setX(B.x() + offset *  v/l);

    A.setY(A.y()+offset * h /l);
    B.setY(B.y() + offset *  h/l);

    m_Length = m_Length + l; // calculate the length in pixels
}

void CPipe::paintFirst()
{
    //calc first parallel

     QPointF startA,endA;
     QPointF startB,endB;
     QPointF isec;

     CPathPoint * ppoint = m_pPath;


     while (ppoint->next)
     {
        startA = ppoint->point;
        endA = ppoint->next->point;

        calcPar(startA,endA,m_pipeR);

        if (!isec.isNull()) //start line has not interception point.
            startA = isec;  //Start drawing at interception point

        if (ppoint->next->next)
        { // Not the end. Get next
            startB = ppoint->next->point;
            endB = ppoint->next->next->point;
            //Calc next line
            calcPar(startB,endB,m_pipeR);

            //find intersection point, and set as end of the current and beggining of then next
            isec = intersection(startA,endA,startB,endB);
            endA =isec;
         }
        m_PipePath.moveTo(startA);
        m_PipePath.lineTo(endA);
        m_ShapePath.lineTo(startA);
        m_ShapePath.lineTo(endA);
        ppoint = ppoint->next;
     }
    m_ShapePath.lineTo(ppoint->point.x(),ppoint->point.y());
}

void CPipe::paintSecond()
{
    //draw second parallel
    QPointF startA,endA;
    QPointF startB,endB;
    QPointF isec;

    CPathPoint * ppoint = m_pPath;

    m_Length = 0; // lenght of the pipe will be calculated on CalcPar

    while (ppoint->next)
    {
       startA = ppoint->point;
       endA = ppoint->next->point;

       calcPar(startA,endA,-m_pipeR);

       if (!isec.isNull()) //start line has not interception point.
           startA = isec;  //Start drawing at interception point

       if (ppoint->next->next)
       { // Not the end. Get next
           startB = ppoint->next->point;
           endB = ppoint->next->next->point;
           //Calc next line
           calcPar(startB,endB,-m_pipeR);

           //find intersection point, and set as end of the current and beggining of then next
           isec = intersection(startA,endA,startB,endB);
           endA =isec;
        }

        m_PipePath.moveTo(startA);
        m_PipePath.lineTo(endA);

        m_ShapePath.lineTo(startA);
        m_ShapePath.lineTo(endA);
       ppoint = ppoint->next;
    }
    m_ShapePath.lineTo(ppoint->point.x(),ppoint->point.y());
}



void CPipe::makePathElement()
{
    m_PipePath = QPainterPath();
    m_ShapePath = QPainterPath();

    m_PipePath.moveTo(m_pPath->point);
    m_PipePath.addEllipse(m_pPath->point,m_pipeR,m_pipeR);

    m_ShapePath.moveTo(m_pPath->point.x(),m_pPath->point.y());

    paintFirst();
    m_ShapePath.moveTo(m_pPath->point.x(),m_pPath->point.y());
    m_PipePath.moveTo(m_pPath->point);
    paintSecond();

    CPathPoint * p = m_pPath;
    while (p->next)
        p = p->next;
    m_PipePath.moveTo(p->point);
    m_PipePath.addEllipse(p->point,m_pipeR,m_pipeR);

    QString strTip ("Pipe \n Diameter: ");
    strTip.append(QString::number(m_pipeR * 20,'f',2)); //radius*2*10 in mm
    strTip.append(" mm\n Length: ");
    strTip.append(QString::number(m_Length/100,'f',2));
    strTip.append (" m.");
    setToolTip(strTip);
}



void CPipe::mousePress(QMouseEvent *event)
{

    CPathElement::mousePress(event);
    switch (event->button())
    {
        case Qt::RightButton :
            if (m_pPath)
                makePathElement();
            break;

        default:
        ;
    }

}
void CPipe::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    bool bTmp = m_EditingEdge;
    CPathElement::mousePressEvent(event);

    if (bTmp && !m_EditingEdge)
    {
        event->setAccepted(true);
        makePathElement();
        update();
    }
}


QPainterPath CPipe::shape() const
{

    return m_ShapePath.simplified();
}
void CPipe::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    if (m_Dirty)
    {
        CPathElement::paint(painter,option,widget);
        return;
    }
    if (isSelected())
    {
        CPathElement::paint(painter,option,widget);
        QPen spen(m_SelectedPen);
        spen.setWidth(0);
        painter->setPen(spen);
        //QPainterPath pp = shape();

        return;
    }

    painter->setPen(m_NormalPen);
    painter->drawPath(m_PipePath);
    //painter->drawRect(boundingRect());

}



qreal CPipe::getLength() const
{
    return m_Length;
}



void CPipe::ShowContexMenu()
{
    if (m_Dirty || m_EditingEdge)
        return;

    m_MousePos = QCursor::pos();
    m_MousePos = m_pView->mapFromGlobal(m_MousePos);
    m_MousePos = m_pView->mapToScene(m_MousePos).toPoint();
    m_MousePos = mapFromScene(m_MousePos).toPoint();
    CPathPoint * p = clickedOnEdge(m_MousePos);
    int iSelPipes =0;
    QList<QGraphicsItem *> pSelection = m_pView->scene()->selectedItems();
    for (int i = pSelection.count() -1; i >=0; i--)
        if (dynamic_cast<CElement *>(pSelection.at(i))->elementType() == Pipe)
            iSelPipes++;


    setSelected(true);

    QMenu * pMenu = new QMenu("Pipe");
    QAction *a = pMenu->addAction("Add vertex");
    a->setData(QVariant(1));
    a->setEnabled(p == nullptr);

    a = pMenu->addAction("Remove vertex");
    a->setData(QVariant(2));
    a->setEnabled(p != nullptr && m_pPath->next->next); //Enabled if clicked on edge or there is at least 2 points

    pMenu->addAction("Insert connector")->setData(QVariant(4));

    a = pMenu->addAction("Split on two pipes");
    a->setData(QVariant(11));
    if ( !p )
        a->setEnabled(true);
    else
        a->setEnabled( p->next && m_pPath != p);
    pMenu->addSeparator();

    QMenu * pSub = pMenu->addMenu("Pipe type");
    pSub->addAction("110mm")->setData(QVariant(5));
    pSub->addAction("50mm")->setData(QVariant(6));
    pSub->addAction("32mm")->setData(QVariant(7));
    pSub->addAction("20mm")->setData(QVariant(8));

    pSub = pMenu->addMenu("Connect");
    a = pSub->addAction("Pipes w/o connector");
    a->setData(QVariant(9));
    a->setEnabled(iSelPipes > 0);

    a = pSub->addAction("Pipes with  connector");
    a->setData(QVariant(10));
    a->setEnabled(iSelPipes > 0);

    pSub->addAction("To shaft")->setData(QVariant(3));
    pMenu->addAction("Properties...");
    pipeAction(pMenu->exec(QCursor::pos()));
    delete pMenu;
}

void CPipe::pipeAction(QAction * action)
{

    if (!action)
        return; // ESC key
    switch (action->data().toInt()) {
        case 1:
             addVertex(m_MousePos);
        break;
        case 2:
            removeVertex(m_MousePos);
        break;
        case 5:
            setPipeType(Pipef110);
        break;
        case 6:
            setPipeType(Pipef50);
        break;
        case 7:
            setPipeType(Pipef32);
        break;
        case 8:
            setPipeType(Pipef20);
        break;
        case 11:
            splitAt(m_MousePos,nullptr);
        break;

        case 9:
            connectWithSelected();
        break;

    }
    makePathElement();
    prepareGeometryChange();
    update();

}
void CPipe::splitAt(QPoint point, CPathElement * newelement)
{
    newelement = new CPipe;
    newelement->setDirty(false);
    newelement->setView(m_pView);
    CPathElement::splitAt(point,newelement);
    m_pView->scene()->addItem(newelement);
    newelement->setPos(mapToScene(m_pPath->GetLast()->point));
    dynamic_cast<CPipe *>(newelement)->setPipeType(getPipeType());
    dynamic_cast<CPipe *>(newelement)->makePathElement();
    //newelement->setSelected(true);
    setSelected(false);

}

CPipe::PipeType CPipe::getPipeType() const
{
    return m_PipeType;
}

void CPipe::setPipeType(const PipeType &PipeType)
{
    m_PipeType = PipeType;
    switch (PipeType) {
    case Pipef110:
        m_pipeR = 5.5;
        break;
    case Pipef32:
        m_pipeR = 1.6;
        break;
    case Pipef20:
        m_pipeR = 1;
        break;
    case Pipef50:
        m_pipeR = 2.5;
        break;
    case PipeCustom:
            ;

    }
}

QJsonObject& CPipe::getSaveValue()
{
    m_Data.insert("pipe_tp",QJsonValue(getPipeType()));
    return CPathElement::getSaveValue();
}

void CPipe::LoadElement(const QJsonObject& obj)
{
    CPathElement::LoadElement(obj);
    setPipeType( (PipeType) obj["pipe_tp"].toInt());
    makePathElement();

}
