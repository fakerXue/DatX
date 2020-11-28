/*************************************************************************
** Copyright(c) 2020-2025  faker
** All rights reserved.
** Name		: MemJson.h/cpp
** Desc		: ����DatX��cJSON��json�࣬����ʵ�ֱ�׼Json�ַ�����DatX�Ļ�ת��ͬʱҲ���Խ�DatX�ı���Ϊ��αJson���ַ���
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
        ** Desc     : ��json�ַ���תΪDatX
        ** Param    : [in] pJson json�ַ���
        ** Return   : �Ƿ�ɹ���ʧ������pJson�Ƿ�Ϊ��׼��ʽ
        ** Author   : faker@2020-11-22
        *************************************************************************/
        bool Parse(const char* pJson);

        /*************************************************************************
        ** Desc     : ��DatX��ӡΪjson�ַ���
        ** Param    : [out] pJson �������������json�ַ���
        **            [in] nJson pJson�Ĵ�С��һ�㳤��ΪLen()��2��4����
        **            [in] isFmt �Ƿ��ʽ�����
        **            [in] prtBin �Ƿ��ӡ��������Ԫ�أ���ͳһ��ʾΪ[mem_,len_]����[mem_0x12345678,len_1024]
        ** Return   : �Ƿ�ɹ�
        ** Author   : faker@2020-11-22
        *************************************************************************/
        bool Print(char* pJson, int nJson, bool isFmt, bool prtBin = false);
    };
}

#endif
