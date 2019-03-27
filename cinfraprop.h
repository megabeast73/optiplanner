#ifndef CINFRAPROP_H
#define CINFRAPROP_H

#include <QDialog>

namespace Ui {
class CInfraProp;
}

class CInfraProp : public QDialog
{
    Q_OBJECT

public:
    explicit CInfraProp(QWidget *parent = nullptr);
    ~CInfraProp() override;
    virtual void open() override;

protected:
    void closeEvent(QCloseEvent *event) override;
    void loadColors();

private slots:
    void on_btnApply_pressed();

    void on_btnClose_clicked();

    void on_btnSelectColor_clicked();

private:
    Ui::CInfraProp *ui;

protected:
    QColor m_bgColor;
};

#endif // CINFRAPROP_H
