#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDockWidget>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include "celement.h"
#include "cpipe.h"
#include "cshaft.h"

#include <qfiledialog.h>
#include <qapplication.h>
#include <QJsonDocument>
#include <QMessageBox>

#include "globals.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);



    m_EProp = nullptr;

    m_pView = new CAdvGraphicsView;

    ui->centralWidget->layout()->addWidget(m_pView);
    m_pView->setFocus();
    m_pView->setBackColor(Qt::darkGray);

    m_pScene = new QGraphicsScene;
    m_pScene->setSceneRect(QRectF(0.0, 0.0,100000,100000));

    m_pView->setSceneAA(m_pScene);
    setDiagramName("New diagram");

    ConfigureProgressBar(0,100);

    m_ProgressBar.setValue(0);
    ui->statusBar->addWidget(&m_lblName);
    ui->statusBar->addWidget(&m_lblSize);
    ui->statusBar->addWidget(&m_CurrentView);
    ui->statusBar->addWidget(&m_ProgressBar);
    //ui->statusBar->showMessage("Opti Planner started!",5000);
    ConfigureStatusBar();
    m_ProgressBar.setVisible(false);
    ConfigureMenus();
    clearSelectedElement();

}

MainWindow::~MainWindow()
{
    delete m_pAddPassiveMenu;
    delete m_pView;
    delete m_pScene;
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::AltModifier &&
        !event->key())
    {
        event->ignore();
        return;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::ConfigureProgressBar(int min,int max)
{
    m_ProgressBar.setVisible(true);
    m_ProgressBar.setMinimum(min);
    m_ProgressBar.setMaximum(max);
}

void MainWindow::ConfigureMenus()
{

    QMenu * pTmp =ui->menuBar->addMenu("Add");
    m_pAddPassiveMenu =  pTmp->addMenu("Passive Element");
    m_pAddPassiveMenu->addAction(QIcon(":/IMAGES/SHAFT_BUTTON"),"Pit/Shaft",this,&MainWindow::on_Add_Shaft);
    m_pAddPassiveMenu->addAction(QIcon(":/IMAGES/PIPE_BUTTON"),"Pipe",this,&MainWindow::on_Add_Pipe);
    m_pAddPassiveMenu->addAction("Fiber");
    m_pAddPassiveMenu->addAction("Closure");
    m_pAddPassiveMenu->addAction("Connector");
    m_pAddPassiveMenu->addAction("Active applience");


    connect(ui->actionSaveAs,&QAction::triggered,this,&MainWindow::onSaveAs);
    connect(ui->actionOpen,&QAction::triggered,this,&MainWindow::onOpen);
    connect(ui->actionSave,&QAction::triggered,this,&MainWindow::onSave);

    connect(ui->actionDProperties,&QAction::triggered,m_pView,&CAdvGraphicsView::ShowPropDialog);
}

void MainWindow::ConfigureStatusBar()
{
    m_CurrentView.setText("Infrastructure");
    m_lblName.setText(DiagramName());
    QRectF size = m_pScene->sceneRect();
    qreal width = size.width() / 100;
    qreal height = size.height() /100;
    QString strSize ("Size: ");
    strSize.append(QString::number(width,'f',2));
    strSize.append("m X ");
    strSize.append(QString::number(height,'f',2));
    strSize.append("m");
    m_lblSize.setText(strSize);
}

void MainWindow::on_Add_Shaft()
{
    CShaft * p = new CShaft;
    p->AddThis(m_pView);
}

void MainWindow::on_Add_Pipe()
{

    CPipe * p = new CPipe;
    p->AddThis(m_pView);

}

void MainWindow::on_btnAdd_clicked()
{
    QPoint pos(ui->btnAdd->mapToGlobal(ui->btnAdd->rect().bottomLeft()));
    m_pAddPassiveMenu->popup(pos);

}
void MainWindow::onSaveAs()
{
    QString strDir = QCoreApplication::applicationDirPath();
    QString fname = QFileDialog::getSaveFileName(this,tr("Save diagram as"),strDir,tr("OptiPlanner (*.opl)"));
    if (fname.isEmpty())
        return;
    m_EditedFile = fname;
    onSave();

}

void MainWindow::onOpen()
{

    switch (QMessageBox::question(this,"Open file","Save current diagram ?",QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
    {
        case QMessageBox::Yes:
                onSave();
                break;
        case QMessageBox::Cancel:
                return;
    }

    QString strDir = QCoreApplication::applicationDirPath();
    QString fname = QFileDialog::getOpenFileName(this,tr("Open diagram"),strDir,tr("OptiPlanner (*.opl)"));
    if (fname.isEmpty())
        return;

    QFile loadFile(fname);
    QJsonObject::const_iterator itemObject;
    CElement *pEl;


    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open file.");
        return;
    }



    QJsonObject objSave(QJsonDocument::fromBinaryData(loadFile.readAll()).object());

    ConfigureProgressBar(0,objSave.size());
    m_pView->ClearScene();

    m_pView->setSaveObject(objSave.value("infra").toObject());

   // m_pView->InsertGrid();

    int cnt = 0;
    for (itemObject = objSave.begin(); itemObject != objSave.end(); itemObject++)
    {
        pEl = createElement((CElement::ElementType) (*itemObject).toObject()["e_type"].toInt());
        if (pEl)
        {
            pEl->LoadElement((*itemObject).toObject());
            pEl->setView(m_pView);
            pEl->setDirty(false);
            m_pScene->addItem(pEl);
            cnt++;
            m_ProgressBar.setValue(cnt);

        }
    }
    ui->statusBar->showMessage("Diagram loaded.",5000);
    ConfigureStatusBar();
    m_ProgressBar.setVisible(false);
    m_EditedFile = fname;
}

void MainWindow::onSave()
{

    if (m_EditedFile.isEmpty())
    {
        onSaveAs();
        return;
    }
    QJsonObject objSave;
    objSave.insert("infra",m_pView->getSaveObject());
    QList<QGraphicsItem *> items = m_pScene->items();
    QString name;
    CElement *pEl;
    ConfigureProgressBar(0,items.count()-1);
    for (int i = items.count()-1; i>=0; i--)
    {
        name = QString::number(i,10);
        pEl = dynamic_cast<CElement *>(items.at(i));
        if (pEl->elementType() != CElement::StaticElement)
            objSave.insert(name,pEl->getSaveValue());
        m_ProgressBar.setValue(i);
    }
    QJsonDocument savedoc(objSave);
    QFile savefile(m_EditedFile);

    if (!savefile.open(QIODevice::WriteOnly))
    {
              qWarning("Couldn't open save file.");
              return ;
    }
    savefile.write(savedoc.toBinaryData());
    ConfigureStatusBar();
    ui->statusBar->showMessage("Diagram saved",5000);
    m_ProgressBar.setVisible(false);


}

void MainWindow::setSelectedElement(CElement *pEl)
{
    clearSelectedElement();
    m_EProp = pEl->getElementProperties();
    if (!m_EProp)
        return;
    int s = m_EProp->count();
    QTableWidget *pTbl = ui->tblProperty;

    for (int i = 0; i <s;i++)
    {
        pTbl->insertRow(i);
        pTbl->setItem(i,0,new QTableWidgetItem(m_EProp->at(i)->Name));
        pTbl->setCellWidget(i,1,m_EProp->at(i)->pEditor);
    }
    pTbl->setEnabled(true);
}

void MainWindow::clearSelectedElement()
{
    ui->tblProperty->clearContents();
    while (ui->tblProperty->rowCount())
        ui->tblProperty->removeRow(0);
    ui->tblProperty->setEnabled(false);
    if (m_EProp)
        delete  m_EProp;
    m_EProp = nullptr;
}

CAdvGraphicsView *MainWindow::pView() const
{
    return m_pView;
}

void MainWindow::ResizeScene(QRectF &newsz)
{
\
    QRectF scenesize = m_pScene->sceneRect();
    QPolygonF cut;
    if (newsz.width() < scenesize.width() || newsz.height() < scenesize.height())
    {
        //calc cropped area
        cut << QPointF(newsz.width(),0) << QPointF(scenesize.width(),0) << scenesize.bottomRight();
        cut << QPointF(newsz.height(),scenesize.width()) << newsz.bottomRight() << QPointF(newsz.width(),0);

        //select items in the cropped area
        QList<QGraphicsItem *> items = m_pScene->items(cut,Qt::IntersectsItemShape);
        if (items.count() > 1) //Ignore the grid
        {
            switch (
            QMessageBox::question(this,"Resizing diagram","After the resize, the diagram will contain items outside the new size. What would you like to do with these items ? \n \
Select YES to delete these items.\n \
Select NO to move these items at bottom right corner of the diagram. \n \
Select CANCEL to abandon diagram resizing.",
                                  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel)
                    ) //the switch value
            {
                case QMessageBox::Cancel :
                    return;
                case QMessageBox::Yes : //Remove items outside
                    while (items.count() >0)
                        m_pScene->removeItem(items.takeAt(0));
                    break;
            case QMessageBox::No:
                for (int i = 0; i < items.count(); i++)
                    items.at(i)->moveBy((scenesize.width() - newsz.width()) * -1,(scenesize.height() -newsz.height()) * -1);
                 break;
            } // switch

        } //if (items.count() > 0)

    }
    m_pView->RemoveGrid();
    m_pScene->setSceneRect(newsz);
    m_pView->InsertGrid();
    ConfigureStatusBar();

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_pView->Closing(event);
    QMainWindow::closeEvent(event);
}

QString MainWindow::DiagramName() const
{
    return m_DiagramName;
}

void MainWindow::setDiagramName(const QString &DiagramName)
{
    m_DiagramName = DiagramName;
}
