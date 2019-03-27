#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cadvgraphicsview.h"
#include <qlabel.h>
#include <qprogressbar.h>


class QGraphicsView;
class QGraphicsScene;
class CElement;
class ElementProperties;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void ConfigureMenus();
    void ConfigureStatusBar();
    void ConfigureProgressBar(int min,int max);

    void setSelectedElement(CElement *pEl);
    void clearSelectedElement();



    CAdvGraphicsView *pView() const;

    void ResizeScene(QRectF &newsz);
    QString DiagramName() const;
    void setDiagramName(const QString &DiagramName);

protected:
    void closeEvent(QCloseEvent *event) override;

protected:

    CAdvGraphicsView *m_pView;
    QGraphicsScene *m_pScene;
    QMenu * m_pAddPassiveMenu;
    QString m_EditedFile;
    QString m_DiagramName;

    QLabel m_lblName;
    QLabel m_lblSize;
    QLabel m_CurrentView;
    QProgressBar m_ProgressBar;

    ElementProperties * m_EProp;



    void keyPressEvent(QKeyEvent *event);


private slots:
    void on_btnAdd_clicked();
    void on_Add_Shaft();
    void on_Add_Pipe();
    void onSaveAs();
    void onSave();
    void onOpen();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
