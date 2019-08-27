#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDockWidget>
#include <cadvgraphicsview.h>
#include <cadvscene.h>

#include "celement.h"
#include "cshaft.h"
#include "cpipe.h"
#include "ccenter.h"

#include <qfiledialog.h>
#include <qapplication.h>
#include <QJsonDocument>
#include <QMessageBox>
#include <QCompleter>
#include "cinfraprop.h"

#include "globals.h"
#include "undocommands.h"
#include "routefinderdialog.h"



#include "qsizepolicy.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);


    m_EProp = nullptr;
    routeFinder = nullptr;

    //Initialize view
    m_pView = new CAdvGraphicsView;
    ui->centralWidget->layout()->addWidget(m_pView);


    //Initialize Infra scene
    m_InfraBack.setColor(Qt::darkGray);
    m_InfraBack.setStyle(Qt::SolidPattern);
    m_pInfraScene = new CAdvScene;
    m_pInfraScene->setSceneRect(QRectF(0.0, 0.0,100000,100000));
    m_pView->setBackBrush(m_InfraBack);
    m_pView->setScene(m_pInfraScene);
    m_CurrentView = scene_infra;
    //Do not forget to push the buttons
    ui->btnInfra->setChecked(true);
    ui->btnWiring->setChecked(false);

    //Initalize Wirings
    m_WiringScene = new CAdvScene;
    m_WiringBack.setColor(Qt::darkGray);
    m_WiringScene->setSceneRect(QRectF(0,0,100000,100000));

    //Initalize UI
    setDiagramName("New diagram");

    ConfigureProgressBar(0,100);

    m_ProgressBar.setValue(0);
    ui->statusBar->addWidget(&m_lblName);
    ui->statusBar->addWidget(&m_lblSize);
    ui->statusBar->addWidget(&m_CurrentViewName);
    ui->statusBar->addWidget(&m_ProgressBar);
    //ui->statusBar->showMessage("Opti Planner started!",5000);

    ConfigureStatusBar();
    m_ProgressBar.setVisible(false);

    g_UndoStack = new QUndoStack(this);

    ConfigureMenus();
    clearSelectedElement();
    g_Objects.setObjectSelector(ui->cmbEList);
    ui->cmbEList->completer()->setFilterMode(Qt::MatchContains);
    clearObjectSelector();
    m_pView->setFocus();
    m_pView->setInteractive(true);

}

MainWindow::~MainWindow()
{
    if (routeFinder)
        delete routeFinder;
    delete g_UndoStack;
    delete m_pAddPassiveMenu;
    delete m_pView;
    delete m_pInfraScene;
    delete m_WiringScene;
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
    m_pAddPassiveMenu->addAction(QIcon(":/IMAGES/CONTROL_CENTER_ICON"),"Control center",this,&MainWindow::on_Add_CC);
    m_pAddPassiveMenu->addAction("Fiber");
    m_pAddPassiveMenu->addAction("Closure");
    m_pAddPassiveMenu->addAction("Connector");
    m_pAddPassiveMenu->addAction("Active applience");


    connect(ui->actionSaveAs,&QAction::triggered,this,&MainWindow::onSaveAs);
    connect(ui->actionOpen,&QAction::triggered,this,&MainWindow::onOpen);
    connect(ui->actionSave,&QAction::triggered,this,&MainWindow::onSave);
    connect(ui->actionQuit,&QAction::triggered,this,&MainWindow::close);
    connect(ui->actionRoute_finder,&QAction::triggered,this,&MainWindow::onRouteFinnder);



    pTmp = ui->menuDiagram->addMenu("View");
    pTmp->addAction(QIcon(":/IMAGES/INFRA_DIAGRAM"),"Infrastructure",this,&MainWindow::on_btnInfra_clicked,Qt::CTRL+Qt::Key_I);
    pTmp->addAction(QIcon(":/IMAGES/WIRING_ICON"),"Wiring",this,&MainWindow::on_btnWiring_clicked,Qt::CTRL+Qt::Key_W);
    ui->menuDiagram->addAction("Properies",this,&MainWindow::ShowPropDialog);

    QAction * pAction;


    pAction = g_UndoStack->createUndoAction(this,tr("&Undo"));
    pAction->setShortcuts(QKeySequence::Undo);
    ui->menuEdit->addAction(pAction);

    pAction = g_UndoStack->createRedoAction(this,tr("&Redo"));
    pAction->setShortcut(QKeySequence::Redo);
    ui->menuEdit->addAction(pAction);
}

