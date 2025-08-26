#ifndef CGFC_H
#define CGFC_H

#include<windows.h>
#include<QString>

//GSM频段序列号
typedef enum tagEU_GSM_BAND_IDX
{
    G_BAND_GSM900 = 0,
    G_BAND_DCS1800,
    G_BAND_PCS1900,
    G_BAND_GSM850,
    G_BAND_COUNT,		//GSM频段数量
    G_BAND_SIZE = 6		//GSM频段相关的存储BUFF尺寸（“尺寸”必须比“数量”至少大1, 用于存放结束符）
}EU_GSM_BAND_IDX;

//天线类型序列号
typedef enum tagEU_ANT_TYPE_INDEX
{
    ANT_TYPE_IDX_NDF = -1,
    ANT_TYPE_IDX_MAIN_1 = 0,
    ANT_TYPE_IDX_DIV_1,
    ANT_TYPE_IDX_GPS,
    ANT_TYPE_IDX_WIFI_BLU,
    ANT_TYPE_IDX_DIV_GPS,
    ANT_TYPE_IDX_MULTI_DG,
    ANT_TYPE_IDX_MULTI_M,
    ANT_TYPE_IDX_MULTI_M1,
    ANT_TYPE_IDX_MULTI_M2,
    ANT_TYPE_IDX_NR5GSUB6_1,
    ANT_TYPE_IDX_NR5GSUB6_2,
    ANT_TYPE_IDX_NR5GSUB6_3,
    ANT_TYPE_IDX_NR5GSUB6_4,
    ANT_TYPE_IDX_NR5GSUB6_5,
    ANT_TYPE_IDX_NR5GSUB6_6,
    ANT_TYPE_IDX_NR5GSUB6_7,
    ANT_TYPE_IDX_NR5GSUB6_8,
    ANT_TYPE_IDX_MAIN_2,
    ANT_TYPE_IDX_DIV_2,
    ANT_TYPE_IDX_MULTI_M3,
    ANT_TYPE_IDX_MULTI_M4,
    ANT_TYPE_IDX_WIFI_MIMO,
    ANT_TYPE_IDX_V2X_MAIN,
    ANT_TYPE_IDX_V2X_DIV,
    ANT_TYPE_IDX_DSDA_MAIN,
    ANT_TYPE_IDX_DSDA_DIV,
    ANT_TYPE_IDX_V2X_TRX0,
    ANT_TYPE_IDX_V2X_TRX1,
    ANT_TYPE_IDX_NR5GSUB6_9,
    ANT_TYPE_IDX_NR5GSUB6_10,
    ANT_TYPE_IDX_NR5GSUB6_11,
    ANT_TYPE_IDX_NR5GSUB6_12,
    ANT_TYPE_IDX_WIFI_BLU_GPS,
    ANT_TYPE_IDX_BLU_ONLY,
    ANT_TYPE_IDX_ECALL,
    ANT_TYPE_IDX_WIFI_0,
    ANT_TYPE_IDX_LORA,
    ANT_TYPE_IDX_R2_WIFI_BLU,   //36
    ANT_TYPE_IDX_COUNT,			//天线类型的总数量
    ANT_TYPE_IDX_SIZE = 44,		//“每个天线（所有天线）都有自己的参数存储BUFF”的尺寸（“尺寸”必须比“天线类型的总数量”至少大1, 用于存放结束符）
    ANT_TYPE_BUFF_SIZE = 16		//“每个机型拥有的天线（有限天线）的参数存储BUFF”天线的存储BUFF尺寸（“尺寸”必须比“机型拥有的天线数量”至少大1, 用于存放结束符）
    //天线的存储BUFF尺寸占用内存空间资源太多，存储天线数据的相关结构体未来最好还是要调整为“有限天线”的排序式存储。
}EU_ANT_TYPE_INDEX;


class CGFC
{
public:
    CGFC();


    wchar_t *QString2Wchar(QString buf);
    QString Wchar2QString(const wchar_t *buf);
};

#endif // CGFC_H
