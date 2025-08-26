#include "ateuimainwindow.h"
#include "logondialog.h"
#include "tradgoldlogondialog.h"

#include <QApplication>
#include <QLockFile>
#include <QProcess>
#include <QStringlist>

#include <windows.h>
#include <tlhelp32.h>

#include <psapi.h>

#include <windows.h>
#include <dbgHelp.h>
#include <DbgHelp.h>
#include <qt_windows.h>
#include <qsettings.h>

#include "Global_Header.h"
#include "mymessagebox.h"
bool    checkProcessByQProcess(const QString &strExe);
QStringList strMid(QString src, QString start, QString end);
QString getProcessPath(DWORD processId);
QString getExeVersion(QString exepath);
void ChooseLogonUI();

bool m_UI_Debug_Flag = false;  // 定义全局变量（实际分配内存）

// 创建全局事件过滤器类
class ComboBoxWheelBlocker : public QObject {
public:
    explicit ComboBoxWheelBlocker(QObject* parent = nullptr) : QObject(parent) {}

protected:
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (event->type() == QEvent::Wheel && qobject_cast<QComboBox*>(obj)) {
            return true; // 阻止所有 QComboBox 的滚轮事件
        }
        return QObject::eventFilter(obj, event);
    }
};

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException){//程式异常捕获
    /*
      ***保存数据代码***
    */
    //创建 Dump 文件
    qDebug()<<"触发异常!";
    QString createPath = QCoreApplication::applicationDirPath()+"/Dumps";
    QDir dir;
    dir.mkpath(createPath);
    createPath=QString("%1/dump_%2.dmp").arg(createPath).arg(QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss_zzz"));
    std::wstring wlpstr = createPath.toStdWString();
    LPCWSTR lpcwStr = wlpstr.c_str();

    HANDLE hDumpFile = CreateFile(lpcwStr,
                                  GENERIC_WRITE,
                                  0,
                                  NULL,
                                  CREATE_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL);
    if( hDumpFile != INVALID_HANDLE_VALUE){
        //Dump信息
        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = FALSE;
        //写入Dump文件内容
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);
    }
    //这里弹出一个错误对话框并退出程序
    return EXCEPTION_EXECUTE_HANDLER;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 创建全局过滤器
    ComboBoxWheelBlocker blocker;
    a.installEventFilter(&blocker);

    SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);//注冊异常捕获函数


    QString fullexepath = QString::fromUtf8(argv[0]);

    QStringList exepathlist = fullexepath.split('\\');

    //这里增加弹框信息
    checkProcessByQProcess(exepathlist.at(exepathlist.count()-1));

    QString version = getExeVersion(fullexepath);
    qDebug() << "Version:" << version;


    //判断是金板制作工具还是测试工具
    ChooseLogonUI();



    // 设置属性


    ATEUIMainWindow w;
    w.show();
    return a.exec();
}

bool checkProcessByQProcess(const QString &strExe)
{
    // 判断进程是否存在
    QProcess tasklist;
    tasklist.start("tasklist",
        QStringList()<< "/NH"
        << "/FO" << "TABLE"
        << "/FI" << QString("IMAGENAME eq %1").arg(strExe));
    tasklist.waitForFinished();
    QString strOutput = tasklist.readAllStandardOutput();
    qInfo()<<strOutput;

    //该字符串已从开始和结束处删除空白，并将内部包括ASCII字符’\t’,’\n’,’\v’,’\f’,’\r’和’ '.替换为‘ ’，如果替换后有两个空格的话，只保留一个空格。
    strOutput=strOutput.simplified();

    qInfo()<<strOutput;

    QStringList pidstr = strMid(strOutput,strExe,"Console");

    qInfo()<<pidstr;

    QStringList ExeFullPathList;
    for(int i=0;i<pidstr.count();i++)
    {
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,0,pidstr.at(i).toInt());      // 获取进程句柄
        wchar_t lpFilename[1024];
        GetModuleFileNameExW(hProcess, NULL, lpFilename, sizeof(lpFilename));
        qDebug() << QString::fromWCharArray(lpFilename);
        ExeFullPathList.append(QString::fromWCharArray(lpFilename));
        CloseHandle(hProcess);
    }

    bool exeflag=false;
    //判断是否有一样的全路径exe 如果有，则不提示需要关闭
    for(int i=0;i<ExeFullPathList.count();i++)
    {
        for(int j=i+1;j<ExeFullPathList.count();j++)
        {
            //判断是否有相同,如果有相同则标志位为1
            if(ExeFullPathList.at(i)==ExeFullPathList.at(j))
            {
                exeflag=true;
            }
        }
    }

    if(exeflag==true)
    {
        //
        MyMessageBox message_box(nullptr,"Error","不能打开同一个目录下的exe");
        message_box.exec();
        exit(0);
    }




