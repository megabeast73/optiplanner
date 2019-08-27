#ifndef CADVSCENCE_H
#define CADVSCENCE_H

#include <QObject>
#include <qgraphicsscene.h>
#include <QRectF>

#include "cgrid.h"

class CUndoMoveElement;

class CAdvScene : public QGraphicsScene
{
    Q_OBJECT
    friend class CGrid;
public:
    CAdvScene();
    ~CAdvScene();


    void setSceneRect(const QRectF &rect);

    void setGridVisible (bool visible);
    bool getGridVisible();
    void setGridColor(QColor &c);
    QColor getGridColor();

    void setScaleColor(QColor &c);
    QColor getScaleColor();

    QJsonObject getSaveValue ();
    void setSaveValue(const QJsonObject& obj);

    void addElement(CElement *pElement);
    void finishAddingElement();
    void cancelAddingElement();
    bool isAdding() { return m_InsertedElement !=nullptr; }

    //Operations
    void deleteSelection();
signals:
    void keyPressed(QKeyEvent* keEvent);
    void mouseRelesed(QGraphicsSceneMouseEvent *mouseEvent);

protected:

    virtual void keyPressEvent(QKeyEvent *keyEvent);
    virtual void keyReleaseEvent(QKeyEvent *keyEvent);

    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent);




protected:
    CGrid * m_Grid;
    CElement *m_InsertedElement;

    //Undo move related
    QList<QGraphicsItem *> m_SelectedItems;
    QPointF m_OldPos;
    CUndoMoveElement * m_UndoMove;

};

#endif // CADVSCENCE_H
