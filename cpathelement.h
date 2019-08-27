#ifndef CPATHELEMENT_H
#define CPATHELEMENT_H
#include "celement.h"
#include <QPointF>

class QUndoCommand;

class CPathPoint
{
public:
    CPathPoint (qreal x,qreal y);
    CPathPoint (const QPointF &p);
    virtual ~CPathPoint();

    CPathPoint * AddNew(qreal x,qreal y);
    CPathPoint * AddNew(const QPointF &p);

    virtual bool RemoveLast();
    virtual void SetLast(const QPointF &p);
    virtual void SetLast(qreal x,qreal y);
    virtual CPathPoint * GetLast();

    QPointF point;
    CPathPoint * next;
};

class CPathElement : public CElement
{
public:
    CPathElement();
    ~CPathElement();
    virtual void AddPoint(qreal x,qreal y);

    virtual QRectF boundingRect() const override;

    virtual void preparePainterPath();
    virtual QPointF intersection(const QPointF &l1A, const QPointF &l1B,
                                 const QPointF &l2A, const QPointF &l2B);

    //GetSaveValue will clear m_Data. Call it before your implementation
    virtual QJsonObject& getSaveValue() override;
    virtual void LoadElement(const QJsonObject& obj) override;

    void setPath(CPathPoint *pPath);
    virtual CPathPoint * path();

    virtual void keyPress(QKeyEvent *keyEvent) override;
    virtual void keyRelease(QKeyEvent *keyEvent) override;

    virtual void mouseMove(QGraphicsSceneMouseEvent *mouseEvent) override;
    virtual void mousePress(QGraphicsSceneMouseEvent *mouseEvent) override;
    virtual void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent) override;

    virtual CPathPoint * pointOnPos(QPointF pos);
    virtual bool pointBelongsTo(const QPoint point, CPathPoint *pPath);
    virtual void addVertex (QPoint atItemPos);
    virtual void removeVertex(QPoint &atItemPos);
    virtual void splitAt(QPoint point, CPathElement * newelement);

    virtual void setNameVisibe(bool b) override;
    virtual void repositionName();


protected:
    virtual void configureContextMenu(QMenu *pMenu) override;
    virtual void contextMenuAction(QAction * pAction) override;

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;


protected:
    CPathPoint * m_pPath;
    bool m_Placed;
    CPathPoint * m_pEditedEdge;
    QUndoCommand * m_pUndo;

 };

#endif // CPATHELEMENT_H
