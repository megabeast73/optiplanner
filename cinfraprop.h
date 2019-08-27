#ifndef CINFRAPROP_H
#define CINFRAPROP_H

#include <QDialog>
#include <QComboBox>

namespace Ui {
class CInfraProp;
}

class CInfraProp : public QDialog
{
    Q_OBJECT

public:
    explicit CInfraProp(QWidget *parent = nullptr);
    ~CInfraProp() override;
    virtual int exec() override;

    static void configureColorCombo(QComboBox *pBox, const QColor& defColor);

protected:
    void closeEvent(QCloseEvent *event) override;
    void loadColors();



private slots:
    void on_btnApply_pressed();

    void on_btnClose_clicked();

    void on_btnSelectColor_clicked();

    void on_btnSelectGridColor_clicked();

    void on_btnInfraSelectScaleColor_clicked();

private:
    Ui::CInfraProp *ui;

protected:
    QColor m_bgColor,m_GridColor,m_InfraScaleColor;
};

#endif // CINFRAPROP_H