//    TCHAR  process_name[MAX_PATH] = {};
//    BOOL Wow64Process;
//    HMODULE hMods[512] = {0};
//    DWORD cbNeeded = 0;
//    TCHAR szModName[MAX_PATH];

//    IsWow64Process(hProcess, &Wow64Process); //判断是32位还是64位进程
//    EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded, Wow64Process?LIST_MODULES_32BIT:LIST_MODULES_64BIT);

//    for (UINT i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
//    {
//        QString tmppath;
//        GetModuleFileNameEx(hProcess, hMods[i], szModName, _countof(szModName));
//        //_tprintf_s(TEXT("%s\n"),szModName);
//        tmppath.sprintf("%s",szModName);
//        qInfo()<<tmppath;
//    }
//    CloseHandle(hProcess);


//    QString tmppath;
//    tmppath = getProcessPath((DWORD)pidstr.at(0).toInt());

    //如果进程存在,则结束进程
}

//截取两字符串中间的字符串
QStringList strMid(QString src, QString start, QString end)
{
//    int startIndex = src.indexOf(start, 0);
//    int endIndex = src.indexOf(end, startIndex);
//    QString subStr = src.mid(startIndex+start.length(), endIndex - startIndex-start.length());
//    return subStr;

    QStringList pidlist;
    int startIndex=0;
    int endIndex=0;
    //
    while(1)
    {
        startIndex = src.indexOf(start, endIndex);
        if(startIndex<0)
        {
            break;
        }
        endIndex = src.indexOf(end, startIndex);

        QString subStr = src.mid(startIndex+start.length(), endIndex - startIndex-start.length());

        pidlist.append(subStr.trimmed());

    }

    return pidlist;
}


QString getProcessPath(DWORD processId)
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
    {
        return "";
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnapshot, &pe))
    {
        CloseHandle(hSnapshot);
        return "";
    }

    QString path;

    do
    {
        if (pe.th32ProcessID == processId)
        {
            path = QString::fromWCharArray(pe.szExeFile);
            break;
        }
        qInfo()<<QString::fromWCharArray(pe.szExeFile);
    } while (Process32Next(hSnapshot, &pe));

    CloseHandle(hSnapshot);

    return path;
}

QString getExeVersion(QString exepath)
{
    QString exePath = exepath; // 替换为你的exe路径
    QFile file(exePath);
    if (!file.exists())
    {
        qWarning() << "File does not exist!";
        return "";
    }

    QSettings setting("HKEY_CLASSES_ROOT\\.exe\\shell\\open\\command", QSettings::NativeFormat);
    QString versionKey = exePath + ",0"; // 对于版本信息，通常是exe路径后面加一个逗号和零
    QString version = setting.value(versionKey).toString();

    return version;
}

//判断是金板制作工具还是测试工具
void ChooseLogonUI()
{
    //根据DataBases文件夹中的配置文件UI_Setting.ini里面的信息来判断是金板制作工具还是测试工具

   QString exeDir = QCoreApplication::applicationDirPath(); // 获取exe所在目录（不含文件名）

   QSettings  *m_UI_Setting_ini; // 指定INI文件路径和格式
   m_UI_Setting_ini = new QSettings(exeDir+"/DataBases/UI_Setting.ini",QSettings::IniFormat);

   QLogOutput::install(exeDir); //设置debuglog所在的路径

   m_UI_Debug_Flag = m_UI_Setting_ini->value("Main/Debug_Log_Flag", false).toBool(); //读取debuglog标志位，用来判断是否打印log

   qInfo()<<"m_UI_Debug_Flag=="<<m_UI_Debug_Flag;

   bool goldenabled = m_UI_Setting_ini->value("Main/Gold_Plate_Tool", false).toBool(); //读取金板制作工具的标志位


   if(true==goldenabled)//如果是金板工具，则进入金板工具的界面
   {
       //为了状态栏有显示图标
       TradGoldLogonDiaLog *tradlogonw = new TradGoldLogonDiaLog();
       tradlogonw->InitExternalAPI();
       tradlogonw->exec();
   }
   else //进入测试工具的界面
   {
       //为了状态栏有显示图标
       LogonDiaLog *logonw = new LogonDiaLog();
       logonw->InitExternalAPI();
       logonw->exec();
   }

}
