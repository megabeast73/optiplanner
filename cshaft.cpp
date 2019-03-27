#include "cshaft.h"
#include <qpainter.h>
#include "cadvgraphicsview.h"
#include <qgraphicsscene.h>
#include <qmenu.h>
#include <qcombobox.h>


#define SINGLE_SHAFT_WIDTH 90
#define SINGLE_SHAFT_HEIGHT 60
#define SINGLE_NAME "Single (90x60)"

#define DOUBLE_SHAFT_WIDHT 90
#define DOUBLE_SHAFT_HEIGHT 120
#define DOUBLE_NAME "Double (90x120)"

#define ROUNDED_SHAFT_WIDTH 160
#define ROUNDED_SHAFT_HEIGHT 160
#define ROUNDED_NAME "Big (160x160)"






CShaft::CShaft() : CElement ()
{
    setShaftType(SingleShaft);
    m_BoundingRect.setX(0);
    m_BoundingRect.setY(0);

    m_NormalPen.setColor(Qt::white);
    m_NormalPen.setBrush(QBrush(Qt::white,Qt::SolidPattern));
    m_NormalPen.setStyle(Qt::SolidLine);
    m_NormalPen.setWidth(5);

    m_SelectedPen.setColor(Qt::yellow);
    m_SelectedPen.setBrush(QBrush(Qt::yellow,Qt::SolidPattern));//Dense4Pattern));
    m_SelectedPen.setStyle(Qt::DashLine);
    m_SelectedPen.setWidth(5);

    m_MiddlePen = m_NormalPen;
    m_MiddlePen.setWidth(2);
}
CShaft::~CShaft ()
{

}


void CShaft::setShaftType(const ShaftType &ShaftType)
{
    QString Tip("Shaft/Pit \n Type: ");

    m_ShaftType = ShaftType;

    switch (ShaftType) {
    case SingleShaft:
        m_BoundingRect.setWidth(SINGLE_SHAFT_WIDTH);
        m_BoundingRect.setHeight(SINGLE_SHAFT_HEIGHT);
        Tip.append ("Single");
        break;
    case DoubleShaft:
        m_BoundingRect.setWidth(DOUBLE_SHAFT_WIDHT);
        m_BoundingRect.setHeight(DOUBLE_SHAFT_HEIGHT);
        Tip.append ("Double");
        break;
    case RoundedShaft:
        m_BoundingRect.setWidth(ROUNDED_SHAFT_WIDTH);
        m_BoundingRect.setHeight(ROUNDED_SHAFT_HEIGHT);
        Tip.append ("Big/Rounded");
        break;
    }

    Tip.append("\n Size in cm (WxH): ");
    Tip.append(QString::number(m_BoundingRect.width(),'f',2));
    Tip.append("x");
    Tip.append(QString::number(m_BoundingRect.height(),'f',2));
    setToolTip(Tip);

}

void CShaft::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{


    CElement::paint(painter,option,widget);

    QPoint  center;

    painter->drawRect(m_BoundingRect);
    painter->setPen(m_MiddlePen);
    switch (m_ShaftType)
    {
    case SingleShaft:
        painter->drawLine(4,4,SINGLE_SHAFT_WIDTH -4,SINGLE_SHAFT_HEIGHT -4);
        painter->drawLine(SINGLE_SHAFT_WIDTH-4,4,4,SINGLE_SHAFT_HEIGHT-4);
        break;
    case DoubleShaft:
            painter->drawLine(4,DOUBLE_SHAFT_HEIGHT/2 -2,DOUBLE_SHAFT_WIDHT -4,DOUBLE_SHAFT_HEIGHT/2 -2);

            painter->drawLine(4,4,DOUBLE_SHAFT_WIDHT -4,DOUBLE_SHAFT_HEIGHT/2 -2);
            painter->drawLine(4,DOUBLE_SHAFT_HEIGHT/2 -2, DOUBLE_SHAFT_WIDHT -4,4);

            painter->drawLine(4,DOUBLE_SHAFT_HEIGHT/2+2,DOUBLE_SHAFT_WIDHT-4,DOUBLE_SHAFT_HEIGHT-4);
            painter->drawLine(4,DOUBLE_SHAFT_HEIGHT-4,DOUBLE_SHAFT_WIDHT-4,DOUBLE_SHAFT_HEIGHT/2+2);


        break;
    case RoundedShaft:
        center = m_BoundingRect.center().toPoint();
        painter->drawEllipse(center,ROUNDED_SHAFT_HEIGHT /4,ROUNDED_SHAFT_WIDTH /4);
        painter->drawEllipse(center,5,5);

        break;
    }


}
void CShaft::AddThis(CAdvGraphicsView * pView)
{
    CElement::AddThis(pView);

    pView->setMouseTracking(true);
    pView->setCursorShape(CAdvGraphicsView::Precision);

}



