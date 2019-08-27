#ifndef CPIPE_H
#define CPIPE_H

#include <QObject>
#include "cpathelement.h"
#include <QGraphicsSceneContextMenuEvent>

class CAdvGraphicsView;

class CPipe : public CPathElement
{
public:
    enum PipeType {
        Pipef110,
        Pipef50,
        Pipef32,
        Pipef20,
        PipeCustom
    };

    CPipe();
    virtual ~CPipe() override;

    PipeType getPipeType() const;
    void setPipeType(const PipeType &PipeType);

    virtual ElementType elementType() override {return Pipe;}
    virtual QString elementTypeName() override { return "Pipe"; }

    virtual ElementProperties *getElementProperties() override;
    virtual void setElementProperties(ElementProperties * pProp) override;


    //Drawing related
    virtual void preparePainterPath() override;
    //Draw parrallels of the main path
    virtual void paintFirst();
    virtual void paintSecond();
    //Calcs length of the parrallels depending of their intersections
    virtual void calcPar(QPointF &A, QPointF &B,int offset);

    //Returns the selection shape, generated within preparePainterPath, paintFirst and paintSectond
    virtual QPainterPath shape() const override;
//    virtual bool contains(const QPointF &point) const override;

    //Save & Load
    virtual QJsonObject& getSaveValue() override;
    virtual void LoadElement(const QJsonObject& obj) override;

    virtual bool isConnectionAccepted(CElement *other) override;

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
    //Conext menu handlings
    virtual void configureContextMenu(QMenu *pMenu) override;
    virtual void contextMenuAction(QAction * pAction) override;

protected:
    qreal m_pipeR;
    PipeType m_PipeType;
    qreal m_Length;
    QPainterPath m_Shape;

};

#endif // CPIPE_H
