#ifndef CPATHELEMENT_H
#define CPATHELEMENT_H
#include "celement.h"
#include <QPointF>

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

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual QRectF boundingRect() const override;
    virtual void AddThis(CAdvGraphicsView * pView) override;

    virtual void AddPoint(qreal x,qreal y);
    virtual CPathPoint * clickedOnEdge(QPointF pos);
    //virtual void RemoveEdge(CPathPoint * edge);
    //virtual void InsertEdge(QPointF pos);

    virtual void removeVertex(QPoint &atItemPos);
    virtual void addVertex (QPoint &atItemPos);

    virtual QPointF intersection(const QPointF &l1A, const QPointF &l1B,
                                 const QPointF &l2A, const QPointF &l2B);
    virtual bool pointBelongsTo(const QPoint point, CPathPoint *pPath);

    virtual void splitAt(QPoint point, CPathElement * newelement);
    virtual void connectWithSelected();
    virtual void connectWith(CPathElement * pOther);

    CPathPoint * Path() const;
    void setPath(CPathPoint *pPath);

    virtual QJsonObject& getSaveValue() override;
    virtual void LoadElement(const QJsonObject& obj) override;

protected:
    //These are attached to mouse events of tthe viewport as signals.
    //Used when placing the item, or when editing its path
    virtual void mousePress(QMouseEvent *event) override;
    virtual void mouseRelease(QMouseEvent *event) override;
    virtual void mouseMove(QMouseEvent *event) override;
    virtual void keyPress(QKeyEvent *event) override;

    //These are called when the item is already added to the scene.
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;


protected:
    CPathPoint * m_pPath;
    CPathPoint * m_pCurPoint;
    qreal m_startx,m_starty;
    bool m_EditingEdge;
    QPointF m_SavedPoint;
};

#endif // CPATHELEMENT_H
