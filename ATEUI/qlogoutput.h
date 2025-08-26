#ifndef QLOGOUTPUT_H
#define QLOGOUTPUT_H

#include <QObject>
#include <QApplication>
#include <QDir>
#include <QMutex>
#include <QDateTime>
#include <QTextStream>
#include <QFile>

#include "Global_Header.h"


const int LOG_OPERATE_OK = 100; //操作成功
const int LOG_OPENFILE_FAILED = 101; //打开文件失败


class QLogOutput : public QObject
{

    Q_OBJECT

public:
    explicit QLogOutput(QObject *parent = nullptr);


    //安装信息处理函数
    static void install(QString strLogFile);

    //卸载信息处理函数
    static void uninstall();

    //日志信息处理函数
    static void outPutMsg(QtMsgType msgType, const QMessageLogContext &context, const QString &strText);

    //保存日志
    static int SaveLog(const QString &strMsg);

     //保存日志路径
     static QString m_strLogFile;

};

#endif // QLOGOUTPUT_H
