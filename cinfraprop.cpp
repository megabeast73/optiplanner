#include "cinfraprop.h"
#include "ui_cinfraprop.h"
#include "globals.h"
#include "mainwindow.h"
#include "cadvgraphicsview.h"
#include <qcolordialog.h>



CInfraProp::CInfraProp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInfraProp)
{
    ui->setupUi(this);
    setFixedSize(width(), height());
}

CInfraProp::~CInfraProp()
{
    delete ui;
}
void CInfraProp::open()
{
    ui->txtName->setText(g_MainWindow->DiagramName());
    CAdvGraphicsView *pView = g_MainWindow->pView();
    QRect r = pView->sceneRect().toRect();
    ulong s = r.width() / 100;
    ui->spnWidth->setValue(s);
    s = r.height() / 100;
    ui->spnHeight->setValue(s);
    m_bgColor = g_MainWindow->pView()->getBackColor();
    loadColors();
    QDialog::open();

}
void CInfraProp::loadColors()
{
    const QStringList colorNames= { "White","Black","Cyan","DarkCyan","red","darkRed","magenta","darkMagenta","green",
                                    "darkGreen","yellow","darkYellow","blue","darkBlue","grey","darkGrey","lightGrey"};
    QModelIndex idx;
    int sel = -1;
    int index = 0;
    ui->cbBack->clear();
    foreach (const QString &colorName, colorNames)
    {

        const QColor color(colorName);
        if (m_bgColor == color)
            sel = index;
        ui->cbBack->addItem(colorName, color);
        //index++;
        idx = ui->cbBack->model()->index(index++, 0);
        ui->cbBack->model()->setData(idx, color, Qt::BackgroundColorRole);

    }
    ui->cbBack->addItem("Custom");
    index++;
    idx = ui->cbBack->model()->index(index, 0);
    ui->cbBack->model()->setData(idx, m_bgColor, Qt::BackgroundColorRole); //Set dummy color
    if (sel < 0) //Default color is not selected. Select "custom"
    {
        ui->cbBack->model()->setData(idx, m_bgColor.name(QColor::HexArgb), Qt::BackgroundColorRole);
        ui->cbBack->setCurrentIndex(index);
    }
    else
        ui->cbBack->setCurrentIndex(sel);
}
void CInfraProp::on_btnApply_pressed()
{
    QRectF nesz (0,0,ui->spnWidth->value() * 100,ui->spnHeight->value() * 100);
    g_MainWindow->ResizeScene(nesz);
    g_MainWindow->setDiagramName(ui->txtName->text());
    QColor bg;
    g_MainWindow->ConfigureStatusBar();
    if (ui->cbBack->currentText().compare("Custom"))
        bg = QColor(ui->cbBack->currentText()); //Custom is not selected
    else {
        bg = QColor(ui->cbBack->itemData(ui->cbBack->count()-1).toString());
    }
    if (!bg.isValid())
        bg = QColor("darkGrey");
    g_MainWindow->pView()->setBackColor(bg);

    close();

}

void CInfraProp::on_btnClose_clicked()
{
    close();
}
void CInfraProp::closeEvent(QCloseEvent *event)
{
    QDialog::closeEvent(event);
    g_MainWindow->pView()->ClosePropDialog();
}

void CInfraProp::on_btnSelectColor_clicked()
{

    QColor tmp = QColorDialog::getColor(m_bgColor,this,"Select diagram background color");
    if (!tmp.isValid())
        return;
    m_bgColor = tmp;



}
