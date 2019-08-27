#ifndef UNDOCOMMANDS_H
#define UNDOCOMMANDS_H
#include <QUndoCommand>

#include "celement.h"
#include "cadvscene.h"

struct el_pos
{
    int el_id;
    QPointF start_pos;
    QPointF end_pos;
};

struct el_prop
{
    CElement * element;
    QJsonObject prop;
};
struct el_prop_new : public el_prop
{
    QJsonObject new_prop;
};


class CUndoAddElement : public QUndoCommand
{
public:
    CUndoAddElement(CElement *pEl);

    void undo() override;
    void redo() override;

protected:
    //CElement * m_El;
    QJsonObject m_El_Object;
    QGraphicsScene * m_Scene;

};
class CUndoSplitElement : public QUndoCommand
{

public:
    CUndoSplitElement(CElement * orig);
    ~CUndoSplitElement();
    void undo() override;
    void redo() override;
    void finishSplit(CElement * splitted);

protected:
    el_prop m_OrigObject;
    QJsonObject m_OrigObject_Splitted;
     CUndoAddElement * m_UndoAdd;

};
class CUndoDelElement : public QUndoCommand
{
public:
    CUndoDelElement(ITEMLIST items);

    void undo() override;
    void redo() override;

protected:
    QList<el_prop *> m_Prop;
    QGraphicsScene * m_Scene;

};

class CUndoPropChange : public QUndoCommand
{
public:
    CUndoPropChange(ITEMLIST items);
    ~CUndoPropChange();

    void undo() override;
    void redo() override;
    void finishPropChange();

protected:
    QList<el_prop_new *> m_Prop;
};

class CUndoRotate : public CUndoPropChange
{

public:
    CUndoRotate(ITEMLIST items);
    virtual int id() const override {return 1; }
    virtual bool mergeWith(const QUndoCommand *command) override;
    const QJsonObject elementNewProp(CElement *pEl) const;




};

class CUndoMoveElement: public QUndoCommand
{
public:
    CUndoMoveElement(ITEMLIST items);
    ~CUndoMoveElement();

    void finishPropChange();

    void undo() override;
    void redo() override;
    el_pos * itemAt(int i) {return m_items.at(i); }

protected:
    QList<el_pos *> m_items;
};

#endif // UNDOCOMMANDS_H
