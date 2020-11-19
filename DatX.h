
/*************************************************************************
** Copyright(c) 2016-2025  faker
** All rights reserved.
** Name		: DatX.h/cpp
** Desc		: һ�ֽ��յ����ݽṹ���ʺ����紫�䣬���ô洢�ȣ�һ���̶��������json����д������json��0�����
**				�ڴ�ṹ���£� {DatX::__len__,[szKey01,nBin01,pBin01],[szKey02,nBin02,pBin02],...}
**              ��������json��ʽ���磺
**                  1.��������{"aaa":521,"bbb":13.14,"ccc":"hello"}
**                  2.�������飺["elem0","elem1","elem2"]
**                  3.��������飺{"aaa":521,"bbb":13.14,"ccc":"hello","ddd":["elem0","elem1","elem2"]}
**                  4.���������[{"aaa0":521,"bbb0":13.14,"ccc0":"hello"},{"aaa1":521,"bbb1":13.14}]
**              ����֧�ֵĸ�ʽ���磺
**                  1.��������1[0~2Ϊ��ֵ��Ԫ�أ�3Ϊ����Ԫ��]��{"aaa":521,"bbb":13.14,"ccc":"hello",["elem0","elem1","elem2"]}
**                  2.��������2��[0~2Ϊ��ֵ��Ԫ�أ�3Ϊ����Ԫ�أ�4~5Ϊ������]��{"aaa":521,"bbb":13.14,"ccc":"hello",["elem0","elem1","elem2"],"binary0":0101101,"binary1":1110101}
**            �����÷�������
**              1.Put����ǰDatX�������һ��KV����K�Ѵ���ʱ�����ֵ�滻�����ص�ǰDatX�����ã�
**              2.Get(s)���ӵ�ǰDatX�����ȡK==s��KV�ԣ���֧���м�����Ԫ�أ���XTY��װ�����أ�
**              3.Get(i)���ӵ�ǰDatX�����ȡ��i��KV�ԣ�֧����������Ԫ�أ���XTY��װ�����أ�
**              4.Put����ǰDatX���һ���м���Ԫ�أ�һ��������Ӽ�ֵ��Ԫ�أ����ص�ǰDatX�����ã�
**              5.Add����ǰDatX���һ���޼���Ԫ�أ�һ�������������Ԫ�أ����ص�ǰDatX�����ã�
**              6.[s]������ǰDatX����Ϊs�ļ�ֵת��Ϊһ��DatX��
**              7.[i]������ǰDatX��i��Ԫ�ؼ�ֵת��Ϊһ��DatX��
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
    // ���ݽṹ��
    struct DatX
    {
        static const unsigned int COB = 1024; // �ڴ�������Сֵ
        DatX* __parent__; // ���ڵ�
		unsigned int __mem__; // ������ʼ��ַ
		unsigned int __len__; // �����ܴ�С���ᱣ��������ͷ��
		unsigned int __cap__; // Ԥ���ڴ�����
		unsigned int __cnt__; // ��ǰ�ڵ㹲�ж��ٸ�Ԫ��
		unsigned int __err__; // �������һ�δ����룬0��ʾ�޴��󡣡���ʵ�֡�
        
        // ΪK����洢������ͨ��KV�ԡ�������ʽ����ΪV����洢������Vֵ��������ʽ��
        // __type__=='?': {__len__,}
        // __type__=='K': {__len__,[K,n,V],[K,n,V],...}
        // __type__=='V': {__len__,[0,n,V],[0,n,V],...}
        char __type__; // faker@2020-11-18 20:53:36 TODO __type__��ʱ������[__Put]�����򽫲�֧�ֻ������飬����Լ�ֵ�ԡ�KV��
		
		struct XTY
		{
			bool IsNull() { return  (k == nullptr || v == 0 || n < 1); };

            unsigned int _n_; // ��ǰ��ֵ�Ե��ܴ�С
            union
            {
                unsigned int _p_; // ��ǰ��ֵ�Ե���ʼ��ַ
			    char* k; // ָ�����������DatX::__type__=='V'��kʼ��ָ��'\0'
            };
			unsigned int n; // ��ֵ�ֽ���
            unsigned int v; // ��ֵ��ַ
            int i; // ��������

			int I; // ����Ϊint��
			float F; // ����Ϊfloat��
			char *S; // ����Ϊchar*��
			friend struct DatX;

            XTY(unsigned int _k_mem = 0)
            {
                Read(_k_mem, 0);
            };

		private:
            bool Read(unsigned int _k_mem, int iKey)
            {
                memset(this, 0, sizeof(XTY));
                i = -1;
                if (_k_mem != 0)
                {
                    i = iKey;
                    k = (char*)_k_mem;
                    int klen = strlen(k) + 1;
                    n = *(unsigned int*)(_k_mem + klen);
                    v = _k_mem + klen + sizeof(XTY::n);
                    _n_ = klen + sizeof(XTY::n) + n;

                    char szTemp[32];
                    memcpy(szTemp, (char*)v, n > sizeof(szTemp) ? sizeof(szTemp) :n);
                    szTemp[31] = 0;
                    I = atoi(szTemp);
                    F = (float)atof(szTemp);
                    S = (char*)v;

                    return true;
                }
                return false;
            };
		};

		DatX();
		DatX(void* pMem, int nLen);
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
		DatX& Put(const char* szKey, int iVal);
		DatX& Put(const char* szKey, double dVal);
		DatX& Put(const char* szKey, char* sVal);
		DatX& Put(const char* szKey, const char* szFmt, ...);
		DatX& Put(const char* szKey, unsigned int nBin, void* pBin);
        DatX& Put(const char* szKey, DatX& dx);

		// �����������Ԫ�أ�����Add�����ص�ǰDatX����
        DatX& Add(int iVal);
        DatX& Add(double dVal);
        DatX& Add(char* sVal);
        DatX& Add(const char* szFmt, ...);
        DatX& Add(unsigned int nBin, void* pBin);
        DatX& Add(DatX& dx);

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

        /*************************************************************************
        ** Desc     : ��szKey��Ӧ��ֵǿתΪһ���������͡�DatX����ת��ʧ����Ὣԭֵ�ÿ�
        ** Param    : [in] szKey
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        DatX operator[](const char* szKey);

        /*************************************************************************
        ** Desc     : ��iKey��Ӧ��ֵǿתΪһ���������͡�DatX����ת��ʧ����Ὣԭֵ�ÿգ���iKey>=��ǰDatX::Cnt()������DatX::Cnt()������
        ** Param    : [in] iKey
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        DatX operator[](int iKey);
#if 0 // faker@2020-11-19 20:26:15 ��ʱ��������Ϊ�Ѳ������������ͨ����
        /*************************************************************************
        ** Desc     : ��szKey��Ӧ��ֵ�ӻ�Ϊһ���������͡�DatX����ת��ʧ����Ὣԭֵ�ÿ�
        ** Param    : [in] szKey 
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        DatX operator()(const char* szKey);

        /*************************************************************************
        ** Desc     : ��iKey��Ӧ��ֵ�ӻ�Ϊһ���������͡�DatX����ת��ʧ����Ὣԭֵ�ÿ�
        ** Param    : [in] iKey
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        DatX operator()(int iKey);
#endif
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

		// ��ʵ�֡�����
		bool FromJson(char* pJson) { return false; };
		int ToJson(DatX* pDatX) { return 0; };
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
		**              ��mem==0��xlen>lenʱ���ᴥ����this��Ա�ĳ�ʼ��
		** Param    : [in] p_mem ����ӻ�ɾ���ļ�ֵ�Ե�ַ�ĵ�ַ��mem==0��ζ�����Ǹ��ڵ���׸�����
		**            [in] len ԭ��mem�������ݴ�С��len==0��ζ������һ���µ�Ԫ��
		**            [in] xlen �����ݴ�С
		** Return   :
		** Author   : faker@2020-11-9 20:46:02
		*************************************************************************/
		void move_memory(unsigned int* p_mem, unsigned int len, unsigned int xlen);
		DatX& _Put(const char* szKey, const char* szFmt, va_list body);
		DatX& __Put(const char* szKey, unsigned int nBin, void* pBin);
	};
	
	// ��֧��ֻ��������������д�����ʱ���ٴ�DatX�л�ȡ����ֵ
	struct FastDatX : DatX
	{
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
		DatX& Put(const char* szKey, const char* szFmt, ...){return *this;}
		DatX& Put(const char* szKey, unsigned int nBin, void* pBin){return *this;}
		DatX& Put(const char* szKey, DatX& dx){return *this;}
		DatX& Add(int iVal) { return *this; }
		DatX& Add(float fVal) { return *this; }
		DatX& Add(char* sVal) { return *this; }
		DatX& Add(const char* szFmt, ...) { return *this; }
		DatX& Add(unsigned int nBin, void* pBin) { return *this; }
		DatX& Add(DatX& dx) { return *this; }
		bool Del(const char *szKey) { return false; }
		int LastError(){return __err__;}
		void Build(void* pMem, int nLen){}

		bool __is_valid__;
	};
}

#endif

