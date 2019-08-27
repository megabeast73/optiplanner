#ifndef CELEMENT_H
#define CELEMENT_H

#include <QGraphicsItem>
#include <qobject.h>
#include <QMouseEvent>
#include <QPen>
#include <qvariant.h>
#include <QJsonObject>
#include <QVector>




class QMenu;
class CElement;
class CTextElement;

typedef QVector<int> CONNECTIONS;
typedef QList<QGraphicsItem *> ITEMLIST;

struct EProp {
    QString Name;
    QWidget *pEditor;
};


class ElementProperties : public QList<EProp *>
{
public:
    ElementProperties();
    ~ElementProperties();

    const EProp * byName(const QString &name);
    CElement *Element() const;
    void setElement(CElement *Element);

protected:
    int  m_Element;


};


class CElement : public QObject, public QGraphicsItem
{

    Q_OBJECT

public:
    enum ElementType
    {
        Unknown,
        StaticElement,
        Shaft,
        Pipe,
        ControlCenter,
        Cable,
        Tube,
        Fiber
    };
    CElement();
    ~CElement() override;

    virtual QPainterPath shape() const override;

    long ElementID() const;
    void setElementID(long ElementID);

    virtual ElementType elementType() { return m_ElementType; }//=0;
    virtual QString elementTypeName() { return "Undefined"; }

    virtual QJsonObject& getSaveValue();
    virtual void LoadElement(const QJsonObject& obj);


    QString getElementName() const;
    virtual void setElementName(const QString &ElementName);
    virtual void setNameVisibe(bool b);
    virtual bool getNameVisible() { return m_TextElement != nullptr; }


    //Pure virtuals
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual QRectF boundingRect() const override;

    //CAdvGraphicsScene will call these methods when adding the item
    virtual void keyPress(QKeyEvent *keyEvent);
    virtual void keyRelease(QKeyEvent *keyEvent);

    virtual void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);

    //Connections to other elements
    virtual void clearAllConnections();
    virtual void connectElement(CElement *other);  //calls other->addConnection()
    virtual void disconnectElement(CElement *other); //calls other->clearConnection()
    // Just adds/removes the otherId from m_Connections
    virtual void addConnection(int otherId);
    virtual void clearConnection(int otherId);

    virtual ITEMLIST intersectedElements(); //returns the list of elements under boundingRect()
    virtual void setConnectedElements(const ITEMLIST &elements); //uses connectElement() to setAllConnections
    virtual bool isConnectionAccepted(CElement *other); //Override to return if the element is accepted for connection
                                                          // CElement::isConnectionAccepted() returns always true
    virtual void reconnectElement();

    //Properties editor
    virtual ElementProperties * getElementProperties();
    //Call setElementProperties at the end of your implementation,
    // since it calls prepareGeometryChange(), update() and reconnectElement()
    virtual void setElementProperties(ElementProperties * pProp);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    //Conext menu handlings
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    virtual void configureContextMenu(QMenu *pMenu);
    virtual void contextMenuAction(QAction * pAction);

    //Handle changes
   // virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

protected:

    ElementType m_ElementType;
    long m_ElementID;
    //bool m_ShowName;

    //used for basic drawing the element in paint method
    //Members are modified by inherited classes
    //
    QPainterPath m_PainterPath;

    QPen m_NormalPen;
    QPen m_SelectedPen;

    QRectF m_BoundingRect;

    QString m_ElementName;

    QJsonObject m_Data;
    QPoint m_MouseClickedLocal; //Context menus, etc

    CONNECTIONS m_Connections;

    CTextElement * m_TextElement;

};



#endif // CELEMENT_H
