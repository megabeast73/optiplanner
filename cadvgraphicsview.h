#ifndef CADVGRAPHICSVIEW_H
#define CADVGRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsView>
#include "caxises.h"

class CUndoPropChange;

class CAdvGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:

    enum MouseShape {
        Arrow,
        Rotate,
        Move,
        Precision
    };

    CAdvGraphicsView();
    CAdvGraphicsView(QWidget * parent);
    virtual ~CAdvGraphicsView() override;

    //Scene related
    const QBrush getBackBrush() const;
    void setBackBrush(const QBrush& c);

    virtual void setCursorShape(MouseShape sh);

    void ScrollSceneTo(const QPoint newPos);
    void CAdvGraphicsView::rotateSelection(const QPoint pos);

protected:
    QPoint m_BaseMousePos;
    QList<QGraphicsItem *> m_SelectedItems;
    CAxises * m_Axises;
    CUndoPropChange * m_UndoRotate;

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *mouseEvent) override;
    virtual void mouseReleaseEvent(QMouseEvent *mouseEvent) override;

    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;


    virtual void wheelEvent(QWheelEvent* event) override;

    void zoomIn();
    void zoomOut();
    qreal GetZoomPixelSize();

    void showAxises(const QPointF pos);
    void hideAxises();

protected slots:
    void ruberChanged(QRect rubberBandRect, QPointF fromScenePoint, QPointF toScenePoint);


};



#endif // CADVGRAPHICSVIEW_H
