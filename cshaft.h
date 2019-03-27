#ifndef CSHAFT_H
#define CSHAFT_H
#include "celement.h"
#include <qpen.h>
#include <QMouseEvent>
#include <qobject.h>


class CShaft : public CElement
{
    Q_OBJECT
public:
    enum ShaftType
    {
        SingleShaft,
        DoubleShaft,
        RoundedShaft,
        CustomShaft
    };

    CShaft();
    virtual ~CShaft();

    void setShaftType(const ShaftType &ShaftType);
    ShaftType shaftType() { return  m_ShaftType; }

    //Pure virtuals
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual void AddThis(CAdvGraphicsView * pView) override;
    virtual QRectF boundingRect() const override;

    virtual void ShowContexMenu() override;

    virtual ElementType elementType() override {return Shaft;}
    virtual QString elementTypeName() { return "Shaft"; }

    virtual QJsonObject& getSaveValue() override;
    virtual void LoadElement(const QJsonObject& obj) override;

    virtual ElementProperties * getElementProperties() override;
    virtual void setElementProperties(ElementProperties * pProp) override;

protected:
    //Theese are when adding the element
    virtual void mousePress(QMouseEvent *event) override;
    virtual void mouseRelease(QMouseEvent *event) override;
    virtual void mouseMove(QMouseEvent *event) override;

    //slots
    void actionShaftType(QAction * action);







protected:
    ShaftType m_ShaftType;
    QRectF m_BoundingRect;

    QPen m_MiddlePen;


};

#endif // CSHAFT_H
