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
    virtual ~CShaft() override;

    void setShaftType(const ShaftType &ShaftType);
    ShaftType shaftType() { return  m_ShaftType; }
    virtual QString elementTypeName() override { return "Shaft"; }

    virtual void definePainterPath();
    //Related to Save, Load and Undo
    virtual QJsonObject& getSaveValue() override;
    virtual void LoadElement(const QJsonObject& obj) override;

    virtual bool isConnectionAccepted(CElement *other) override;

    virtual ElementProperties *getElementProperties() override;
    virtual void setElementProperties(ElementProperties * pProp) override;

protected:
    virtual void configureContextMenu(QMenu *pMenu) override;
    virtual void contextMenuAction(QAction * pAction) override;

protected:
    ShaftType m_ShaftType;


};

#endif // CSHAFT_H
