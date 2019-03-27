#include "celement.h"
#include "cadvgraphicsview.h"
#include <QGraphicsSceneMouseEvent>

#include <QLoggingCategory>
#include <qmenu.h>
#include "globals.h"
#include <qspinbox.h>
#include <QLineEdit>


CElement::CElement() : QGraphicsItem (), QObject()
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
    setFlag(ItemIsSelectable,true);
    setFlag(ItemIsMovable,true);
    setFlag(ItemSendsGeometryChanges,true);
    m_Dirty = true;
    m_ElementID = -1;
    m_ElementName = "No name";

}

CElement::~CElement ()
{
    if (m_Dirty )
        disconnectSignals();
    g_Objects.deregisterElement(this,false);

}
void CElement::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{

    if (isSelected())
    {
        painter->setPen(m_SelectedPen);
    }
    else
    {
        painter->setPen(m_NormalPen);
    }

}
QRectF CElement::boundingRect() const
{
    return m_BoundingRect;
}

void CElement::setMouseCursor(const QString &resalias)
{
    QCursor cursor(QPixmap(resalias,nullptr,Qt::AutoColor));
    m_pView->viewport()->setCursor(cursor);
}

void CElement::AddThis(CAdvGraphicsView * pView)
{
    m_pView = pView;
    m_Dirty = true;
    pView->setActiveElement(this);
    connectSignals();
    if (m_ElementID < 0)
        g_Objects.registerElement(this);
}
void CElement::mPress(QMouseEvent *event)
{
    mousePress(event);
}

void CElement::mRelease(QMouseEvent *event)
{
        mouseRelease(event);
}

void CElement::mMove(QMouseEvent *event)
{
    mouseMove(event);
}
void CElement::kPress(QKeyEvent *event)
{
    keyPress(event);
}
void CElement::keyPress(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        disconnectSignals();
        m_pView->setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
        m_pView->viewport()->setCursor(QCursor(Qt::ArrowCursor));
        m_Dirty = true;
        setSelected(false);
    }
}

void CElement::disconnectSignals()
{
    m_pView->setInserting(false);
    disconnect(m_pView,&CAdvGraphicsView::mousePress, this, &CElement::mPress);
    disconnect(m_pView,&CAdvGraphicsView::mouseRelease, this, &CElement::mRelease);
    disconnect(m_pView,&CAdvGraphicsView::mouseMove, this, &CElement::mMove);
    disconnect(m_pView,&CAdvGraphicsView::keyPress, this, &CElement::kPress);
}
void CElement::connectSignals()
{
    m_pView->setInserting(true);
    connect(m_pView,&CAdvGraphicsView::mousePress, this, &CElement::mPress);
    connect(m_pView,&CAdvGraphicsView::mouseRelease, this, &CElement::mRelease);
    connect(m_pView,&CAdvGraphicsView::mouseMove, this, &CElement::mMove);
    connect(m_pView,&CAdvGraphicsView::keyPress, this, &CElement::kPress);
}

QString CElement::getElementName() const
{
    return m_ElementName;
}

void CElement::setElementName(const QString &ElementName)
{
    m_ElementName = ElementName;
}


long CElement::ElementID() const
{
    return m_ElementID;
}

void CElement::setElementID(long ElementID)
{
    m_ElementID = ElementID;
}

CAdvGraphicsView *CElement::View() const
{
    return m_pView;
}

void CElement::setView(CAdvGraphicsView *pView)
{
    m_pView = pView;
}

bool CElement::Dirty() const
{
    return m_Dirty;
}

void CElement::setDirty(bool Dirty)
{
    m_Dirty = Dirty;
}

void CElement::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

     QGraphicsItem::mouseReleaseEvent(event);

}
void CElement::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

        QGraphicsItem::mousePressEvent(event);
}

void CElement::mouseMove(QMouseEvent *event)
{
}
void CElement::mousePress(QMouseEvent *event)
{
}
void CElement::mouseRelease(QMouseEvent *event)
{
    event->setAccepted(true);
}

QVariant CElement::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{

    return QGraphicsItem::itemChange(change,value);
}

 void CElement::ShowContexMenu()
 {


 }

QJsonObject& CElement::getSaveValue()
{
    m_Data.insert("pos_x",QJsonValue(pos().x()));
    m_Data.insert("pos_y",QJsonValue(pos().y()));
    m_Data.insert("rot",QJsonValue(rotation()));
    m_Data.insert("e_type",QJsonValue(elementType()));
    m_Data.insert("e_id",QJsonValue((int) m_ElementID));
    m_Data.insert("e_name",QJsonValue(getElementName()));

    return m_Data;
}
void CElement::LoadElement(const QJsonObject& obj)
{
    qreal pos_x = obj["pos_x"].toDouble();
    qreal pos_y = obj["pos_y"].toDouble();
    qreal rot = obj["rot"].toDouble();
    setElementName(obj["e_name"].toString());
    setPos(pos_x,pos_y);
    setRotation(rot);
    m_ElementID = obj["e_id"].toInt();
    g_Objects.pushElement(this);
}

ElementProperties * CElement::getElementProperties()
{
    ElementProperties *pRet = new ElementProperties;
    EProp * pProp;

    pProp = new EProp;
    pProp->Name = "Type";
    pProp->pEditor = new QLineEdit;
    pProp->pEditor->setEnabled(false);
    dynamic_cast<QLineEdit*>(pProp->pEditor)->setText(elementTypeName());
    pRet->append(pProp);

    pProp = new EProp;
    pProp->Name = "Name";
    pProp->pEditor = new QLineEdit;
    dynamic_cast<QLineEdit*>(pProp->pEditor)->setText(getElementName());
    pRet->append(pProp);

    pProp = new EProp;
    pProp->Name = "Rotation";
    pProp->pEditor = new QSpinBox;
    dynamic_cast<QSpinBox*>(pProp->pEditor)->setRange(0,359);
    pRet->append(pProp);
    return pRet;

}

void CElement::setElementProperties(ElementProperties * pProp)
{
}



ElementProperties::ElementProperties() : QList<EProp *>()
{
}

ElementProperties::~ElementProperties()
{
    EProp *p;
    while (count() > 0)
    {
        p= takeAt(0);
        delete p->pEditor;
        delete p;

    }
}
