
/*************************************************************************
** Copyright(c) 2016-2025  faker
** All rights reserved.
** Name		: DatX.h/cpp
** Desc		: һ�ֽ��յ����ݽṹ���ʺ����紫�䣬���ô洢�ȣ�һ���̶��������json����д������json��0�����
**				�ڴ�ṹ���£� {DatX::__len__,DatX::__type__,[szKey01,xType01,nBin01,pBin01],[szKey02,xType02,nBin02,pBin02],...}
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
**              1.Put����ǰDatX�������һ��KV����K�Ѵ���ʱ�����ֵ�滻�����ص�ǰDatX�����ã�
**              2.Get(s)���ӵ�ǰDatX�����ȡK==s��KV�ԣ���֧���м�����Ԫ�أ���XTY��װ�����أ�
**              3.Get(i)���ӵ�ǰDatX�����ȡ��i��KV�ԣ�֧����������Ԫ�أ���XTY��װ�����أ�
**              4.Put����ǰDatX���һ���м���Ԫ�أ�һ��������Ӽ�ֵ��Ԫ�أ����ص�ǰDatX�����ã�
**              5.Add����ǰDatX���һ���޼���Ԫ�أ�һ�������������Ԫ�أ����ص�ǰDatX�����ã�
**              6.Del(s)���ӵ�ǰDatXɾ��һ���м�����Ԫ�أ�һ������ɾ����ֵ��Ԫ�أ�
**              7.Del(i)����ǰDatX���һ���޼�����Ԫ�أ�һ������ɾ������Ԫ�أ�
**              8.[s]��������Ϊs��Ԫ��ת��Ϊ�������͡��ӽڵ㣻
**              9.[i]������i��Ԫ��ת��Ϊ�������͡��ӽڵ㣻
**              10.(s)��������Ϊs��Ԫ��ת��Ϊ�������͡��ӽڵ㣻
**              11.(i)������i��Ԫ��ת��Ϊ�������͡��ӽڵ㣻
** Author	: faker@2017-3-19 11:11:57
*************************************************************************/

#ifndef _FDA2F23D_BB85_4DB9_9A6C_6928F804A54B
#define _FDA2F23D_BB85_4DB9_9A6C_6928F804A54B

#define _CRT_SECURE_NO_WARNINGS

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <vadefs.h>
#include <stdarg.h>
#include <string>

namespace x2lib
{
    struct DatX
    {
        struct XTY
        {
            bool IsNull() { return  (k != nullptr && v != 0 && n == 0 && t == 'N'); };
            bool IsValid() { return  (k != nullptr && v != 0); };

            unsigned int _n_; // ��ǰ��ֵ�Ե��ܴ�С
            union
            {
                unsigned int _p_; // ��ǰ��ֵ�Ե���ʼ��ַ
                char* k; // ָ�����������DatX::__type__=='V'��kʼ��ָ��'\0'
            };
            char t; // ֵ���ͣ�['N',null],['I',int],['F',double],['B',bool],['S',string],['H',binary],['K',DatX::__type__=='K'],['V',DatX::__type__=='V']
            unsigned int n; // ��ֵ�ֽ���
            unsigned int v; // ��ֵ��ַ
            int i; // ��������

            int I; // ����Ϊint��
            double F; // ����Ϊfloat��
            char *S; // ����Ϊchar*��
            friend struct DatX;

            XTY(unsigned int _k_mem = 0)
            {
                Read(_k_mem, 0);
            };

        private:
            // iKeyֻ��������
            bool Read(unsigned int _k_mem, int iKey)
            {
                memset(this, 0, sizeof(XTY));
                i = -1;
                if (_k_mem != 0)
                {
                    i = iKey;
                    k = (char*)_k_mem;
                    int klen = strlen(k) + 1;
                    t = *(char*)(_k_mem + klen);
                    n = *(unsigned int*)(_k_mem + klen + sizeof(XTY::t));
                    v = _k_mem + klen + sizeof(XTY::t) + sizeof(XTY::n);
                    _n_ = klen + sizeof(XTY::t) + sizeof(XTY::n) + n;

#if 0
                    char szTemp[32];
                    memcpy(szTemp, (char*)v, n > sizeof(szTemp) ? sizeof(szTemp) : n);
                    szTemp[31] = 0;
                    I = atoi(szTemp);
                    F = (float)atof(szTemp);
                    S = (char*)v;
#else
                    I = *(int*)v;
                    F = *(double*)v;
                    S = (char*)v;
#endif

                    return true;
                }
                return false;
            };
        };

