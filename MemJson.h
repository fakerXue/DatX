
/*************************************************************************
** Copyright(c) 2016-2025  faker
** All rights reserved.
** Name		: MemJson.h/cpp
** Desc		: һ�ֽ��յ����ݽṹ���ʺ����紫�䣬���ô洢�ȣ�һ���̶��������json����д������json��0�����
**				���ڵ���ӽڵ���ڴ�ṹ�����£� {MemJson::__len__,[szKey01,xType01,nBin01,pBin01],[szKey02,xType02,nBin02,pBin02],...}
**              �����KV�ԣ���ͨԪ��[k,t,n,v]���޼���Ԫ��['\0',t,n,v]���м���nullԪ��[k,t,n,_]���޼���nullԪ��['\0',t,n,_]��
**              ��������json��ʽ���磺
**                  1.��������{"aaa":521,"bbb":13.14,"ccc":"hello"}
**                  2.�������飺["elem0","elem1","elem2"]
**                  3.��������飺{"aaa":521,"bbb":13.14,"ccc":"hello","ddd":["elem0","elem1","elem2"]}
**                  4.���������[{"aaa0":521,"bbb0":13.14,"ccc0":"hello"},{"aaa1":521,"bbb1":13.14}]
**              ����֧�ֵĸ�ʽ���磺
**                  1.��������1[0~2Ϊ��ֵ��Ԫ�أ�3Ϊ����Ԫ��]��{"aaa":521,"bbb":13.14,"hello",["elem0","elem1","elem2"]}
**                  2.��������2[0~2Ϊ��ֵ��Ԫ�أ�3Ϊ����Ԫ�أ�4~5Ϊ������]��{"aaa":521,"bbb":13.14,"hello",["elem0","elem1","elem2"],"binary0":stream,"binary1":stream}
**            �����÷�������
**              1.Put����ǰMemJson�������һ��KV����K�Ѵ���ʱ�����ֵ�滻�����ص�ǰMemJson�����ã�
**              2.Get(s)���ӵ�ǰMemJson�����ȡK==s��KV�ԣ���֧���м�����Ԫ�أ���XTY��װ�����أ�
**              3.Get(i)���ӵ�ǰMemJson�����ȡ��i��KV�ԣ�֧����������Ԫ�أ���XTY��װ�����أ�
**              4.Put����ǰMemJson���һ���м���Ԫ�أ�һ��������Ӽ�ֵ��Ԫ�أ����ص�ǰMemJson�����ã�
**              5.Add����ǰMemJson���һ���޼���Ԫ�أ�һ�������������Ԫ�أ����ص�ǰMemJson�����ã�
**              6.Del(s)���ӵ�ǰMemJsonɾ��һ���м�����Ԫ�أ�һ������ɾ����ֵ��Ԫ�أ�
**              7.Del(i)����ǰMemJson���һ���޼�����Ԫ�أ�һ������ɾ������Ԫ�أ�
**              8.[s]����ȡ��ǰMemJson�м���Ϊs���ӽڵ㣻
**              9.[i]����ȡ��ǰMemJson�ĵ�i���ӽڵ㣻
**              10.(s)����ȡ��ǰMemJson�м���Ϊs���ӽڵ㣬�������ڻ�ǽڵ�����д������ÿգ�
** Author	: faker@2017-3-19 11:11:57
*************************************************************************/

#ifndef _FDA2F23D_BB85_4DB9_9A6C_6928F804A54B
#define _FDA2F23D_BB85_4DB9_9A6C_6928F804A54B

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdint.h>

#define __SUPPORT_STD_JSON__ 1 // �Ƿ�֧�ֱ�׼json

namespace x2lib
{
	// ���಻Ӧ�ô���virtual��Ա
    class MemJson
    {
    public:
        struct XTY
        {
			// �м���ֵ
            bool IsNull() { return  (k != nullptr && v != 0 && n == 0 && t == 'N'); };

            bool IsValid() { return  (k != nullptr && v != 0); };

            uint32_t _n_; // ��ǰ��ֵ�Ե��ܴ�С
            union
            {
                uint32_t _p_; // ��ǰ��ֵ�Ե���ʼ��ַ
                char* k; // ָ�����������Ϊ'\0'����ʱΪ�޼���Ԫ��
            };
            char t; // ֵ���ͣ�['N',null],['I',int],['F',double],['B',bool],['S',string],['H',binary],['M',MemJson]
            uint32_t n; // ��ֵ�ֽ�������t=='N'��n==0
            uint32_t v; // ��ֵ��ַ
            int i; // ��������

