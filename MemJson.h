/*************************************************************************
** Copyright(c) 2020-2025  faker
** All rights reserved.
** Name		: MemJson.h/cpp
** Desc		: 基于DatX和cJSON的json类，用于实现标准Json字符串与DatX的互转，同时也可以将DatX文本化为“伪Json”字符串
** Author	: faker@2020-11-22 20:21:51
*************************************************************************/

#ifndef _1C845505_09DA_422C_812C_0B582166CA8D
#define _1C845505_09DA_422C_812C_0B582166CA8D

#include "DatX.h"

namespace x2lib
{
    class MemJson : public DatX
    {
    public:
        /*************************************************************************
        ** Desc     : 将json字符串转为DatX
        ** Param    : [in] pJson json字符串
        ** Return   : 是否成功，失败请检查pJson是否为标准格式
        ** Author   : faker@2020-11-22
        *************************************************************************/
        bool Parse(const char* pJson);

        /*************************************************************************
        ** Desc     : 将DatX打印为json字符串
        ** Param    : [out] pJson 用来保存输出的json字符串
        **            [in] nJson pJson的大小，一般长度为Len()的2到4倍；
        **            [in] isFmt 是否格式化输出
        **            [in] prtBin 是否打印二进制流元素，将统一显示为[mem_,len_]，如[mem_0x12345678,len_1024]
        ** Return   : 是否成功
        ** Author   : faker@2020-11-22
        *************************************************************************/
        bool Print(char* pJson, int nJson, bool isFmt, bool prtBin = false);
    };
}

#endif
