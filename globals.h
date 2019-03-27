#ifndef GLOBALS_H
#define GLOBALS_H

#include "cobjectcollection.h"
#include "celement.h"

class MainWindow;
extern  CObjectCollection g_Objects;
extern CElement * createElement(CElement::ElementType tp);
extern MainWindow * g_MainWindow;
#endif // GLOBALS_H
