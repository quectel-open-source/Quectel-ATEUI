#ifndef MYCHANNELNUMDLG_H
#define MYCHANNELNUMDLG_H

#include <QObject>
#include <QDialog>
#include <QMouseEvent>
#include <QLabel>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QScrollBar>
#include <QPushButton>


class MyChannelNumDlg : public QDialog
{
    Q_OBJECT
public:
    explicit MyChannelNumDlg(QWidget *parent = nullptr);

    void UIinit();

    QPoint   mousePoint;
    bool     mouse_press;

    QLabel    *channelnumlab;
    QComboBox *channelnumBox;

    QPushButton *okBtn;
    QPushButton *cancelBtn;

signals:

    void sendData(QString data);

private slots:
        void Okclicked();
        void Cancelclicked();
protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

};

#endif // MYCHANNELNUMDLG_H
