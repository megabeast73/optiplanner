#include "celement.h"
//#include "cadvgraphicsview.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

//#include <QLoggingCategory>
#include <qmenu.h>
#include "globals.h"
#include <qspinbox.h>
#include <QLineEdit>
#include <QCheckBox>
#include <cadvscene.h>
#include "undocommands.h"
#include "ctextelement.h"
#include <qcombobox.h>


CElement::CElement() : QObject(),  QGraphicsItem ()
{
    m_ElementType = Unknown;



    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
    setFlag(ItemIsSelectable,true);
    setFlag(ItemIsMovable,true);
    setFlag(ItemSendsGeometryChanges,true);
    m_ElementID = -1;
    m_ElementName = "No name";
    m_TextElement = nullptr;


    setZValue(1);
}

CElement::~CElement ()
{
    clearAllConnections();
    g_Objects.deregisterElement(this);
}

QPainterPath CElement::shape() const
{
    return m_PainterPath.simplified();
}

long CElement::ElementID() const
{
    return m_ElementID;
}

void CElement::setElementID(long ElementID)
{
    m_ElementID = ElementID;
}

QJsonObject& CElement::getSaveValue()
{
    m_Data.insert("pos_x",QJsonValue(pos().x()));
    m_Data.insert("pos_y",QJsonValue(pos().y()));
    m_Data.insert("rot",QJsonValue(rotation()));
    m_Data.insert("e_type",QJsonValue(elementType()));
    m_Data.insert("e_id",QJsonValue((int) m_ElementID));
    m_Data.insert("e_name",QJsonValue(getElementName()));
    m_Data.insert("sh_name",QJsonValue(getNameVisible()));
    m_Data.insert("ZValue",QJsonValue(zValue()));

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
    setNameVisibe(obj["sh_name"].toBool());
    setZValue(obj["ZValue"].isUndefined() ? 1 : obj["ZValue"].toInt());

    //Set the object to the collection
    g_Objects.pushElement(this);
}

QString CElement::getElementName() const
{
    return m_ElementName;
}

void CElement::setElementName(const QString &ElementName)
{
    m_ElementName = ElementName;
    if (m_TextElement)
        m_TextElement->setText(ElementName);
}

void CElement::setNameVisibe(bool b)
{
    if (m_TextElement && b)
        return;
    if (!m_TextElement && ! b)
        return;
    if (b)
    {
        m_TextElement = new CTextElement;
        m_TextElement->setText(m_ElementName);
        m_TextElement->setParentItem(this);
        m_TextElement->setPos(5,-5);
    }
    else {
        m_TextElement->setParentItem(nullptr);
        scene()->removeItem(m_TextElement);
        delete m_TextElement;
        m_TextElement = nullptr;
    }
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

    painter->drawPath(m_PainterPath);


// DEBUG see where is selection shape
//    QPen p;
//    p.setColor(Qt::red);
//    p.setBrush(QBrush(Qt::red,Qt::SolidPattern));
//    p.setStyle(Qt::SolidLine);
//    p.setWidth(1);

//    painter->setPen(p);
//    painter->drawPath(shape());

}

QRectF CElement::boundingRect() const
{
    return m_BoundingRect;
}

void CElement::keyPress(QKeyEvent *keyEvent)
{
}
void CElement::keyRelease(QKeyEvent *keyEvent)
{
    if (keyEvent->key() == Qt::Key_Escape)
        dynamic_cast<CAdvScene*>(scene())->cancelAddingElement();

}

void CElement::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    setPos(mouseEvent->scenePos().x(),mouseEvent->scenePos().y());

}
void CElement::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton)
        dynamic_cast<CAdvScene*>(scene())->finishAddingElement();
    else if (mouseEvent->button() == Qt::RightButton)
        dynamic_cast<CAdvScene*>(scene())->cancelAddingElement();
}
void CElement::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
}

void CElement::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}
void CElement::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

void CElement::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    m_MouseClickedLocal = mapFromScene(event->scenePos()).toPoint();
    QMenu context;
    configureContextMenu(&context);

    contextMenuAction(context.exec(event->screenPos()));
}

void CElement::configureContextMenu(QMenu *pMenu)
{
    //Z-Order management. All items are created with ZValue=1,
    //Except Text and Grid, which are with ZValue -1
    pMenu->addAction("Send to back")->setData(QVariant(-2));
    pMenu->addAction("Bring to front")->setData(QVariant(-3));
    pMenu->addAction("Put to middle")->setData(QVariant(-4));
    pMenu->addSeparator();
    pMenu->addAction("Delete")->setData(QVariant(-1));

    setSelected(true);
}


