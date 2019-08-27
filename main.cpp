#include "mainwindow.h"
#include <QApplication>
#include <globals.h>

//Disable ALT key to open the menu bar.
// 10x Meefte@stackoverflow
#include <qproxystyle.h>

class MenuStyle : public QProxyStyle
{
public:
    int styleHint(StyleHint stylehint, const QStyleOption *opt, const QWidget *widget, QStyleHintReturn *returnData) const
    {
        if (stylehint == QStyle::SH_MenuBar_AltKeyNavigation)
            return 0;

        return QProxyStyle::styleHint(stylehint, opt, widget, returnData);
    }
};


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(new MenuStyle());
    MainWindow w;
    g_MainWindow = &w;
    w.show();

    return a.exec();
}
