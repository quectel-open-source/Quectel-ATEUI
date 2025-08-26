#ifndef MYTOOLVERSION_H
#define MYTOOLVERSION_H

#include <QObject>
#include<qdialog.h>
#include<qpushbutton.h>
#include<QLabel.h>
#include<qline.h>
#include <QLineEdit>
#include <QMouseEvent>
#include <QGroupBox>
#include<QTextEdit>
#include <QScrollBar>
#include <QApplication>

class MyToolVersion: public QDialog
{
    Q_OBJECT
public:
    explicit MyToolVersion(QWidget *parent = nullptr,QString testscript_version="",QString dll_version="");

    void UIinit();

    QPushButton *m_ok_btn;

    QString m_testscript_version;
    QString m_function_dll_version;

    QPoint   mousePoint;
    bool     mouse_press;

private slots:

    void OKBtnClicked();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // MYTOOLVERSION_H