void MainWindow::ConfigureStatusBar()
{
    QRectF size = m_pView->scene()->sceneRect();
    qreal width = size.width();
    qreal height = size.height();
    QString strSize ("Size: ");

    if (m_CurrentView == scene_infra)
    {
        m_CurrentViewName.setText("Infrastructure");
        strSize.append(QString::number(width /100,'f',2));
        strSize.append("m X ");
        strSize.append(QString::number(height/100,'f',2));
        strSize.append("m");
    }
    else {
        m_CurrentViewName.setText("Wiring");
        strSize.append(QString::number(width,'f',2));
        strSize.append("pts X ");
        strSize.append(QString::number(height,'f',2));
        strSize.append("pts");
    }
    m_lblName.setText(DiagramName());
    m_lblSize.setText(strSize);
}

QJsonObject MainWindow::getOptions()
{
    QJsonObject ret;
    ret.insert("infra_width",QJsonValue(m_pInfraScene->sceneRect().width()));
    ret.insert("infra_height",QJsonValue(m_pInfraScene->sceneRect().height()));
    ret.insert("infra_name",QJsonValue(DiagramName()));
    ret.insert("infra_bgcolor",QJsonValue(m_InfraBack.color().name(QColor::HexArgb)));
    ret.insert("infra_grid_visible",QJsonValue(m_pInfraScene->getGridVisible()));
    ret.insert("infra_grid_color",QJsonValue(m_pInfraScene->getGridColor().name(QColor::HexArgb)));
    //Add functionality to save the currently edited scene
    return ret;
}

void MainWindow::setOptions(QJsonObject &obj)
{

    CAdvScene * pS = dynamic_cast<CAdvScene*>(m_pInfraScene);
    QRectF sz;
    sz.setWidth(obj["infra_width"].toDouble());
    sz.setHeight(obj["infra_height"].toDouble());
    pS->setSceneRect(sz);

    setDiagramName(obj["name"].toString());

    QColor bg;
    bg.setNamedColor(obj["infra_bgcolor"].toString());
    m_InfraBack.setColor(bg);

    m_pInfraScene->setGridVisible(obj["infra_grid_visible"].toBool());
    bg.setNamedColor(obj["infra_grid_color"].toString());
    m_pInfraScene->setGridColor(bg);


//Add functionality to load last used scene
    m_pView->setBackBrush(m_InfraBack);
   ConfigureStatusBar();

}

