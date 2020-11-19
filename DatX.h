
/*************************************************************************
** Copyright(c) 2016-2025  faker
** All rights reserved.
** Name		: DatX.h/cpp
** Desc		: 一种紧凑的数据结构，适合网络传输，配置存储等，一定程度上能替代json，读写均优于json，0秒解析
**				内存结构如下： {DatX::__len__,[szKey01,nBin01,pBin01],[szKey02,nBin02,pBin02],...}
**              兼容所有json格式，如：
**                  1.单级对象：{"aaa":521,"bbb":13.14,"ccc":"hello"}
**                  2.单级数组：["elem0","elem1","elem2"]
**                  3.对象包数组：{"aaa":521,"bbb":13.14,"ccc":"hello","ddd":["elem0","elem1","elem2"]}
**                  4.数组包对象：[{"aaa0":521,"bbb0":13.14,"ccc0":"hello"},{"aaa1":521,"bbb1":13.14}]
**              另外支持的格式，如：
**                  1.混杂数组1[0~2为键值对元素，3为数组元素]：{"aaa":521,"bbb":13.14,"ccc":"hello",["elem0","elem1","elem2"]}
**                  2.混杂数组2：[0~2为键值对元素，3为数组元素，4~5为二进制]：{"aaa":521,"bbb":13.14,"ccc":"hello",["elem0","elem1","elem2"],"binary0":0101101,"binary1":1110101}
**            函数用法概览：
**              1.Put：向当前DatX对象添加一个KV，当K已存在时会进行值替换，返回当前DatX的引用；
**              2.Get(s)：从当前DatX对象获取K==s的KV对，仅支持有键名的元素；用XTY包装并返回；
**              3.Get(i)：从当前DatX对象获取第i个KV对，支持所有类型元素；用XTY包装并返回；
**              4.Put：向当前DatX添加一个有键名元素，一般用于添加键值对元素，返回当前DatX的引用；
**              5.Add：向当前DatX添加一个无键名元素，一般用于添加数组元素，返回当前DatX的引用；
**              6.[s]：将当前DatX键名为s的键值转化为一个DatX；
**              7.[i]：将当前DatX第i个元素键值转化为一个DatX；
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
    // 数据结构：
    struct DatX
    {
        static const unsigned int COB = 1024; // 内存增长最小值
        DatX* __parent__; // 父节点
		unsigned int __mem__; // 数据起始地址
		unsigned int __len__; // 数据总大小，会保存在数据头部
		unsigned int __cap__; // 预留内存容量
		unsigned int __cnt__; // 当前节点共有多少个元素
		unsigned int __err__; // 保持最后一次错误码，0表示无错误。【待实现】
        
        // 为K代表存储的是普通的KV对【对象形式】，为V代表存储的连续V值【数组形式】
        // __type__=='?': {__len__,}
        // __type__=='K': {__len__,[K,n,V],[K,n,V],...}
        // __type__=='V': {__len__,[0,n,V],[0,n,V],...}
        char __type__; // faker@2020-11-18 20:53:36 TODO __type__暂时被废弃[__Put]，否则将不支持混杂数组，万物皆键值对【KV】
		
		struct XTY
		{
			bool IsNull() { return  (k == nullptr || v == 0 || n < 1); };

            unsigned int _n_; // 当前键值对的总大小
            union
            {
                unsigned int _p_; // 当前键值对的起始地址
			    char* k; // 指向键名，对于DatX::__type__=='V'，k始终指向'\0'
            };
			unsigned int n; // 键值字节数
            unsigned int v; // 键值地址
            int i; // 键的索引

			int I; // 导出为int型
			float F; // 导出为float型
			char *S; // 导出为char*型
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
		~DatX(); // 构造函数使用了memset，禁止设为virtual

		// 获取数据内存起始地址
		unsigned int Mem() const;
		// 获取数据总字节数
		unsigned int Len() const;
		// 获取元素个数
		unsigned int Cnt() const;

        // 用于添加键值对，所有Put都返回当前DatX对象
		DatX& Put(const char* szKey, int iVal);
		DatX& Put(const char* szKey, double dVal);
		DatX& Put(const char* szKey, char* sVal);
		DatX& Put(const char* szKey, const char* szFmt, ...);
		DatX& Put(const char* szKey, unsigned int nBin, void* pBin);
        DatX& Put(const char* szKey, DatX& dx);

		// 用于添加数组元素，所有Add都返回当前DatX对象
        DatX& Add(int iVal);
        DatX& Add(double dVal);
        DatX& Add(char* sVal);
        DatX& Add(const char* szFmt, ...);
        DatX& Add(unsigned int nBin, void* pBin);
        DatX& Add(DatX& dx);

        /*************************************************************************
        ** Desc     : 删除一个元素，不支持数组元素
        ** Param    : [in] szKey 
        ** Return   : 只有szKey存在时才返回true，对数组调用此函数永远返回false
        ** Author   : faker@2020-11-9 20:46:02
        *************************************************************************/
		bool Del(const char *szKey);

        /*************************************************************************
        ** Desc     : 删除一个元素
        ** Param    : [in] iKey
        ** Return   : 只有iKey存在时才返回true
        ** Author   : faker@2020-11-9 20:46:02
        *************************************************************************/
        bool Del(int iKey);

        /*************************************************************************
        ** Desc     : 将szKey对应的值强转为一个【数组型】DatX，若转换失败则会将原值置空
        ** Param    : [in] szKey
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        DatX operator[](const char* szKey);

        /*************************************************************************
        ** Desc     : 将iKey对应的值强转为一个【数组型】DatX，若转换失败则会将原值置空，若iKey>=当前DatX::Cnt()，则在DatX::Cnt()处创建
        ** Param    : [in] iKey
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        DatX operator[](int iKey);
#if 0 // faker@2020-11-19 20:26:15 暂时废弃，因为已不区分数组和普通对象
        /*************************************************************************
        ** Desc     : 将szKey对应的值子化为一个【对象型】DatX，若转换失败则会将原值置空
        ** Param    : [in] szKey 
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        DatX operator()(const char* szKey);

        /*************************************************************************
        ** Desc     : 将iKey对应的值子化为一个【对象型】DatX，若转换失败则会将原值置空
        ** Param    : [in] iKey
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        DatX operator()(int iKey);
#endif
        /*************************************************************************
        ** Desc     : 获取第iKey个元素，支持获取数组元素
        ** Param    : [in] iKey
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
		XTY Get(unsigned int iKey);

        /*************************************************************************
        ** Desc     : 获取名为szKey的元素，不支持获取数组元素
        ** Param    : [in] szKey
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
		XTY Get(const char* szKey);

        /*************************************************************************
        ** Desc     : 获取最后一次错误代码，为0表示正常
        ** Param    : [in] 
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
		int LastError();
		// 从另一个DatX对象构建到本对象，相当于拷贝
		bool Build(void* pMem, int nLen);
        bool IsValid();

		// 待实现。。。
		bool FromJson(char* pJson) { return false; };
		int ToJson(DatX* pDatX) { return 0; };
	protected:
        /*************************************************************************
        ** Desc     : 只能尝试判断是否有效，且操作不当有可能会造成崩溃，最好使用win最好使用IsBadReadPtr判断
        ** Param    : [in] pMem
		**			: [in] nLen
		**			: [out] pnCnt 获取解析后的元素个数
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
		static bool IsValid(void* pMem, int nLen, unsigned int* pnCnt = nullptr);

		/*************************************************************************
		** Desc     : 移动数据，为将要添加或删除的键值对预留空间，所有的内存分配/释放都必须在此函数内进行；
		**              当mem==0且xlen>len时，会触发对this成员的初始化
		** Param    : [in] p_mem 欲添加或删除的键值对地址的地址，mem==0意味着这是根节点的首个数据
		**            [in] len 原本mem处的数据大小，len==0意味着这是一个新的元素
		**            [in] xlen 新数据大小
		** Return   :
		** Author   : faker@2020-11-9 20:46:02
		*************************************************************************/
		void move_memory(unsigned int* p_mem, unsigned int len, unsigned int xlen);
		DatX& _Put(const char* szKey, const char* szFmt, va_list body);
		DatX& __Put(const char* szKey, unsigned int nBin, void* pBin);
	};
	
	// 仅支持只读，用于在无需写入操作时快速从DatX中获取所需值
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

	private: // 隐藏父类的以下成员
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