            int I; // ����Ϊint��
            double F; // ����Ϊfloat��
            char *S; // ����Ϊchar*��
            friend class MemJson;

            int stoI() { return atoi(S); }
            double stoF() { return atof(S); }

            XTY(uint32_t _k_mem = 0)
            {
                Read(_k_mem, 0);
            };

        private:
            // iKeyֻ��������
            bool Read(uint32_t _k_mem, int iKey)
            {
                memset(this, 0, sizeof(XTY));
                i = -1;
                if (_k_mem != 0)
                {
                    i = iKey;
                    k = (char*)_k_mem;
                    int klen = strlen(k) + 1;
                    t = *(char*)(_k_mem + klen);
                    n = *(uint32_t*)(_k_mem + klen + sizeof(XTY::t));
                    v = _k_mem + klen + sizeof(XTY::t) + sizeof(XTY::n);
                    _n_ = klen + sizeof(XTY::t) + sizeof(XTY::n) + n;

                    F = *(double*)v;
                    if (t == 'F') { I = (int)F; }
                    else { I = *(int*)v; }
                    S = (char*)v;

                    return true;
                }
                return false;
            };
        };

    public:
        MemJson(); // ��ʼ��Ϊ���ڵ�
        //MemJson(const MemJson& mj, bool mjAsRoot = false); // ��ʼ��Ϊ���ڵ���ӽڵ�
        MemJson& operator=(const MemJson& dx) = delete; // ��ʼ��Ϊ���ڵ�
        MemJson(const char* szKey, const char* szFmt, ...); // ��ʼ��Ϊ���ڵ�
        MemJson(const char* szKey, int nBin, void* pBin); // ��ʼ��Ϊ���ڵ�
        ~MemJson(); // ���캯��ʹ����memset����ֹ��Ϊvirtual

		MemJson(uint32_t mem); // ���ṩ��Clone���ã����ڵ�==�����Զ���

		/*************************************************************************
		** Desc     : ����һ���ڵ㣨�����Ǹ��ڵ㣩����
		** Param    : [in]
		** Return   : ����һ��������MemJson����
		** Author   : faker@2021-4-18 13:22:56
		*************************************************************************/
		MemJson(uint64_t parent_mem); // ���ṩ��Quote,(),[],=���ã��ӽڵ�==�����Ͷ���
		//MemJson& operator=(uint32_t mem);

		/*************************************************************************
		** Desc     : ����ǰ�ڵ㣨�������ӽڵ����ڵ㣩��¡Ϊһ�����ڵ�
		** Param    : [in]
		** Return   : ����һ��������MemJson����
		** Author   : faker@2021-4-18 13:22:56
		*************************************************************************/
		MemJson Clone();

		/*************************************************************************
		** Desc     : ���ص�ǰ�ڵ㣨�������ӽڵ����ڵ㣩�����ã�����ʱЧ�ԣ���ԭ�������ı���ʧЧ
		** Param    : [in]
		** Return   : ����һ��������MemJson����
		** Author   : faker@2021-4-18 13:20:27
		*************************************************************************/
		MemJson Quote();

		bool IsRoot() const;

		bool IsQuote() const;

		const MemJson* GetRoot() const;
                 // ��ȡ�����ڴ���ʼ��ַ
        uint32_t Mem() const;
        // ��ȡ�������ֽ���
        uint32_t Len() const;
        // ��ȡԪ�ظ���
        uint32_t Cnt() const;

        // ������Ӽ�ֵ�ԣ�����Put�����ص�ǰMemJson����
        MemJson& Put(const char* szKey, bool bVal);
        MemJson& Put(const char* szKey, int iVal);
        MemJson& Put(const char* szKey, double dVal);
        MemJson& Put(const char* szKey, const char* szFmt, ...); // ��szFmt=nullptrʱ���һ����ΪszKey��nullԪ��
        MemJson& Put(const char* szKey, uint32_t nBin, void* pBin);
        MemJson& Put(const char* szKey, MemJson& dx);

