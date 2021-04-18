
/*************************************************************************
** Copyright(c) 2016-2025  faker
** All rights reserved.
** Name		: MemJson.h/cpp
** Desc		: 一种紧凑的数据结构，适合网络传输，配置存储等，一定程度上能替代json，读写均优于json，0秒解析
**				根节点和子节点的内存结构均如下： {MemJson::__len__,[szKey01,xType01,nBin01,pBin01],[szKey02,xType02,nBin02,pBin02],...}
**              万物皆KV对，普通元素[k,t,n,v]，无键名元素['\0',t,n,v]，有键名null元素[k,t,n,_]，无键名null元素['\0',t,n,_]；
**              兼容所有json格式，如：
**                  1.单级对象：{"aaa":521,"bbb":13.14,"ccc":"hello"}
**                  2.单级数组：["elem0","elem1","elem2"]
**                  3.对象包数组：{"aaa":521,"bbb":13.14,"ccc":"hello","ddd":["elem0","elem1","elem2"]}
**                  4.数组包对象：[{"aaa0":521,"bbb0":13.14,"ccc0":"hello"},{"aaa1":521,"bbb1":13.14}]
**              另外支持的格式，如：
**                  1.混杂数组1[0~2为键值对元素，3为数组元素]：{"aaa":521,"bbb":13.14,"hello",["elem0","elem1","elem2"]}
**                  2.混杂数组2[0~2为键值对元素，3为数组元素，4~5为二进制]：{"aaa":521,"bbb":13.14,"hello",["elem0","elem1","elem2"],"binary0":stream,"binary1":stream}
**            函数用法概览：
**              1.Put：向当前MemJson对象添加一个KV，当K已存在时会进行值替换，返回当前MemJson的引用；
**              2.Get(s)：从当前MemJson对象获取K==s的KV对，仅支持有键名的元素；用XTY包装并返回；
**              3.Get(i)：从当前MemJson对象获取第i个KV对，支持所有类型元素；用XTY包装并返回；
**              4.Put：向当前MemJson添加一个有键名元素，一般用于添加键值对元素，返回当前MemJson的引用；
**              5.Add：向当前MemJson添加一个无键名元素，一般用于添加数组元素，返回当前MemJson的引用；
**              6.Del(s)：从当前MemJson删除一个有键名的元素，一般用于删除键值对元素；
**              7.Del(i)：向当前MemJson添加一个无键名的元素，一般用于删除数组元素；
**              8.[s]：获取当前MemJson中键名为s的子节点；
**              9.[i]：获取当前MemJson的第i个子节点；
**              10.(s)：获取当前MemJson中键名为s的子节点，若不存在或非节点则进行创建或置空；
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

#define __SUPPORT_STD_JSON__ 1 // 是否支持标准json

namespace x2lib
{
	// 此类不应该存在virtual成员
    class MemJson
    {
    public:
        struct XTY
        {
			// 有键无值
            bool IsNull() { return  (k != nullptr && v != 0 && n == 0 && t == 'N'); };

            bool IsValid() { return  (k != nullptr && v != 0); };

            uint32_t _n_; // 当前键值对的总大小
            union
            {
                uint32_t _p_; // 当前键值对的起始地址
                char* k; // 指向键名，允许为'\0'，此时为无键名元素
            };
            char t; // 值类型：['N',null],['I',int],['F',double],['B',bool],['S',string],['H',binary],['M',MemJson]
            uint32_t n; // 键值字节数，若t=='N'则n==0
            uint32_t v; // 键值地址
            int i; // 键的索引

            int I; // 导出为int型
            double F; // 导出为float型
            char *S; // 导出为char*型
            friend class MemJson;

            int stoI() { return atoi(S); }
            double stoF() { return atof(S); }

            XTY(uint32_t _k_mem = 0)
            {
                Read(_k_mem, 0);
            };

        private:
            // iKey只用来设置
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
        MemJson(); // 初始化为根节点
        //MemJson(const MemJson& mj, bool mjAsRoot = false); // 初始化为根节点或子节点
        MemJson& operator=(const MemJson& dx) = delete; // 初始化为根节点
        MemJson(const char* szKey, const char* szFmt, ...); // 初始化为根节点
        MemJson(const char* szKey, int nBin, void* pBin); // 初始化为根节点
        ~MemJson(); // 构造函数使用了memset，禁止设为virtual

		MemJson(uint32_t mem); // 仅提供给Clone调用；根节点==对象性对象

		/*************************************************************************
		** Desc     : 创建一个节点（可以是父节点）引用
		** Param    : [in]
		** Return   : 返回一个对象型MemJson对象
		** Author   : faker@2021-4-18 13:22:56
		*************************************************************************/
		MemJson(uint64_t parent_mem); // 仅提供给Quote,(),[],=调用；子节点==引用型对象
		//MemJson& operator=(uint32_t mem);

		/*************************************************************************
		** Desc     : 将当前节点（可以是子节点或根节点）克隆为一个根节点
		** Param    : [in]
		** Return   : 返回一个对象型MemJson对象
		** Author   : faker@2021-4-18 13:22:56
		*************************************************************************/
		MemJson Clone();

		/*************************************************************************
		** Desc     : 返回当前节点（可以是子节点或根节点）的引用，具有时效性，若原对象发生改变则失效
		** Param    : [in]
		** Return   : 返回一个引用型MemJson对象
		** Author   : faker@2021-4-18 13:20:27
		*************************************************************************/
		MemJson Quote();

		bool IsRoot() const;

		bool IsQuote() const;

		const MemJson* GetRoot() const;
                 // 获取数据内存起始地址
        uint32_t Mem() const;
        // 获取数据总字节数
        uint32_t Len() const;
        // 获取元素个数
        uint32_t Cnt() const;

        // 用于添加键值对，所有Put都返回当前MemJson对象
        MemJson& Put(const char* szKey, bool bVal);
        MemJson& Put(const char* szKey, int iVal);
        MemJson& Put(const char* szKey, double dVal);
        MemJson& Put(const char* szKey, const char* szFmt, ...); // 当szFmt=nullptr时添加一个名为szKey的null元素
        MemJson& Put(const char* szKey, uint32_t nBin, void* pBin);
        MemJson& Put(const char* szKey, MemJson& dx);

        // 用于添加数组元素，所有Add都返回当前MemJson对象
        MemJson& Add(bool bVal);
        MemJson& Add(int iVal);
        MemJson& Add(double dVal);
        MemJson& Add(const char* szFmt, ...); // 当szFmt=nullptr时添加一个null元素
        MemJson& Add(uint32_t nBin, void* pBin);
        MemJson& Add(MemJson& dx);

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

        // 清空
        void Clear();

        /*************************************************************************
        ** Desc     : 通过首个元素是否有键名判定是否为数组，例如首个元素无键名则认为是数组
        ** Param    : [in]
        ** Return   :
        ** Author   : faker@2020-11-22
        *************************************************************************/
        bool IsArr();

		/*************************************************************************
		** Desc     : 判定当前对象是否为标准json
		** Param    : [in]
		** Return   :
		** Author   : faker@2021-4-11
		*************************************************************************/
		bool IsJSON();

        /*************************************************************************
        ** Desc     : 获取名为szKey的子节点，szKey对应的子节点必须存在
        ** Param    : [in] szKey
        ** Return   : 返回子节点引用型对象，若子节点不存在，则引发程序异常
        ** Author   : faker@2020-11-12
        *************************************************************************/
        MemJson operator[](const char* szKey);

        /*************************************************************************
        ** Desc     : 获取第iKey个子节点，szKey对应的子节点必须存在
        ** Param    : [in] iKey
        ** Return   : 返回子节点引用型对象，若子节点不存在，则引发程序异常
        ** Author   : faker@2020-11-12
        *************************************************************************/
        MemJson operator[](int iKey);

        /*************************************************************************
        ** Desc     : 获取名为szKey的子节点，当子节点不存在、null或为值类型时，将会清空或创建一个空节点
        ** Param    : [in] szKey
        ** Return   : 返回得到的子节点
        ** Author   : faker@2020-11-12
        *************************************************************************/
        MemJson operator()(const char* szKey);

        /*************************************************************************
        ** Desc     : 获取第iKey个元素，支持获取数组元素
        ** Param    : [in] iKey
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        XTY Get(uint32_t iKey) const;

        /*************************************************************************
        ** Desc     : 获取名为szKey的元素，不支持获取数组元素
        ** Param    : [in] szKey
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        XTY Get(const char* szKey) const;

        /*************************************************************************
        ** Desc     : 获取最后一次错误代码，为0表示正常
        ** Param    : [in]
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        int LastError();

        bool IsValid();

#if __SUPPORT_STD_JSON__
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
        ** Return   : 是否成功，当使用返回值为char*的重载版时，需要注意的是不要直接保存指针使用，而需要使用std::string或直接作为函数实参才可使用
        ** Author   : faker@2020-11-22
        *************************************************************************/
        bool Print(char* pJson, int nJson, bool isFmt, bool prtBin = false);
        char* Print(bool isFmt, int* pnJson = nullptr, bool prtBin = false);
