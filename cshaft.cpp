#include "cshaft.h"
#include <qpainter.h>
#include "cadvgraphicsview.h"
#include <qgraphicsscene.h>
#include <qmenu.h>
#include <qcombobox.h>
#include "undocommands.h"
#include "globals.h"
#include "cpipe.h"



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
    m_ElementType = Shaft;
    m_BoundingRect.setX(0);
    m_BoundingRect.setY(0);

    m_NormalPen.setColor(Qt::white);
    m_NormalPen.setBrush(QBrush(Qt::white,Qt::SolidPattern));
    m_NormalPen.setStyle(Qt::SolidLine);
    m_NormalPen.setWidth(3);

    m_SelectedPen.setColor(Qt::yellow);
    m_SelectedPen.setBrush(QBrush(Qt::yellow,Qt::SolidPattern));//Dense4Pattern));
    m_SelectedPen.setStyle(Qt::DashLine);
    m_SelectedPen.setWidth(3);
}
CShaft::~CShaft ()
{

}


void CShaft::setShaftType(const ShaftType &ShaftType)
{
    QString Tip("Shaft/Pit \n Type: ");

    prepareGeometryChange();
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
    definePainterPath();
}

 void CShaft::definePainterPath()
 {

     QPoint  center;
     m_PainterPath = QPainterPath();


     m_PainterPath.addRect(m_BoundingRect);
     m_PainterPath.moveTo(2,2);

     switch (m_ShaftType)
     {
     case SingleShaft:
        m_PainterPath.lineTo(SINGLE_SHAFT_WIDTH-2,SINGLE_SHAFT_HEIGHT-2);
        m_PainterPath.moveTo(2,SINGLE_SHAFT_HEIGHT-2);
        m_PainterPath.lineTo(SINGLE_SHAFT_WIDTH-2,2);
        break;
     case DoubleShaft:
        m_PainterPath.lineTo(DOUBLE_SHAFT_WIDHT -2,DOUBLE_SHAFT_HEIGHT/2 -1); // \

        m_PainterPath.moveTo(DOUBLE_SHAFT_WIDHT-2,DOUBLE_SHAFT_HEIGHT/2);
        m_PainterPath.lineTo(4,DOUBLE_SHAFT_HEIGHT/2); // -

        m_PainterPath.moveTo(4,DOUBLE_SHAFT_HEIGHT/2+2);
        m_PainterPath.lineTo(DOUBLE_SHAFT_WIDHT-2,DOUBLE_SHAFT_HEIGHT-2); // \

        m_PainterPath.moveTo(4,DOUBLE_SHAFT_HEIGHT-1);
        m_PainterPath.lineTo(DOUBLE_SHAFT_WIDHT-2,DOUBLE_SHAFT_HEIGHT/2-1); // /

        m_PainterPath.moveTo(4,DOUBLE_SHAFT_HEIGHT/2-2);
        m_PainterPath.lineTo(DOUBLE_SHAFT_WIDHT-2,2); // /
         break;
     case RoundedShaft:
         center = m_BoundingRect.center().toPoint();
         m_PainterPath.addEllipse(center,ROUNDED_SHAFT_HEIGHT /4,ROUNDED_SHAFT_WIDTH /4);
         m_PainterPath.addEllipse(center,5,5);
         break;
     }
 }

void CShaft::configureContextMenu(QMenu *pMenu)
{


    QMenu * pSub = pMenu->addMenu("Shaft type");
    pSub->addAction(SINGLE_NAME)->setData(QVariant(1));
    pSub->addAction(DOUBLE_NAME)->setData(QVariant(2));
    pSub->addAction(ROUNDED_NAME)->setData(QVariant(3));
    pMenu->addAction("Properties...")->setData(QVariant(4));
    pMenu->addSeparator();
    CElement::configureContextMenu(pMenu);



}

void CShaft::contextMenuAction(QAction * pAction)
{
    if (!pAction)
        return;
    CUndoPropChange *pUndo = new CUndoPropChange(scene()->selectedItems());
    pUndo->setText("Change shaft type");
    int ix = pAction->data().toInt();
    switch (ix)
    {
    case 1 :
        setShaftType(SingleShaft);
        pUndo->finishPropChange();
        g_UndoStack->push(pUndo);
        break;
    case 2:
        setShaftType(DoubleShaft);
        pUndo->finishPropChange();
        g_UndoStack->push(pUndo);
        break;
    case 3:
        setShaftType(RoundedShaft);
        pUndo->finishPropChange();
        g_UndoStack->push(pUndo);
        break;

    }
    CElement::contextMenuAction(pAction);

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
bool CShaft::isConnectionAccepted(CElement *other)
{
    if (other->elementType() == Pipe)
    {
        CPipe * pOther = (CPipe *) other;
        QRectF rectSelf = mapToScene(m_BoundingRect).boundingRect();
        CPathPoint * pPath = pOther->path();
        QPointF pipePoint = pOther->mapToScene(pPath->point);
        if (rectSelf.contains(pipePoint)) //Check the start of the pipe
            return true;
        pipePoint = pOther->mapToScene(pPath->GetLast()->point);
        return rectSelf.contains(pipePoint); //Check the end of the pipe
    }
    return false;
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

    int tp = dynamic_cast<QComboBox *>(pProp->byName("Shaft type")->pEditor)->currentIndex();
    setShaftType((ShaftType) tp);
    definePainterPath();
    CElement::setElementProperties(pProp); //Call base class member. This will calls prepareGeometryChange and Update
 }