        // �����������Ԫ�أ�����Add�����ص�ǰMemJson����
        MemJson& Add(bool bVal);
        MemJson& Add(int iVal);
        MemJson& Add(double dVal);
        MemJson& Add(const char* szFmt, ...); // ��szFmt=nullptrʱ���һ��nullԪ��
        MemJson& Add(uint32_t nBin, void* pBin);
        MemJson& Add(MemJson& dx);

        /*************************************************************************
        ** Desc     : ɾ��һ��Ԫ�أ���֧������Ԫ��
        ** Param    : [in] szKey
        ** Return   : ֻ��szKey����ʱ�ŷ���true����������ô˺�����Զ����false
        ** Author   : faker@2020-11-9 20:46:02
        *************************************************************************/
        bool Del(const char *szKey);

        /*************************************************************************
        ** Desc     : ɾ��һ��Ԫ��
        ** Param    : [in] iKey
        ** Return   : ֻ��iKey����ʱ�ŷ���true
        ** Author   : faker@2020-11-9 20:46:02
        *************************************************************************/
        bool Del(int iKey);

        // ���
        void Clear();

        /*************************************************************************
        ** Desc     : ͨ���׸�Ԫ���Ƿ��м����ж��Ƿ�Ϊ���飬�����׸�Ԫ���޼�������Ϊ������
        ** Param    : [in]
        ** Return   :
        ** Author   : faker@2020-11-22
        *************************************************************************/
        bool IsArr();

		/*************************************************************************
		** Desc     : �ж���ǰ�����Ƿ�Ϊ��׼json
		** Param    : [in]
		** Return   :
		** Author   : faker@2021-4-11
		*************************************************************************/
		bool IsJSON();

        /*************************************************************************
        ** Desc     : ��ȡ��ΪszKey���ӽڵ㣬szKey��Ӧ���ӽڵ�������
        ** Param    : [in] szKey
        ** Return   : �����ӽڵ������Ͷ������ӽڵ㲻���ڣ������������쳣
        ** Author   : faker@2020-11-12
        *************************************************************************/
        MemJson operator[](const char* szKey);

        /*************************************************************************
        ** Desc     : ��ȡ��iKey���ӽڵ㣬szKey��Ӧ���ӽڵ�������
        ** Param    : [in] iKey
        ** Return   : �����ӽڵ������Ͷ������ӽڵ㲻���ڣ������������쳣
        ** Author   : faker@2020-11-12
        *************************************************************************/
        MemJson operator[](int iKey);

        /*************************************************************************
        ** Desc     : ��ȡ��ΪszKey���ӽڵ㣬���ӽڵ㲻���ڡ�null��Ϊֵ����ʱ��������ջ򴴽�һ���սڵ�
        ** Param    : [in] szKey
        ** Return   : ���صõ����ӽڵ�
        ** Author   : faker@2020-11-12
        *************************************************************************/
        MemJson operator()(const char* szKey);

        /*************************************************************************
        ** Desc     : ��ȡ��iKey��Ԫ�أ�֧�ֻ�ȡ����Ԫ��
        ** Param    : [in] iKey
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        XTY Get(uint32_t iKey) const;

        /*************************************************************************
        ** Desc     : ��ȡ��ΪszKey��Ԫ�أ���֧�ֻ�ȡ����Ԫ��
        ** Param    : [in] szKey
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        XTY Get(const char* szKey) const;

        /*************************************************************************
        ** Desc     : ��ȡ���һ�δ�����룬Ϊ0��ʾ����
        ** Param    : [in]
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        int LastError();

        bool IsValid();

#if __SUPPORT_STD_JSON__
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
        ** Return   : �Ƿ�ɹ�����ʹ�÷���ֵΪchar*�����ذ�ʱ����Ҫע����ǲ�Ҫֱ�ӱ���ָ��ʹ�ã�����Ҫʹ��std::string��ֱ����Ϊ����ʵ�βſ�ʹ��
        ** Author   : faker@2020-11-22
        *************************************************************************/
        bool Print(char* pJson, int nJson, bool isFmt, bool prtBin = false);
        char* Print(bool isFmt, int* pnJson = nullptr, bool prtBin = false);
#endif
    protected:
		void init();
		//bool build(void* pMem, int nLen); // �������캯�����ã��������ڵ�ʹ�ã�

