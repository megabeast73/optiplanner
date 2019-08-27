#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cadvgraphicsview.h"
#include "cadvscene.h"
#include <qlabel.h>
#include <qprogressbar.h>


class QGraphicsView;
class QGraphicsScene;
class CElement;
class ElementProperties;
class RouteFinderDialog;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum SceneView
        { scene_infra,
           scene_wiring
         } ;

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void ConfigureMenus();
    void ConfigureStatusBar();
    void ConfigureProgressBar(int min,int max);
    QString DiagramName() const;
    void setDiagramName(const QString &DiagramName);


    void setSelectedElement(CElement *pEl);
    void clearSelectedElement();
    void clearObjectSelector();
    void setElementToObjectSelector(CElement * pEl);



    CAdvGraphicsView *pView() const;
    CAdvScene *  InfraScene() const { return m_pInfraScene; }
    CAdvScene *WiringScene() const;

    //Save & Load related
    QJsonObject getOptions();
    void setOptions(QJsonObject &obj);

    void setViewScene(SceneView v);
    SceneView getViewScene() { return  m_CurrentView; }

    void ShowPropDialog();

    void ResizeScene(QRectF &newsz);

    void showStatusMessage(const QString& msg, int time = 1000);




    QBrush getInfraBack() const;
    void setInfraBack(const QBrush &InfraBack);

    QBrush getWiringBack() const;
    void setWiringBack(const QBrush &WiringBack);

    RouteFinderDialog *routeFinder;

protected:
    void closeEvent(QCloseEvent *event) override;

protected:

    CAdvGraphicsView *m_pView;
    CAdvScene *m_pInfraScene;
    CAdvScene *m_WiringScene;
    QBrush m_InfraBack;
    QBrush m_WiringBack;
    SceneView m_CurrentView;


    QMenu * m_pAddPassiveMenu;
    QString m_EditedFile;
    QString m_DiagramName;

    QLabel m_lblName;
    QLabel m_lblSize;
    QLabel m_CurrentViewName;
    QProgressBar m_ProgressBar;

    ElementProperties * m_EProp;



    void keyPressEvent(QKeyEvent *event);


private slots:
    void on_btnAdd_clicked();
    void on_Add_Shaft();
    void on_Add_Pipe();
    void on_Add_CC();
    void onSaveAs();
    void onSave();
    void onOpen();
    void onRouteFinnder();

    void on_btnApply_clicked();
    void on_btnReload_clicked();
    void on_cmbEList_currentIndexChanged(int index);
    void on_btnWiring_clicked();
    void on_btnInfra_clicked();
    void on_btnCenterOn_clicked();

private:
    Ui::MainWindow *ui;


};

#endif // MAINWINDOW_H
