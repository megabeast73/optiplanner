#ifndef COBJECTCOLLECTION_H
#define COBJECTCOLLECTION_H

#include <QObject>
#include <QList>
#include <celement.h>



class CObjectCollection : public QObject
{
    Q_OBJECT
public:
    explicit CObjectCollection(QObject *parent = nullptr);
    virtual ~CObjectCollection();

    virtual void pushElement(CElement * pElement);
    virtual void registerElement(CElement *pElement);
    virtual void deregisterElement(CElement *pElement, bool notify = true);
    virtual CElement * elementById(long ID);






};

#endif // COBJECTCOLLECTION_H
