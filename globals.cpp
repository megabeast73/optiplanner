#include "globals.h"
#include "cshaft.h"
#include "cpipe.h"


CObjectCollection g_Objects;
MainWindow * g_MainWindow;

CElement * createElement(CElement::ElementType tp)
{
    switch (tp)
    {
        case CElement::Shaft:
            return new CShaft;
        case CElement::Pipe:
            return new CPipe;
/*
        Cable,
        Tube,
        Fiber
        */
    }
    return nullptr;
}