void CElement::contextMenuAction(QAction * pAction)
{
    if (!pAction)
        return;
    ITEMLIST selected;
    switch (pAction->data().toInt())
    {
        case -1 : //Delete selection
                dynamic_cast<CAdvScene *> (scene())->deleteSelection();
                break;
        case -2: //Send to back
                setZValue(0);
                break;
        case -3: //Bring to front
                setZValue(2);
                break;
        case -4: //Middle
                setZValue(1);
                break;

    }
}

void CElement::reconnectElement()
{
    setConnectedElements(intersectedElements());
}
void CElement::clearAllConnections()
{
    while (m_Connections.count() > 0)
        g_Objects.elementById(m_Connections.takeLast())->clearConnection(m_ElementID);
}

void CElement::connectElement(CElement *other)
{
    if (isConnectionAccepted(other) &&
        other->isConnectionAccepted(this) )
    {
        addConnection(other->ElementID());
        other->addConnection(m_ElementID);
    }
}

void CElement::disconnectElement(CElement *other)
{
    clearConnection(other->ElementID());
    other->clearConnection(m_ElementID);
}

void CElement::addConnection(int otherId)
{
    if (m_Connections.indexOf(otherId) > -1)
        return;
    m_Connections.push_back(otherId);
}

void CElement::clearConnection(int otherId)
{
    m_Connections.removeOne(otherId);
}

ITEMLIST CElement::intersectedElements()
{
    return collidingItems(Qt::IntersectsItemShape);
}

void CElement::setConnectedElements(const ITEMLIST &elements)
{
    clearAllConnections();
    for (int i = elements.count()-1; i>=0; i--)
        connectElement((CElement*) elements.at(i));
}

bool CElement::isConnectionAccepted(CElement *other)
{
    return true;
}



ElementProperties * CElement::getElementProperties()
{
    ElementProperties *pRet = new ElementProperties;
    EProp * pProp;

    pRet->setElement(this);

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
    pProp->Name = "Rot(deg)";
    pProp->pEditor = new QSpinBox;
    dynamic_cast<QSpinBox*>(pProp->pEditor)->setRange(0,359);
    qreal r = rotation();
    dynamic_cast<QSpinBox*>(pProp->pEditor)->setValue(r);
    pRet->append(pProp);

    pProp = new EProp;
    pProp->Name = "Show el.name";
    pProp->pEditor = new QCheckBox;
    dynamic_cast<QCheckBox*>(pProp->pEditor)->setCheckState( getNameVisible() ? Qt::Checked : Qt::Unchecked);
    pRet->append(pProp);

    QComboBox *tp = new QComboBox;
    pProp = new EProp;
    pProp->Name = "Z-Value";
    pProp->pEditor = tp;
    tp->setEditable(false);
    tp->addItem("Back");
    tp->addItem("Middle");
    tp->addItem("Front");
    int zv = zValue();
    tp->setCurrentIndex(zv);
    pRet->append(pProp);

    return pRet;

}


void CElement::setElementProperties(ElementProperties * pProp)
{
    qreal r = dynamic_cast<QSpinBox*>(pProp->byName("Rot(deg)")->pEditor)->value();
    setRotation( r);

    setElementName(dynamic_cast<QLineEdit*>(pProp->byName("Name")->pEditor)->text());

    bool bShName = dynamic_cast<QCheckBox *>(pProp->byName("Show el.name")->pEditor)->checkState() == Qt::Checked;
    setNameVisibe(bShName);

    int zv = dynamic_cast<QComboBox *>(pProp->byName("Z-Value")->pEditor)->currentIndex();
    setZValue(zv);

    prepareGeometryChange();
    update();
    reconnectElement();
}

//void CElement::applyElementProperties()
//{
//}


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

const EProp * ElementProperties::byName(const QString &name)
{

    for (int i = count() -1; i>= 0; i--)
        if (!at(i)->Name.compare(name))
            return at(i);
    return nullptr;
}

CElement *ElementProperties::Element() const
{
    return g_Objects.elementById(m_Element);
}

void ElementProperties::setElement(CElement *Element)
{
    m_Element = Element->ElementID();
}