    public:
        DatX();
        DatX(const char* szKey, const char* szFmt, ...);
        DatX(const char* szKey, int nBin, void* pBin);
        ~DatX(); // ���캯��ʹ����memset����ֹ��Ϊvirtual

        // ��ȡ�����ڴ���ʼ��ַ
        unsigned int Mem() const;
        // ��ȡ�������ֽ���
        unsigned int Len() const;
        // ��ȡԪ�ظ���
        unsigned int Cnt() const;

        // ������Ӽ�ֵ�ԣ�����Put�����ص�ǰDatX����
        DatX& Put(const char* szKey, bool bVal);
        DatX& Put(const char* szKey, int iVal);
        DatX& Put(const char* szKey, double dVal);
        //DatX& Put(const char* szKey, char* sVal);
        DatX& Put(const char* szKey, const char* szFmt, ...); // ��szFmt=nullptrʱ���һ����ΪszKey��nullԪ��
        DatX& Put(const char* szKey, unsigned int nBin, void* pBin);
        //DatX& Put(const char* szKey, DatX& dx); // ֻ��1�������ڵ���ӿڣ�ͨ��()����������

        // �����������Ԫ�أ�����Add�����ص�ǰDatX����
        DatX& Add(bool bVal);
        DatX& Add(int iVal);
        DatX& Add(double dVal);
        DatX& Add(const char* szFmt, ...); // ��szFmt=nullptrʱ���һ��nullԪ��
        DatX& Add(unsigned int nBin, void* pBin);
        //DatX& Add(DatX& dx); // ֻ��1�������ڵ���ӿڣ�ͨ��()����������

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
        ** Desc     : �жϵ�ǰ�ڵ��Ƿ�Ϊ���飬���Ա�׼Json������Ч��
        ** Param    : [in] 
        ** Return   : 
        ** Author   : faker@2020-11-22
        *************************************************************************/
        bool IsArr();

        /*************************************************************************
        ** Desc     : ��ȡ��ΪszKey���ӽڵ㣬szKey��Ӧ���ӽڵ�������
        ** Param    : [in] szKey
        ** Return   : ���صõ����ӽڵ㣬���ӽڵ㲻���ڣ��򷵻�һ����ЧDatX
        ** Author   : faker@2020-11-12
        *************************************************************************/
        DatX& operator[](const char* szKey);

        /*************************************************************************
        ** Desc     : ��ȡ��iKey���ӽڵ㣬szKey��Ӧ���ӽڵ�������
        ** Param    : [in] iKey
        ** Return   : ���صõ����ӽڵ㣬���ӽڵ㲻���ڣ��򷵻�һ����ЧDatX
        ** Author   : faker@2020-11-12
        *************************************************************************/
        DatX& operator[](int iKey);

        /*************************************************************************
        ** Desc     : ��ȡ��ΪszKey���ӽڵ㣬���ӽڵ㲻���ڡ�null��Ϊֵ����ʱ��������ջ򴴽�һ���սڵ�
        ** Param    : [in] szKey 
        ** Return   : ���صõ����ӽڵ�
        ** Author   : faker@2020-11-12
        *************************************************************************/
        DatX& operator()(const char* szKey);

        /*************************************************************************
        ** Desc     : ��ȡ��iKey���ӽڵ㣬���ӽڵ㲻���ڡ�null��Ϊֵ����ʱ��������ջ򴴽�һ���սڵ�
        ** Param    : [in] iKey
        ** Return   : ���صõ����ӽڵ�
        ** Author   : faker@2020-11-12
        *************************************************************************/
        DatX& operator()(int iKey);

