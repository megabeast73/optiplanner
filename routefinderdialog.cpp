#include "routefinderdialog.h"
#include "ui_routefinderdialog.h"
#include <QLabel>
#include <QToolBar>
#include <QToolButton>
#include "globals.h"


RouteFinderDialog::RouteFinderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RouteFinderDialog)
{
    ui->setupUi(this);
    m_path = nullptr;
    arrangeElements();
    setFixedSize(size());
}

RouteFinderDialog::~RouteFinderDialog()
{
    clearPath();
    delete ui;
}


path* RouteFinderDialog::getPath()
{
    if (m_path)
        return m_path;
    m_path = new path;
    m_path->el_id = -1;
    m_path->el_type = PATH_START;
    m_path->next = nullptr;
    return  m_path;
}
void RouteFinderDialog::setStartElement(long elid)
{
    path * p = getPath();
    assert(p->el_type == PATH_START); // first element must be PATH_START
    p->el_id = elid;
    p->el_type = PATH_START;
}
void RouteFinderDialog::setStopElement(long elid)
{
    path* p = getLastPath();
    if (p->el_type != PATH_STOP)
    {
        p->next = new path;
        p = p->next;
        p->el_type = PATH_STOP;
    }
    p->el_id = elid;
}

path*  RouteFinderDialog::addPassthruElement(long after)
{
    path * p = getPath();
    while (p && p->el_id != after)
        p = p->next;

    if (!p || p->el_id != after)
        return nullptr;

    path *pnext = p->next; //save next pointer
    p->next = new path;
    p->next->next = pnext;
    p->next->el_id = -1;
    p->next->el_type = PATH_PASSTHRU;
    return p->next;

}
void RouteFinderDialog::removePassthruElement(long elid)
{
    path * p = getPath();
    while (p->next && p->next->el_id != elid)
        p = p->next;

    if (p->next && p->next->el_id == elid)
    {
        path *pnext = p->next->next;
        delete p->next;
        p->next =pnext;
    }
}
void RouteFinderDialog::clearPath()
{
    if (m_path)
        deletePath(m_path);
    m_path = nullptr;
}

void RouteFinderDialog::deletePath(path *p)
{
    if (p->next)
        deletePath(p->next);
    delete p;
}
path* RouteFinderDialog::getLastPath()
{
   path *p = m_path;
   while (p && p->next)
       p = p->next;
   return p;
}

void RouteFinderDialog::addPathSection(const QString& secName, const QString& elName,int el_type,int el_id)
{

    QFont fnt("Arial");
    fnt.setPointSize(12);

    QLabel *pL = new QLabel;
    pL->setText(secName);
    pL->setFont(fnt);
    ui->tblPath->insertRow(ui->tblPath->rowCount());

    ui->tblPath->setCellWidget(ui->tblPath->rowCount()-1,0,pL);

    pL = new QLabel;
    pL->setText(elName);
    fnt.setUnderline(true);
    pL->setFont(fnt);
    ui->tblPath->setCellWidget(ui->tblPath->rowCount()-1,1,pL);

    QToolBar *pBar = new QToolBar;
    QToolButton *pButton;
    QAction *a;

    pButton = new QToolButton;
    //pButton->setWindowIcon(QIcon(":/IMAGES/ADD_LINK"));
    //a = pBar->addAction(QIcon(":/IMAGES/ADD_LINK"),"+");

    a = new QAction;
    a->setEnabled(el_type != PATH_STOP); // it is not stop element
    a->setData(QVariant(el_id));
    a->setIcon(QIcon(":/IMAGES/ADD_LINK"));
    pButton->setDefaultAction(a);
    pBar->addWidget(pButton);
    connect(pButton,&QToolButton::triggered,this,&RouteFinderDialog::addLink);

    pButton = new QToolButton;
    a = new QAction;
    a->setEnabled(el_type != PATH_STOP); // it is not stop element
    a->setData(QVariant(el_id));
    a->setIcon(QIcon(":/IMAGES/REMOVE_LINK"));
    a->setEnabled(el_type == PATH_PASSTHRU);
    pButton->setDefaultAction(a);
    pBar->addWidget(pButton);
    connect(pButton,&QToolButton::triggered,this,&RouteFinderDialog::removeLink);

    pButton = new QToolButton;
    a = new QAction;
    //a->setEnabled(el_type != PATH_STOP); // it is not stop element
    a->setData(QVariant(el_id));
    a->setIcon(QIcon(":/IMAGES/EDIT_LINK"));
    pButton->setDefaultAction(a);
    pBar->addWidget(pButton);
    connect(pButton,&QToolButton::triggered,this,&RouteFinderDialog::editLink);



//    a = pBar->addAction(QIcon(":/IMAGES/REMOVE_LINK"),"-");
//    a->setEnabled(el_type == PATH_PASSTHRU); //it is pass-thru element
//    a->setData(QVariant(el_id));

//    pBar->addAction(QIcon(":/IMAGES/EDIT_LINK"),"E")->setData(QVariant(el_id));
    ui->tblPath->setCellWidget(ui->tblPath->rowCount()-1,2,pBar);


}

void RouteFinderDialog::arrangeElements()
{


   int idEnd = -1;
   path *p = m_path;
   QString strSec,strName;

   //Clear the table
    while (ui->tblPath->rowCount() > 0)
        ui->tblPath->removeRow(0);

    while (ui->tblPath->columnCount() < 3)
         ui->tblPath->insertColumn(ui->tblPath->columnCount());
    ui->tblPath->setColumnWidth(1,240);

    ui->btnFindRoute->setEnabled(true);

    if (!m_path || m_path->el_type == PATH_START)
    {
            addPathSection("Start from: ","Click edit to select",PATH_START,-1);
            ui->btnFindRoute->setEnabled(false);
    }

    while (p)
    {
        switch (p->el_type) {
        case PATH_START:
                strSec = "Start from: ";
            break;
        case PATH_PASSTHRU:
                strSec = "Pass thru:";
            break;
        case PATH_STOP:
                strSec = "Stop at:";
                idEnd =p->el_id;

        }
        if (p->el_id == -1)
            strName = "Click edit to select";
        else {
            strName = g_Objects.elementById(p->el_id)->getElementName();
        }

        addPathSection(strName,strSec,p->el_type,p->el_id);
    }



    if (idEnd == -1)
    {
        addPathSection("Stop at: ","Click edit to select",PATH_STOP,-1);
        ui->btnFindRoute->setEnabled(false);
    }

}

void RouteFinderDialog::on_btnClose_clicked()
{
    hide();
}


void RouteFinderDialog:: addLink(QAction *action)
{
}
void  RouteFinderDialog::editLink(QAction *action)
{
}
void  RouteFinderDialog::removeLink(QAction *action)
{
}