#endif
    protected:
		void init();
		//bool build(void* pMem, int nLen); // 仅供构造函数调用（仅供根节点使用）

        /*************************************************************************
        ** Desc     : 只能尝试判断是否有效，且操作不当有可能会造成崩溃，最好使用win最好使用IsBadReadPtr判断
        ** Param    : [in] pMem
        **			: [in] nLen
        **			: [out] pnCnt 获取解析后的元素个数
        ** Return   :
        ** Author   : faker@2020-11-12 08:53:51
        *************************************************************************/
        static bool Check(void* pMem, uint32_t* pnCnt = nullptr);

        /*************************************************************************
        ** Desc     : 移动数据，为将要添加或删除的键值对预留空间，所有的内存分配/释放都必须在此函数内进行；
        **              当mem==0且xlen>len时，会触发对this成员的初始化；
        **              函数内部在必要时会更新当前节点以及所有父辈节点的__mem__, __len__, __cap__,函数外部应该做好__parent__, __cnt__的更新；
        ** Param    : [in] p_mem 欲添加或删除的键值对地址的地址，mem==0意味着这是根节点的首个数据
        **            [in] len 原本mem处的数据大小，len==0意味着这是一个新的元素
        **            [in] xlen 新数据大小
        ** Return   :
        ** Author   : faker@2020-11-9 20:46:02
        *************************************************************************/
        void move_memory(uint32_t* p_mem, uint32_t len, uint32_t xlen);
        MemJson& _Put(const char* szKey, const char* szFmt, va_list body);
		uint32_t __Put(const char* szKey, char t, uint32_t nBin, void* pBin);

        static const uint32_t COB = 1024; // 内存增长最小值
        static const uint32_t GC_SIZE = COB * 4; // 内存回收最小值
		static const uint32_t MIN_LEN = sizeof(uint32_t); // 原__len__
        MemJson* __parent__; // 父节点；根节点的__parent__=nullptr，子节点的__parent=parent
        uint32_t __mem__; // 当前节点的内存地址
        //uint32_t __len__; // 数据总大小，会保存在数据头部
        uint32_t __cap__; // 预留内存容量，根节点一般__cap__>__len__，子节点必须__cap__==__len__
        uint32_t __cnt__; // 当前节点共有多少个元素
        uint32_t __err__; // 保持最后一次错误码，0表示无错误。【待实现】

        char* __buff__; // 提供给Print函数使用
    };

    // 仅支持只读，用于在无需写入操作时快速从MemJson中获取所需值
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

    private: // 隐藏父类的以下成员
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

