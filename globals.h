#ifndef GLOBALS_H
#define GLOBALS_H

#include "cobjectcollection.h"
#include "celement.h"
#include <qundostack.h>

class MainWindow;
extern  CObjectCollection g_Objects;
extern CElement * createElement(CElement::ElementType tp);
extern MainWindow * g_MainWindow;
extern QUndoStack * g_UndoStack;
#endif // GLOBALS_H
