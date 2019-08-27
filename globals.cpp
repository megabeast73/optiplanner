#include "globals.h"
#include "cshaft.h"
#include "cpipe.h"
#include "ccenter.h"



CObjectCollection g_Objects;
MainWindow * g_MainWindow;
QUndoStack * g_UndoStack;

CElement * createElement(CElement::ElementType tp)
{
    switch (tp)
    {
        case CElement::Shaft:
            return new CShaft;
        case CElement::Pipe:
            return new CPipe;
        case CElement::ControlCenter:
            return new CCenter;
    default:
            Q_ASSERT(false);
/*
        Cable,
        Tube,
        Fiber
        */
    }
    return nullptr;
}
