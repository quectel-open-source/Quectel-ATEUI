#include "cgfc.h"

CGFC::CGFC()
{

}
wchar_t* CGFC::QString2Wchar(QString buf)
{
    return (wchar_t*)reinterpret_cast<const wchar_t *>(buf.utf16());
}

QString CGFC::Wchar2QString(const wchar_t *buf)
{
    return QString::fromWCharArray(buf);
}
