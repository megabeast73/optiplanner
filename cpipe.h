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

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual QRectF boundingRect() const override;
    virtual QPainterPath shape() const override;

    virtual void AddThis(CAdvGraphicsView * pView) override;

    virtual void paintFirst();
    virtual void paintSecond();

    virtual void calcPar(QPointF &A, QPointF &B,int offset);
    virtual void makePathElement();

    virtual void splitAt(QPoint point, CPathElement * newelement) override;

    qreal getLength() const;

    virtual void ShowContexMenu() override;

    PipeType getPipeType() const;
    void setPipeType(const PipeType &PipeType);

    virtual ElementType elementType() override {return Pipe;}
    virtual QString elementTypeName() { return "Pipe"; }

    virtual QJsonObject& getSaveValue() override;
    virtual void LoadElement(const QJsonObject& obj) override;

protected:
    virtual void mousePress(QMouseEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void pipeAction(QAction * action);


protected:
    qreal m_pipeR;
    QPainterPath m_PipePath;
    QPainterPath m_ShapePath;
    qreal m_Length;
    PipeType m_PipeType;

    QPoint m_MousePos;

};

#endif // CPIPE_H