void MainWindow::setViewScene(SceneView v)
{

    if ( v == m_CurrentView)
        return;
    m_CurrentView = v;
    if (v == scene_infra)
    {
        m_pView->setScene(m_pInfraScene);
        m_pView->setBackgroundBrush(m_InfraBack);
        ui->btnInfra->setChecked(true);
        ui->btnWiring->setChecked(false);
    }
    else {
        m_pView->setScene(m_WiringScene);
        m_pView->setBackgroundBrush(m_WiringBack);
        ui->btnInfra->setChecked(false);
        ui->btnWiring->setChecked(true);
    }
    ConfigureStatusBar();
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

void MainWindow::showStatusMessage(const QString& msg, int time)
{
    ui->statusBar->showMessage(msg,time);
}

void MainWindow::onOpen()
{
    if (g_UndoStack->index() != 0)
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
    ui->statusBar->showMessage("Loading diagram.",0);

    //Clear
    InfraScene()->clear();
    WiringScene()->clear();
    clearObjectSelector();
    m_EditedFile = fname;
    g_UndoStack->clear();
    //Open file and load
    QJsonObject objSave(QJsonDocument::fromBinaryData(loadFile.readAll()).object());
    m_DiagramName = objSave.value("name").toString();

    ConfigureProgressBar(0,objSave.size());

    InfraScene()->setSaveValue(objSave.value("infra").toObject());
    WiringScene()->setSaveValue(objSave.value("wiring").toObject());

    int cnt = 0;
    for (itemObject = objSave.constBegin(); itemObject != objSave.constEnd(); itemObject++)
    {
        if (!itemObject.key().contains("e_infra"))
            continue;
        pEl = createElement((CElement::ElementType) (*itemObject).toObject()["e_type"].toInt());
        if (pEl)
        {
            pEl->LoadElement((*itemObject).toObject());
            if (itemObject.key().contains("e_infra"))
                m_pInfraScene->addItem(pEl);
            else {
                m_WiringScene->addItem(pEl);
            }
            cnt++;
            m_ProgressBar.setValue(cnt);

        }
    }
    //Reconnect all elements after loading
    ui->statusBar->showMessage("Connecting elements.",0);
    int sz = g_Objects.count();
    ConfigureProgressBar(0,sz);
    for (int i = 0; i < sz; ++i)
    {
        g_Objects.elementAt(i)->reconnectElement();
        m_ProgressBar.setValue(cnt);
    }


    ui->statusBar->clearMessage();
    ui->statusBar->showMessage("Diagram loaded.",5000);
    ConfigureStatusBar();
    m_ProgressBar.setVisible(false);
    clearSelectedElement();
}

void MainWindow::onSave()
{

    if (m_EditedFile.isEmpty())
    {
        onSaveAs();
        return;
    }
    QJsonObject objSave;
    objSave.insert("infra",InfraScene()->getSaveValue());
    objSave.insert("wiring",WiringScene()->getSaveValue());
    objSave.insert("name",m_DiagramName);

    //save Infrastructure diagram
    QList<QGraphicsItem *> items = m_pInfraScene->items();
    QString name;
    CElement *pEl;
    ConfigureProgressBar(0,items.count()-1);
    for (int i = items.count()-1; i>=0; i--)
    {
        name = QString::number(i,10);
        name.prepend("e_infra_");
        pEl = dynamic_cast<CElement *>(items.at(i));
        if (pEl->elementType() != CElement::StaticElement)
            objSave.insert(name,pEl->getSaveValue());
        m_ProgressBar.setValue(i);
    }
    //Do not forget to save the wiring diagram
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

CAdvScene* MainWindow::WiringScene() const
{
    return m_WiringScene;
}
CAdvGraphicsView* MainWindow::pView() const
{
    return m_pView;
}

QString MainWindow::DiagramName() const
{
    return m_DiagramName;
}

void MainWindow::setDiagramName(const QString &DiagramName)
{
    m_DiagramName = DiagramName;
}

void MainWindow::ShowPropDialog()
{
    CInfraProp dlg;
    dlg.exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMainWindow::closeEvent(event);
}

QBrush MainWindow::getWiringBack() const
{
    return m_WiringBack;
}

void MainWindow::setWiringBack(const QBrush &WiringBack)
{
    m_WiringBack = WiringBack;
    if (m_CurrentView == scene_wiring)
        m_pView->setBackBrush(WiringBack);
}

QBrush MainWindow::getInfraBack() const
{
    return m_InfraBack;
}

void MainWindow::setInfraBack(const QBrush &InfraBack)
{
    m_InfraBack = InfraBack;
    if (m_CurrentView == scene_infra)
    {
        m_pView->setBackBrush(InfraBack);
    }
}
void MainWindow::on_btnWiring_clicked()
{
    setViewScene(scene_wiring);
}

void MainWindow::on_btnInfra_clicked()
{
    setViewScene(scene_infra);
}
//===============================================


void MainWindow::on_Add_Shaft()
{
    CShaft * p = new CShaft;

    m_pInfraScene->addElement(p);

}

void MainWindow::on_Add_Pipe()
{

    CPipe * p = new CPipe;
    m_pInfraScene->addElement(p);
}
void MainWindow::on_Add_CC()
{
    m_pInfraScene->addElement(new CCenter);
}

void MainWindow::on_btnAdd_clicked()
{
    QPoint pos(ui->btnAdd->mapToGlobal(ui->btnAdd->rect().bottomLeft()));
    m_pAddPassiveMenu->popup(pos);

}

void MainWindow::ResizeScene(QRectF &newsz)
{
\
    QRectF scenesize = m_pInfraScene->sceneRect();
    QPolygonF cut;
    if (newsz.width() < scenesize.width() || newsz.height() < scenesize.height())
    {
        //calc cropped area
        cut << QPointF(newsz.width(),0) << QPointF(scenesize.width(),0) << scenesize.bottomRight();
        cut << QPointF(newsz.height(),scenesize.width()) << newsz.bottomRight() << QPointF(newsz.width(),0);

        //select items in the cropped area
        QList<QGraphicsItem *> items = m_pInfraScene->items(cut,Qt::IntersectsItemShape);
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
                        m_pInfraScene->removeItem(items.takeAt(0));
                    break;
            case QMessageBox::No:
                for (int i = 0; i < items.count(); i++)
                    items.at(i)->moveBy((scenesize.width() - newsz.width()) * -1,(scenesize.height() -newsz.height()) * -1);
                 break;
            } // switch

        } //if (items.count() > 0)

    }

    m_pInfraScene->setSceneRect(newsz);

    ConfigureStatusBar();

}