        /*************************************************************************
        ** Desc     : ֻ�ܳ����ж��Ƿ���Ч���Ҳ��������п��ܻ���ɱ��������ʹ��win���ʹ��IsBadReadPtr�ж�
        ** Param    : [in] pMem
        **			: [in] nLen
        **			: [out] pnCnt ��ȡ�������Ԫ�ظ���
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        static bool Check(void* pMem, uint32_t* pnCnt = nullptr);

        /*************************************************************************
        ** Desc     : �ƶ����ݣ�Ϊ��Ҫ��ӻ�ɾ���ļ�ֵ��Ԥ���ռ䣬���е��ڴ����/�ͷŶ������ڴ˺����ڽ��У�
        **              ��mem==0��xlen>lenʱ���ᴥ����this��Ա�ĳ�ʼ����
        **              �����ڲ��ڱ�Ҫʱ����µ�ǰ�ڵ��Լ����и����ڵ��__mem__, __len__, __cap__,�����ⲿӦ������__parent__, __cnt__�ĸ��£�
        ** Param    : [in] p_mem ����ӻ�ɾ���ļ�ֵ�Ե�ַ�ĵ�ַ��mem==0��ζ�����Ǹ��ڵ���׸�����
        **            [in] len ԭ��mem�������ݴ�С��len==0��ζ������һ���µ�Ԫ��
        **            [in] xlen �����ݴ�С
        ** Return   :
        ** Author   : faker@2020-11-9 20:46:02
        *************************************************************************/
        void move_memory(uint32_t* p_mem, uint32_t len, uint32_t xlen);
        MemJson& _Put(const char* szKey, const char* szFmt, va_list body);
		uint32_t __Put(const char* szKey, char t, uint32_t nBin, void* pBin);

        static const uint32_t COB = 1024; // �ڴ�������Сֵ
        static const uint32_t GC_SIZE = COB * 4; // �ڴ������Сֵ
		static const uint32_t MIN_LEN = sizeof(uint32_t); // ԭ__len__
        MemJson* __parent__; // ���ڵ㣻���ڵ��__parent__=nullptr���ӽڵ��__parent=parent
        uint32_t __mem__; // ��ǰ�ڵ���ڴ��ַ
        //uint32_t __len__; // �����ܴ�С���ᱣ��������ͷ��
        uint32_t __cap__; // Ԥ���ڴ����������ڵ�һ��__cap__>__len__���ӽڵ����__cap__==__len__
        uint32_t __cnt__; // ��ǰ�ڵ㹲�ж��ٸ�Ԫ��
        uint32_t __err__; // �������һ�δ����룬0��ʾ�޴��󡣡���ʵ�֡�

        char* __buff__; // �ṩ��Print����ʹ��
    };

    // ��֧��ֻ��������������д�����ʱ���ٴ�MemJson�л�ȡ����ֵ
    struct FastDatX : MemJson
    {
    public:
        FastDatX(void* pMem, int nLen = 0)
        {
            __is_valid__ = Hook(pMem, nLen);
        }

        bool Hook(void* pMem, int nLen = 0)
        {
            if (nLen == 0) nLen = *(uint32_t*)pMem;

            __is_valid__ = MemJson::Check(pMem, &__cnt__);
            if (!__is_valid__)
                return false;
            __mem__ = (uint32_t)pMem;
            return true;
        }

        bool IsValid() { return __is_valid__; }

        ~FastDatX()
        {
            __mem__ = 0;
            __cnt__ = 0;
        }

    private: // ���ظ�������³�Ա
        MemJson& Put(const char* szKey, int iVal) { return *this; }
        MemJson& Put(const char* szKey, float fVal) { return *this; }
        MemJson& Put(const char* szKey, char* sVal) { return *this; }
        MemJson& Put(const char* szKey, const char* szFmt, ...) { return *this; }
        MemJson& Put(const char* szKey, uint32_t nBin, void* pBin) { return *this; }
        MemJson& Put(const char* szKey, MemJson& dx) { return *this; }
        MemJson& Add(int iVal) { return *this; }
        MemJson& Add(float fVal) { return *this; }
        MemJson& Add(char* sVal) { return *this; }
        MemJson& Add(const char* szFmt, ...) { return *this; }
        MemJson& Add(uint32_t nBin, void* pBin) { return *this; }
        MemJson& Add(MemJson& dx) { return *this; }
        bool Del(const char *szKey) { return false; }
        int LastError() { return __err__; }
        void Build(void* pMem, int nLen) {}

        bool __is_valid__;
    };
}

#endif