        /*************************************************************************
        ** Desc     : ��ȡ��iKey��Ԫ�أ�֧�ֻ�ȡ����Ԫ��
        ** Param    : [in] iKey
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        XTY Get(unsigned int iKey);

        /*************************************************************************
        ** Desc     : ��ȡ��ΪszKey��Ԫ�أ���֧�ֻ�ȡ����Ԫ��
        ** Param    : [in] szKey
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        XTY Get(const char* szKey);

        /*************************************************************************
        ** Desc     : ��ȡ���һ�δ�����룬Ϊ0��ʾ����
        ** Param    : [in]
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        int LastError();

        // ����һ��DatX���󹹽����������൱�ڿ���
        bool Build(void* pMem, int nLen);

        bool IsValid();

    protected:
        /*************************************************************************
        ** Desc     : ֻ�ܳ����ж��Ƿ���Ч���Ҳ��������п��ܻ���ɱ��������ʹ��win���ʹ��IsBadReadPtr�ж�
        ** Param    : [in] pMem
        **			: [in] nLen
        **			: [out] pnCnt ��ȡ�������Ԫ�ظ���
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        static bool IsValid(void* pMem, int nLen, unsigned int* pnCnt = nullptr);

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
        void move_memory(unsigned int* p_mem, unsigned int len, unsigned int xlen);
        DatX& _Put(const char* szKey, const char* szFmt, va_list body);
        DatX::XTY __Put(const char* szKey, char t, unsigned int nBin, void* pBin);

        static const unsigned int COB = 1024; // �ڴ�������Сֵ
        static const unsigned int GC_SIZE = COB * 4; // �ڴ������Сֵ
        DatX* __parent__; // ���ڵ�
        unsigned int __mem__; // ������ʼ��ַ
        unsigned int __len__; // �����ܴ�С���ᱣ��������ͷ��
        unsigned int __cap__; // Ԥ���ڴ�����
        unsigned int __cnt__; // ��ǰ�ڵ㹲�ж��ٸ�Ԫ��
        unsigned int __err__; // �������һ�δ����룬0��ʾ�޴��󡣡���ʵ�֡�

        // ΪK����洢������ͨ��KV�ԡ�������ʽ����ΪV����洢������Vֵ��������ʽ�����´�����DatXĬ��ΪK��
        // __type__=='K': {__len__,__type__,[K,n,V],[K,n,V],...}
        // __type__=='V': {__len__,__type__,[0,n,V],[0,n,V],...}
        // __type__ֻ�ڵ�ǰ�ڵ�����׸�Ԫ�ػ�ʹ��()/[]ǿתʱ�������ú��ж�
        char __type__; // Ϊ����ʹ��Parse/Print��Json��ת��������Ԫ�غͻ�������ʱ��Ȼ�������⡿����Ҫ����__type__����
    private: // ������������
        DatX(const DatX&) {};
        DatX& operator=(const DatX&) {};
        DatX* find_node_ptr(unsigned int _v_);
        void append_node_ptr(DatX* pdx);
        void remove_node_ptr(DatX* pdx);
        void update_nodes_ptr();
        
        // �������ж���/����ڵ�ָ��
        DatX* __nodes__; // ��ǰ�ڵ������һ���ӽڵ㣬ˮƽ�����ṹ
        //struct Node
        //{
        //    DatX* item;
        //    Node* next;
        //}*__node__;
    };

    // ��֧��ֻ��������������д�����ʱ���ٴ�DatX�л�ȡ����ֵ
    struct FastDatX : DatX
    {
    public:
        FastDatX(void* pMem, int nLen = 0)
        {
            __is_valid__ = Hook(pMem, nLen);
        }

        bool Hook(void* pMem, int nLen = 0)
        {
            if (nLen == 0) nLen = *(unsigned int*)pMem;

            __is_valid__ = DatX::IsValid(pMem, nLen, &__cnt__);
            if (!__is_valid__)
                return false;
            __mem__ = (unsigned int)pMem;
            __len__ = nLen;
            return true;
        }

        bool IsValid() { return __is_valid__; }

        ~FastDatX()
        {
            __mem__ = 0;
            __len__ = 0;
            __cnt__ = 0;
        }

    private: // ���ظ�������³�Ա
        DatX& Put(const char* szKey, int iVal) { return *this; }
        DatX& Put(const char* szKey, float fVal) { return *this; }
        DatX& Put(const char* szKey, char* sVal) { return *this; }
        DatX& Put(const char* szKey, const char* szFmt, ...) { return *this; }
        DatX& Put(const char* szKey, unsigned int nBin, void* pBin) { return *this; }
        DatX& Put(const char* szKey, DatX& dx) { return *this; }
        DatX& Add(int iVal) { return *this; }
        DatX& Add(float fVal) { return *this; }
        DatX& Add(char* sVal) { return *this; }
        DatX& Add(const char* szFmt, ...) { return *this; }
        DatX& Add(unsigned int nBin, void* pBin) { return *this; }
        DatX& Add(DatX& dx) { return *this; }
        bool Del(const char *szKey) { return false; }
        int LastError() { return __err__; }
        void Build(void* pMem, int nLen) {}

        bool __is_valid__;
    };
}

#endif

