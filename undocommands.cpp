#include "undocommands.h"
#include "globals.h"


CUndoAddElement::CUndoAddElement(CElement *pEl) :QUndoCommand (nullptr)
{
    //m_El = pEl;
    m_Scene = pEl->scene();
    m_El_Object = pEl->getSaveValue();
    setText ("Add element");
}

void CUndoAddElement::undo()
{
    CElement * el = g_Objects.elementById(m_El_Object["e_id"].toInt());
    m_Scene->removeItem(el);
    g_Objects.deregisterElement(el);
    el->clearAllConnections();
    delete el;
}

void CUndoAddElement::redo()
{
     CElement * el = createElement((CElement::ElementType) m_El_Object["e_type"].toInt());
    el->LoadElement(m_El_Object); // Will push element into g_Objects
    m_Scene->addItem(el);
    el->setNameVisibe(true);
    el->reconnectElement();

}


CUndoDelElement::CUndoDelElement(ITEMLIST items) :QUndoCommand (nullptr)
{

    m_Scene = items.at(0)->scene();
    el_prop * pEl;
    for (int i = items.count()-1; i >=0; i--)
    {
        pEl = new (el_prop);
        pEl->prop = dynamic_cast<CElement *>(items.at(i))->getSaveValue();
        //pEl->element = (CElement *) items.at(i);
        m_Prop.push_back(pEl);
    }

    setText ("Delete element(s)");
}

void CUndoDelElement::redo()
{
    int eid;
    CElement *pEl;
    for (int i = m_Prop.count()-1; i >=0; i--)
    {
        eid = m_Prop.at(i)->prop["e_id"].toInt();
        pEl = g_Objects.elementById(eid);
        m_Scene->removeItem(pEl);
                    //m_Prop.at(i)->element);
        g_Objects.deregisterElement(pEl);
        pEl->clearAllConnections();
        //
        delete pEl;
    }
}

void CUndoDelElement::undo()
{
    CElement * pEl;
    for (int i = m_Prop.count()-1; i >=0; i--)
    {
        pEl = createElement((CElement::ElementType) m_Prop.at(i)->prop["e_type"].toInt());
        pEl->LoadElement(m_Prop.at(i)->prop);
        m_Prop[i]->element = pEl;
        m_Scene->addItem(pEl);
        pEl->reconnectElement();
    }
}


CUndoPropChange::CUndoPropChange(ITEMLIST items) :QUndoCommand (nullptr)
{


    el_prop_new * pEl;
    for (int i = items.count()-1; i >=0; i--)
    {
        pEl = new (el_prop_new);
        pEl->prop = dynamic_cast<CElement *>(items.at(i))->getSaveValue();
        pEl->element = (CElement *) items.at(i);
        m_Prop.push_back(pEl);
    }

    setText ("Element(s) property change");
}

CUndoPropChange::~CUndoPropChange()
{
    while (m_Prop.count() > 0)
        delete m_Prop.takeLast();
}

void CUndoPropChange::redo()
{
    int eid;
    CElement *pEl;
    for (int i = m_Prop.count()-1; i >=0; i--)
    {
        eid = m_Prop.at(i)->prop["e_id"].toInt();
        pEl = g_Objects.elementById(eid);
        pEl->LoadElement(m_Prop.at(i)->new_prop);
    }
}

void CUndoPropChange::undo()
{
    int eid;
    CElement *pEl;
    for (int i = m_Prop.count()-1; i >=0; i--)
    {
        eid = m_Prop.at(i)->prop["e_id"].toInt();
        pEl = g_Objects.elementById(eid);
        pEl->LoadElement(m_Prop.at(i)->prop);
    }

}

void CUndoPropChange::finishPropChange()
{
    el_prop_new * pEl;
    for (int i = m_Prop.count()-1; i >=0; i--)
    {
        pEl = m_Prop.at(i);
        pEl->new_prop = pEl->element->getSaveValue();
    }
}


CUndoMoveElement::CUndoMoveElement(ITEMLIST items) :QUndoCommand (nullptr)
{

    el_pos * el;
    //Get positions of selected elements
    for (int i = items.count() -1; i >=0; i--)
    {
        el = new (el_pos);
        el->start_pos = items.at(i)->pos();
        el->el_id = dynamic_cast<CElement*>(items.at(i))->ElementID();
        m_items.push_back(el);
    }
    setText ("Element(s) Property change");
}

CUndoMoveElement::~CUndoMoveElement()
{
    while (m_items.count() > 0)
        delete m_items.takeLast();
}

void CUndoMoveElement::finishPropChange()
{
    CElement *el;
    //Get new positions of selected elements
    for (int i = m_items.count()-1; i >=0; i--)
    {
        int id = m_items.at(i)->el_id;
        el = g_Objects.elementById(id);
        m_items[i]->end_pos = el->pos();
    }
}

void CUndoMoveElement::undo()
{
    CElement *el;
    for (int i = m_items.count()-1; i >=0; i--)
    {
        int id = m_items.at(i)->el_id;
        el = g_Objects.elementById(id);
        el->setPos(m_items[i]->start_pos);
        el->reconnectElement();
        //g_Objects.elementById(m_items.at(i)->el_id)->setPos(m_items[i]->start_pos);
    }
}

void CUndoMoveElement::redo()
{
    CElement *el;
    for (int i = m_items.count()-1; i >=0; i--)
    {
        el = g_Objects.elementById(m_items.at(i)->el_id);
        el->setPos(m_items[i]->end_pos);
        el->reconnectElement();
    }
}

CUndoRotate::CUndoRotate(ITEMLIST items) : CUndoPropChange (items)
{
    setText("Rotate element(s)");
}

bool CUndoRotate::mergeWith(const QUndoCommand *command)
{
    const CUndoRotate * pNew = dynamic_cast<const CUndoRotate *> (command);
    QJsonObject tmp;
    for (int i = m_Prop.count()-1; i>=0; i--)
    {
        tmp = pNew->elementNewProp(m_Prop.at(i)->element);
        if (tmp.isEmpty())
            return false;
        m_Prop[i]->new_prop = tmp;
    }
    return true;
}

const QJsonObject CUndoRotate::elementNewProp(CElement *pEl) const
{
    QJsonObject ret;
    for (int i = m_Prop.count()-1; i >=0; i--)
        if (m_Prop.at(i)->element == pEl)
            return m_Prop.at(i)->new_prop;
    return QJsonObject();
}

CUndoSplitElement::CUndoSplitElement(CElement * orig)
{
    m_OrigObject.prop = orig->getSaveValue();
    m_OrigObject.element = orig;
    m_UndoAdd = nullptr;
    setText("Split element");

}

CUndoSplitElement::~CUndoSplitElement()
{
    if (m_UndoAdd)
        delete m_UndoAdd;
}

void CUndoSplitElement::undo()
{
    if (m_UndoAdd)
        m_UndoAdd->undo();
    int id = m_OrigObject_Splitted["id"].toInt();
    CElement * pEl = g_Objects.elementById(id);
    pEl->LoadElement(m_OrigObject.prop);
}

void CUndoSplitElement::redo()
{
    if (m_UndoAdd)
        m_UndoAdd->redo();
    int id = m_OrigObject_Splitted["id"].toInt();
    CElement * pEl = g_Objects.elementById(id);
    pEl->LoadElement(m_OrigObject_Splitted);
}

void CUndoSplitElement::finishSplit(CElement * splitted)
{
    m_OrigObject_Splitted = m_OrigObject.element->getSaveValue();
    m_UndoAdd = new CUndoAddElement(splitted);

}
