#ifndef CADVGRAPHICSVIEW_H
#define CADVGRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QJsonValue>
#include <QJsonObject>
//#include <QGraphicsTextItem>

class CGrid;
class CElement;
class CInfraProp;

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
    virtual ~CAdvGraphicsView() override;
    virtual void setSceneAA(QGraphicsScene *scene);
    void InsertGrid ();
    void RemoveGrid();
    void ClearScene();

    void drawForeground(QPainter* painter, const QRectF& rect) override;
    void paintEvent(QPaintEvent *ev) override;

    void startRotation();
    void endRotation(bool bCancel = false);
    void rotate(const QPoint pos);

    void selectItems(const QPoint &toPos);
    void drawSelectionRect();

    virtual void setCursorShape(MouseShape sh);

    bool inserting() const;
    void setInserting(bool inserting);

    CElement *ActiveElement() const;
    void setActiveElement(CElement *ActiveElement);

    void setSaveObject(const QJsonObject &obj);
    QJsonObject getSaveObject();

    virtual void Closing(QCloseEvent *e);
    void ClosePropDialog();
    void ShowPropDialog();

    const QColor& getBackColor() const;
    void setBackColor(const QColor& c);

signals:
    void mousePress(QMouseEvent *event);
    void mouseRelease(QMouseEvent *event);
    void mouseMove(QMouseEvent *event);
    void keyPress(QKeyEvent * event);



protected:
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void mousePressEvent(QMouseEvent *mouseEvent) override;
    virtual void mouseMoveEvent(QMouseEvent *mouseEvent) override;
    virtual void mouseReleaseEvent(QMouseEvent *mouseEvent) override;

    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;

    virtual void contextMenuEvent(QContextMenuEvent *event) override;

    void ScrollSceneTo(const QPoint &newPos);

    void zoomIn();
    void zoomOut();

    qreal GetZoomPixelSize();

protected:
    int m_MouseOrigX;
    int m_MouseOrigY;
    bool m_bRotating;
    bool m_bSelecting;
    bool m_inserting;
    QPoint m_rotatePoint;

    QRectF m_SceneRect;

    Qt::MouseButtons m_MouseBState;

    CGrid * m_pGrid;
    CElement * m_ActiveElement;
    CInfraProp * m_pPropDialog;




};



#endif // CADVGRAPHICSVIEW_H
