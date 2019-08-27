#include "cobjectcollection.h"
#include <qvector.h>
#include <QComboBox>

QVector<CElement *> Elements;

CObjectCollection::CObjectCollection(QObject *parent) : QObject(parent)
{

}
CObjectCollection::~CObjectCollection()
{

}

void CObjectCollection::pushElement(CElement * pElement)
{
    int elID = pElement->ElementID();
    if (Elements.count() <= elID)
        Elements.resize(elID+1);
    Elements[elID] = pElement;

    addElementToSelector(pElement);;
}

void CObjectCollection::registerElement(CElement *pElement)
{
    int lLast = Elements.indexOf(nullptr);
    if (lLast < 0)
    {
        Elements.push_back(pElement);
        lLast =Elements.count()-1;
    }
    else
        Elements[lLast] = pElement;

    pElement->setElementID(lLast);
    //add item to the object selector
    addElementToSelector(pElement);
}

void CObjectCollection::deregisterElement(CElement *pElement, bool notify)
{
    long lID = pElement->ElementID();
    if (lID < 0)
        return;

    //CElement * pE;
    Elements[lID] = nullptr;
    if (!notify)
        return;
    //Remove the item from the object selector combo
    if (m_ObjectSelector)
    {
        for (int i = m_ObjectSelector->count() -1; i >=0; i--)
            if (m_ObjectSelector->itemData(i).toInt() == lID)
            {
                m_ObjectSelector->removeItem(i);
                break;
            }
    }
}
void CObjectCollection::addElementToSelector(CElement * pElement)
{

    if (!m_ObjectSelector)
        return;
    int elID = pElement->ElementID();
    if (m_ObjectSelector->findData(QVariant(elID)) > -1)
        return;
    QString objName (pElement->elementTypeName());
    objName.append(" : ");
    objName.append(pElement->getElementName());
    m_ObjectSelector->addItem(objName,QVariant(elID));
}
CElement * CObjectCollection::elementById(long ID)
{
    return Elements.at(ID);
}
/*
QComboBox *CObjectCollection::ObjectSelector() const
{
    return m_ObjectSelector;
}
*/
void CObjectCollection::setObjectSelector(QComboBox *ObjectSelector)
{
    m_ObjectSelector = ObjectSelector;
}


CElement * CObjectCollection::elementAt(long ix)
{
    if (Elements.count() <ix)
        return nullptr;
    return Elements.at(ix);
}
long CObjectCollection::count()
{
    return Elements.count();
}
