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
int CInfraProp::exec()
{
    ui->txtName->setText(g_MainWindow->DiagramName());

    QRect r = g_MainWindow->InfraScene()->sceneRect().toRect();//pView->sceneRect().toRect();
    ulong s = r.width() / 100;

    ui->spnWidth->setValue(s);
    s = r.height() / 100;
    ui->spnHeight->setValue(s);

    m_bgColor = g_MainWindow->getInfraBack().color();
    m_GridColor = g_MainWindow->InfraScene()->getGridColor();
    m_InfraScaleColor = g_MainWindow->InfraScene()->getScaleColor();

    ui->chkInfraGridVisible->setCheckState(g_MainWindow->InfraScene()->getGridVisible() ? Qt::Checked : Qt::Unchecked);
    loadColors();
    return QDialog::exec();

}

void CInfraProp::configureColorCombo(QComboBox *pBox, const QColor& defColor)
{
    const QStringList colorNames= { "White","Black","Cyan","DarkCyan","red","darkRed","magenta","darkMagenta","green",
                                    "darkGreen","yellow","darkYellow","blue","darkBlue","grey","darkGrey","lightGrey"};
    QModelIndex idx;
    int sel = -1;
    int index = 0;
    pBox->clear();
    foreach (const QString &colorName, colorNames)
    {

        const QColor color(colorName);
        //Infra back color
        if (defColor == color)
            sel = index;

        //index++;
        pBox->addItem(colorName, color);
        idx = pBox->model()->index(index, 0);
        pBox->model()->setData(idx, color, Qt::BackgroundColorRole);
        index++;
    }
    //Add "custom" and chek if it is selected for infra back
   pBox->addItem("Custom");
    idx = pBox->model()->index(index, 0);
    pBox->model()->setData(idx, defColor, Qt::BackgroundColorRole); //Set dummy color
    if (sel < 0) //Default color is not selected. Select "custom"
    {
        pBox->model()->setData(idx, defColor, Qt::BackgroundColorRole);
        pBox->setCurrentIndex(index);
    }
    else
        pBox->setCurrentIndex(sel);
}


void CInfraProp::loadColors()
{

    configureColorCombo(ui->cbBack,m_bgColor);
    configureColorCombo(ui->cbGridColor,m_GridColor);
    configureColorCombo(ui->cbInfraScaleColor,m_InfraScaleColor);

}
void CInfraProp::on_btnApply_pressed()
{
    QColor bg;
    QRectF nesz (0,0,ui->spnWidth->value() * 100,ui->spnHeight->value() * 100);

    g_MainWindow->ResizeScene(nesz);
    g_MainWindow->setDiagramName(ui->txtName->text());
    g_MainWindow->ConfigureStatusBar();

    if (ui->cbBack->currentText().compare("Custom"))
        bg = QColor(ui->cbBack->currentText()); //Custom is not selected
    else {
        bg = m_bgColor;
    }
    if (!bg.isValid())
        bg = QColor("darkGrey");
    g_MainWindow->setInfraBack(bg);

    //Infra GridColor
    bg = QColor();
    if (ui->cbGridColor->currentText().compare("Custom"))
        bg = QColor(ui->cbGridColor->currentText()); //Custom is not selected
    else {
        bg = m_GridColor;
    }
    if (!bg.isValid())
        bg = QColor("black");
    g_MainWindow->InfraScene()->setGridColor(bg);

    //Grid visibles
    g_MainWindow->InfraScene()->setGridVisible(ui->chkInfraGridVisible->checkState() == Qt::Checked);

    //Infra Scale Color
    bg = QColor();
    if (ui->cbInfraScaleColor->currentText().compare("Custom"))
        bg = QColor(ui->cbInfraScaleColor->currentText()); //Custom is not selected
    else {
        bg = m_InfraScaleColor;
    }
    if (!bg.isValid())
        bg = QColor(Qt::yellow);
    g_MainWindow->InfraScene()->setScaleColor(bg);



    close();

}

void CInfraProp::on_btnClose_clicked()
{
    close();
}
void CInfraProp::closeEvent(QCloseEvent *event)
{
    QDialog::closeEvent(event);
}

void CInfraProp::on_btnSelectColor_clicked()
{

    QColor tmp = QColorDialog::getColor(m_bgColor,this,"Infrastructure diagram background color");
    if (!tmp.isValid())
        return;
    m_bgColor = tmp;
    loadColors();



}

void CInfraProp::on_btnSelectGridColor_clicked()
{
    QColor tmp = QColorDialog::getColor(m_GridColor,this,"Infrastructure grid color");
    if (!tmp.isValid())
        return;
    m_GridColor = tmp;
    loadColors();

}

void CInfraProp::on_btnInfraSelectScaleColor_clicked()
{
    QColor tmp = QColorDialog::getColor(m_InfraScaleColor,this,"Infrastructure scaletext color");
    if (!tmp.isValid())
        return;
    m_InfraScaleColor = tmp;
    loadColors();
}