//Sets the element into properties window
void MainWindow::setSelectedElement(CElement *pEl)
{
    if (!pEl) //In case od element is not invalid
        return;
    clearSelectedElement();
    m_EProp = pEl->getElementProperties();
//    if (!m_EProp)
//        return;
    int s = m_EProp->count();
    QTableWidget *pTbl = ui->tblProperty;
    QTableWidgetItem *pItem;

    for (int i = 0; i <s;i++)
    {
        pTbl->insertRow(i);
        pItem = new QTableWidgetItem(m_EProp->at(i)->Name);
        pItem->setFlags(pItem->flags() & ~Qt::ItemIsEditable);
        pItem->setToolTip(m_EProp->at(i)->Name);
        pTbl->setItem(i,0,pItem);
        pTbl->setCellWidget(i,1,m_EProp->at(i)->pEditor);
    }
    if (pTbl->rowCount() > 0)
    {
        pTbl->setEnabled(true);
        ui->btnApply->setEnabled(true);
        ui->btnReload->setEnabled(true);
    }
}
//Clears the properties window
void MainWindow::clearSelectedElement()
{
    ui->tblProperty->clearContents();
    while (ui->tblProperty->rowCount())
        ui->tblProperty->removeRow(0);
    ui->tblProperty->setEnabled(false);
    if (m_EProp)
        delete  m_EProp;
    m_EProp = nullptr;
    ui->btnApply->setEnabled(false);
    ui->btnReload->setEnabled(false);
    ui->tblProperty->setEnabled(false);
}

void  MainWindow::clearObjectSelector()
{
    ui->cmbEList->clear();
}


void MainWindow::on_btnApply_clicked()
{
    ITEMLIST pElement;
    pElement.push_back(m_EProp->Element());
    CUndoPropChange *pUndo = new CUndoPropChange(pElement);
    pUndo->setText("Change element property");
    m_EProp->Element()->setElementProperties(m_EProp);
    pUndo->finishPropChange();
    g_UndoStack->push(pUndo);

}

void MainWindow::on_btnReload_clicked()
{
    setSelectedElement(m_EProp->Element());
}

void MainWindow::on_cmbEList_currentIndexChanged(int index)
{

    int eId = ui->cmbEList->itemData(index).toInt();
    setSelectedElement(g_Objects.elementById(eId));
}

void MainWindow::setElementToObjectSelector(CElement * pEl)
{

    int eid = pEl->ElementID();
    setSelectedElement(pEl);
}




void MainWindow::on_btnCenterOn_clicked()
{
    if (!ui->cmbEList->currentData().isValid())
        return;
    int eid = ui->cmbEList->currentData().toInt();
    CElement * pEl = g_Objects.elementById(eid);
    if (!pEl)
        return; // object not found
    m_pInfraScene->clearSelection();
    pEl->setSelected(true);
    m_pView->centerOn(pEl);
    setSelectedElement(pEl);
    setElementToObjectSelector(pEl);
};

void MainWindow::onRouteFinnder()
{

    if (!routeFinder)
        routeFinder = new RouteFinderDialog(this);
    routeFinder->show();
    routeFinder->setFocus();

}
