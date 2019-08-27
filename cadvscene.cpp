#include "cadvscene.h"
#include "globals.h"
#include "mainwindow.h"
#include "undocommands.h"
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>

CAdvScene::CAdvScene() : QGraphicsScene ()
{
    m_Grid = nullptr;
    m_InsertedElement = nullptr;
}
CAdvScene::~CAdvScene()
{
    if (m_Grid)
        delete m_Grid;


}

void CAdvScene::setSceneRect(const QRectF &rect)
{
    QGraphicsScene::setSceneRect(rect);
    if (!m_Grid)
    {
        m_Grid = new CGrid;
        addItem(m_Grid);
    }
    m_Grid->SetSceneSize(rect);

}

void CAdvScene::setGridVisible (bool visible)
{
    if (m_Grid)
        m_Grid->setVisible(visible);
}
bool CAdvScene::getGridVisible()
{
    return m_Grid ? m_Grid->isVisible() : false;
}

void CAdvScene::setGridColor(QColor &c)
{
    if (m_Grid)
        m_Grid->setGridColor(c);
}

 QColor CAdvScene::getGridColor()
{
    return m_Grid ? m_Grid->getGridColor() :  QColor();

}


void CAdvScene::setScaleColor(QColor &c)
{
    if (m_Grid)
        m_Grid->setScaleColor(c);

}
 QColor CAdvScene::getScaleColor()
{
    if (!m_Grid)
        return QColor();
    return m_Grid->getScaleColor();
}

 QJsonObject CAdvScene::getSaveValue ()
 {
    QJsonObject ret;
    QRectF sz = sceneRect();
    QString strDiagr;
    QString strBg;
    if (this == g_MainWindow->InfraScene())
    {
         strBg = g_MainWindow->getInfraBack().color().name();
    }
    else {
        strBg = g_MainWindow->getWiringBack().color().name();
    }
    ret.insert("w",QJsonValue(sz.width()));
    ret.insert("h",QJsonValue(sz.height()));

    ret.insert("bg",QJsonValue(strBg));
    ret.insert("grid_c",QJsonValue(getGridColor().name()));
    ret.insert("scale_c",QJsonValue(getScaleColor().name()));
    ret.insert("grid_v",QJsonValue(getGridVisible()));
    return ret;
 }

 void CAdvScene::setSaveValue(const QJsonObject& obj)
 {

    QRectF r;
    r.setWidth(obj["w"].toDouble());
    r.setHeight(obj["h"].toDouble());
    setSceneRect(r);

    QColor c;
    c.setNamedColor(obj["bg"].toString());
    if (this == g_MainWindow->InfraScene())
        g_MainWindow->setInfraBack(c);
    else
        g_MainWindow->setWiringBack(c);

    c.setNamedColor(obj["grid_c"].toString());
    setGridColor(c);
    c.setNamedColor(obj["scale_c"].toString());
    setScaleColor(c);
    setGridVisible(obj["grid_v"].toBool());

 }

void CAdvScene::addElement(CElement *pElement)
{
    Q_ASSERT(pElement);

    if (m_InsertedElement)
        removeItem(m_InsertedElement);
    m_InsertedElement = pElement;
    addItem(pElement);
    views()[0]->setMouseTracking(true);
    views()[0]->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

void CAdvScene::finishAddingElement()
{
    g_Objects.registerElement(m_InsertedElement); //Must be before UndoCommand to obtain ElementID
    CUndoAddElement * pUndoAdd = new CUndoAddElement(m_InsertedElement);

    removeItem(m_InsertedElement); //Item will be added again by CUndoAddElement::redo();
    delete m_InsertedElement;
    m_InsertedElement = nullptr;

    g_UndoStack->push(pUndoAdd);

    views()[0]->setMouseTracking(false);
    views()[0]->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

}

void CAdvScene::cancelAddingElement()
{
    views()[0]->setMouseTracking(false);
    removeItem(m_InsertedElement);
    m_InsertedElement = nullptr;
    views()[0]->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
}


void CAdvScene::keyPressEvent(QKeyEvent *keyEvent)
{
    if (m_InsertedElement)
    {
        m_InsertedElement->keyPress(keyEvent);
        return;
    }
    emit keyPressed(keyEvent);

    if (keyEvent->key() == Qt::Key_Delete )
    {
        deleteSelection();
        return;
    }
    QGraphicsScene::keyPressEvent(keyEvent);
}

void CAdvScene::keyReleaseEvent(QKeyEvent *keyEvent)
{
    if (m_InsertedElement)

        m_InsertedElement->keyRelease(keyEvent);
    else
        QGraphicsScene::keyReleaseEvent(keyEvent);
}


void CAdvScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (m_InsertedElement)
        m_InsertedElement->mouseMove(mouseEvent);
    else
        QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void CAdvScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{


    if (m_InsertedElement)
    {
        m_InsertedElement->mousePress(mouseEvent);
        return;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
    if (mouseEvent->button() == Qt::LeftButton)
    {
        m_SelectedItems = selectedItems();
        if (m_SelectedItems.count() !=0)
        {
            m_UndoMove = new CUndoMoveElement(m_SelectedItems);
            m_UndoMove->setText("Move element(s)");
        }
    }



}

void CAdvScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (m_InsertedElement)
    { //Now element is inserted. Ignore all other actions
        m_InsertedElement->mouseRelease(mouseEvent);
        return;
    }
    emit (mouseRelesed(mouseEvent));

    if (m_SelectedItems.count() != 0 && mouseEvent->button() == Qt::LeftButton )
    {  //Handle move element
        m_UndoMove->finishPropChange();
        el_pos * pFirst = m_UndoMove->itemAt(0);
        if (pFirst->end_pos != pFirst->start_pos)
        { //Elements has been moved
            g_UndoStack->push(m_UndoMove);
        }
        else if (m_UndoMove)
        {
            delete m_UndoMove;
            m_UndoMove = nullptr;
        }
    }
    m_SelectedItems.clear();
    //Handle element pointed by the user and show it in Property Viewer
    CElement *pEl = (CElement *) itemAt(mouseEvent->scenePos(),QTransform());
    if (pEl)
        g_MainWindow->setElementToObjectSelector(pEl);
    else
        g_MainWindow->clearSelectedElement();
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void CAdvScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *contextMenuEvent)
{
    QGraphicsScene::contextMenuEvent(contextMenuEvent);
}


void CAdvScene::deleteSelection()
{
    m_SelectedItems = selectedItems();
    if (m_SelectedItems.count() > 0)
        if (QMessageBox::question(nullptr,"Confirm delete","Delete selected element(s) ?",QMessageBox::Yes | QMessageBox::No)
            == QMessageBox::Yes)
    {
        CUndoDelElement *pCmd = new CUndoDelElement(m_SelectedItems);
        clearSelection();
        g_UndoStack->push(pCmd);
        m_SelectedItems.clear(); // Empty the list now to avoid mismatches in other event handlers
    }
}
