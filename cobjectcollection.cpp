#include "cobjectcollection.h"
#include <qvector.h>

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
}

void CObjectCollection::registerElement(CElement *pElement)
{
    long lLast = Elements.indexOf(nullptr);
    if (lLast < 0)
    {
        Elements.push_back(pElement);
        lLast =Elements.count()-1;
    }
    else
        Elements[lLast] = pElement;

    pElement->setElementID(lLast);
}

void CObjectCollection::deregisterElement(CElement *pElement, bool notify)
{
    long lID = pElement->ElementID();
    if (lID < 0)
        return;
    CElement * pE;
    Elements[lID] = nullptr;
    if (!notify)
        return;
    for (int i = Elements.count()-1; i > -1; i--)
    {
        pE = Elements.at(i);
        if (pE)
            pE->elementIsDeregistered(lID);
    }


}

CElement * CObjectCollection::elementById(long ID)
{
    return Elements.at(ID);
}


