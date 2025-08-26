#include "qlogoutput.h"


QString  QLogOutput::m_strLogFile = "";


QLogOutput::QLogOutput(QObject *parent) : QObject(parent)
{

}



void QLogOutput::outPutMsg(QtMsgType msgType, const QMessageLogContext &context, const QString &strText)
{

    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = strText.toLocal8Bit();

    QString strMsg("");
    QString strMessage;
    // 设置输出信息格式
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");


    switch(msgType)
    {
    case QtDebugMsg:
    {
        strMsg = QString("Debug:");
         strMessage = QString("[%1] %5 %6\t--File:%2  Line:%3  Function:%4  ")
                .arg(strDateTime).arg(context.file).arg(context.line).arg(context.function).arg(strMsg).arg(strText);
    }
        break;
    case QtInfoMsg:
    {
         strMsg = QString("Info:");
         strMessage = QString("[%1] %2 %3\t").arg(strDateTime).arg(strMsg).arg(strText);
    }
        break;
    case QtWarningMsg:
    {
        strMsg = QString("Warning:");
        strMessage = QString("[%1] %5 %6\t--File:%2  Line:%3  Function:%4  ")
               .arg(strDateTime).arg(context.file).arg(context.line).arg(context.function).arg(strMsg).arg(strText);
    }
        break;
    case QtCriticalMsg:
    {
        strMsg = QString("Critical:");
        strMessage = QString("[%1] %5 %6\t--File:%2  Line:%3  Function:%4  ")
               .arg(strDateTime).arg(context.file).arg(context.line).arg(context.function).arg(strMsg).arg(strText);
    }
        break;
    case QtFatalMsg:
    {
        strMsg = QString("Fatal:");
        strMessage = QString("[%1] %5 %6\t--File:%2  Line:%3  Function:%4  ")
               .arg(strDateTime).arg(context.file).arg(context.line).arg(context.function).arg(strMsg).arg(strText);
    }
        break;
    }

    if(true==m_UI_Debug_Flag)
    {
        SaveLog(strMessage);
    }
    // 解锁
    mutex.unlock();
}


void QLogOutput::install(QString strLogFile)
{
    qInstallMessageHandler(outPutMsg);
    QString log_dir = strLogFile+"/Debug_log";
    QDir dir(log_dir);
    if(!dir.exists())
    {
        //创建在可执行程序所在目录创建log文件
        dir.mkdir(log_dir);
    }
    m_strLogFile = log_dir;

}


void QLogOutput::uninstall()
{
    qInstallMessageHandler(0);
}

int QLogOutput::SaveLog(const QString& strMsg)
{
    QString log_fileName;
    QDateTime current_date_time =QDateTime::currentDateTime();
    QString date = current_date_time.toString("yyyy-MM-dd");
    log_fileName = m_strLogFile +"/"+"ATE_UI_Log_"+date+".txt";
    // 输出信息至文件中（读写、追加形式）
    QFile file(log_fileName);
    bool bOpen = file.open(QIODevice::ReadWrite | QIODevice::Append);
    if(!bOpen)
        return  false;
    QTextStream stream(&file);
    stream << strMsg << "\r\n";
    file.flush();
    file.close();
    return  true;
}




