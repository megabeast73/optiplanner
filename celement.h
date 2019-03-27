#ifndef CELEMENT_H
#define CELEMENT_H

#include <QGraphicsItem>
#include <qobject.h>
#include <QMouseEvent>
#include <QPen>
#include <qvariant.h>
#include <QJsonObject>


class CAdvGraphicsView;
class QMenu;

struct EProp {
    QString Name;
    QWidget *pEditor;
};

class ElementProperties : public QList<EProp *>
{
public:
    ElementProperties();
    ~ElementProperties();
};

class CElement : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    enum ElementType
    {
        StaticElement,
        Shaft,
        Pipe,
        Cable,
        Tube,
        Fiber
    };
    CElement();
    ~CElement() override;

    virtual void AddThis(CAdvGraphicsView * pView);
    bool Dirty() const;
    void setDirty(bool Dirty);
    virtual void setMouseCursor(const QString &resalias);

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual QRectF boundingRect() const override;

    virtual void ShowContexMenu();

    CAdvGraphicsView *View() const;
    void setView(CAdvGraphicsView *pView);

    virtual ElementType elementType() =0;
    virtual QString elementTypeName() { return "Undefined"; }


    long ElementID() const;
    void setElementID(long ElementID);
    void elementIsDeregistered(long ID) {}

    virtual ElementProperties * getElementProperties();
    virtual void setElementProperties(ElementProperties * pProp);

    virtual QJsonObject& getSaveValue();
    virtual void LoadElement(const QJsonObject& obj);

    QString getElementName() const;
    void setElementName(const QString &ElementName);

protected slots :
    void mPress(QMouseEvent *event);
    void mRelease(QMouseEvent *event);
    void mMove(QMouseEvent *event);
    void kPress(QKeyEvent *event);

protected:
    //Theese are when adding the element
    virtual void mousePress(QMouseEvent *event);
    virtual void mouseRelease(QMouseEvent *event);
    virtual void mouseMove(QMouseEvent *event);
    virtual void keyPress(QKeyEvent *event);

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    virtual QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

    virtual void disconnectSignals();
    virtual void connectSignals();


protected:
    bool m_Dirty;
    CAdvGraphicsView * m_pView;
    QPen m_NormalPen;
    QPen m_SelectedPen;
    QRect m_BoundingRect;

    long m_ElementID;

    QJsonObject m_Data;

    QString m_ElementName;

};

#endif // CELEMENT_H