void CShaft::mousePress(QMouseEvent *event)
{

}

void CShaft::mouseRelease(QMouseEvent *event)
{
    QPointF pos;
    Qt::MouseButtons b = event->buttons();
    if (b == Qt::RightButton)
    {
        disconnectSignals();
        m_pView->cursor().setShape(Qt::ArrowCursor);
        m_Dirty = true;
    }
    if (event->button() == Qt::LeftButton)
    {
        disconnectSignals();
        m_pView->setCursorShape(CAdvGraphicsView::Arrow);

        m_Dirty = false;
        m_pView->scene()->addItem(this);
        pos = m_pView->mapToScene(event->pos());
        pos.setX(pos.x() - m_BoundingRect.width() /2);
        pos.setY(pos.y() - m_BoundingRect.height() /2);
        setPos(pos);
    }
}

void CShaft::mouseMove(QMouseEvent *event)
{
}



QRectF CShaft::boundingRect() const
{
    QRectF ret (m_BoundingRect);
    ret.setX(ret.x()-1);
    ret.setY(ret.y()-1);
    ret.setWidth(ret.width() +2);
     ret.setHeight(ret.height() +2);
    return ret;
}

void CShaft::ShowContexMenu()
{
    if (m_Dirty)
        return;
    setSelected(true);

    QMenu * pMenu = new QMenu("Shaft");

    QMenu * pSub = pMenu->addMenu("Shaft type");
    pSub->addAction(SINGLE_NAME)->setData(QVariant(1));
    pSub->addAction(DOUBLE_NAME)->setData(QVariant(2));
    pSub->addAction(ROUNDED_NAME)->setData(QVariant(3));
    pMenu->addAction("Properties...")->setData(QVariant(4));

    actionShaftType(pMenu->exec(QCursor::pos()));
    delete pMenu;
}

void CShaft::actionShaftType(QAction * action)
{
    if (!action)
        return;
    switch (action->data().toInt()) {
        case 1:
            setShaftType(SingleShaft);
        break;
        case 2:
            setShaftType(DoubleShaft);
        break;
        case 3:
            setShaftType(RoundedShaft);
        break;

    }
    prepareGeometryChange();
    update();
}

QJsonObject& CShaft::getSaveValue()
{
    CElement::getSaveValue();
    m_Data.insert("shaft_tp",QJsonValue(m_ShaftType));
    return m_Data;
}
 void CShaft::LoadElement(const QJsonObject& obj)
{
     CElement::LoadElement(obj);
    setShaftType((ShaftType) obj["shaft_tp"].toInt());

}

 ElementProperties * CShaft::getElementProperties()
 {
     ElementProperties *pRet = CElement::getElementProperties();
     EProp * pProp;
     QComboBox *tp = new QComboBox;

     pProp = new EProp;
     pProp->Name = "Shaft type";
     pProp->pEditor = tp;
     tp->setEditable(false);
     tp->addItem(SINGLE_NAME);
     tp->addItem(DOUBLE_NAME);
     tp->addItem(ROUNDED_NAME);
     tp->setCurrentIndex(shaftType());
     pRet->append(pProp);

     return pRet;

 }

 void CShaft::setElementProperties(ElementProperties * pProp)
 {
     CElement::setElementProperties(pProp);
 }
