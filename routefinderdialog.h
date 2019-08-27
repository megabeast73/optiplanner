#ifndef ROUTEFINDERDIALOG_H
#define ROUTEFINDERDIALOG_H

#include <QDialog>

namespace Ui {
class RouteFinderDialog;
}

#define PATH_START 0
#define PATH_PASSTHRU 1
#define PATH_STOP 2

struct path
{
    long el_id; //-1 empty
    int el_type; // 0- Start, 1- PassThru, 2-stop
    path * next;
};

class RouteFinderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RouteFinderDialog(QWidget *parent = nullptr);
    ~RouteFinderDialog();

    void setStartElement(long elid);
    void setStopElement(long elid);
    path*  addPassthruElement(long elid);
    void removePassthruElement(long elid);
    void clearPath();

    path* getPath(); //use this function to access the path list. It will create the list, if necesarry

    void arrangeElements();
private:
    void deletePath(path *p);
    path* getLastPath();

    void addPathSection(const QString& secName, const QString& elName,int el_type,int el_id);

private slots:
    void on_btnClose_clicked();

    void addLink(QAction *action);
    void editLink(QAction *action);
    void removeLink(QAction *action);
    
    

private:
    Ui::RouteFinderDialog *ui;
    path * m_path;
};

#endif // ROUTEFINDERDIALOG_H
