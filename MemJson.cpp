#include "MemJson.h"

namespace x2lib
{
    MemJson::MemJson()
    {
		init();
    }

    MemJson::MemJson(uint32_t mem, uint32_t len)
    {
        if (*(uint32_t*)mem == len)
        {
            __cap__ = *(uint32_t*)mem;
            __mem__ = (uint32_t)malloc(__cap__);
            memcpy((void*)__mem__, (void*)mem, __cap__);
            __parent__ = nullptr;
            __err__ = 0;
            __buff__ = nullptr;
            MemJson::Check((void*)__mem__, &__cnt__);
        }
    }

    MemJson::MemJson(const MemJson& mj)
    {
		memcpy(this, &mj, sizeof(MemJson));
		// 对于根节点的引用，需要将__parent__设为父节点指针
		if (__parent__ == nullptr) { __parent__ = (MemJson*)&mj; }
    }

    MemJson::MemJson(const char* szKey, void* pBin, int nBin)
    {
		init();
        __Put(szKey, 'H', pBin, nBin);
    }

    MemJson::~MemJson()
    {
		if(IsRoot() && 0 != __cap__)
        { // 仅对对象型根节点进行释放（引用型节点__cap__==0）
            free((void*)__mem__);
        }
        if (__buff__ != nullptr)
        {
            free((void*)__buff__);
        }
    }

	MemJson::MemJson(uint32_t mem)
	{
		__cap__ = *(uint32_t*)mem;
		__mem__ = (uint32_t)malloc(__cap__);
		memcpy((void*)__mem__, (void*)mem, __cap__);
		__parent__ = nullptr;
		__err__ = 0;
		__buff__ = nullptr;
		MemJson::Check((void*)__mem__, &__cnt__);
	}

	MemJson::MemJson(uint64_t parent_mem)
	{
		memset(this, 0, sizeof(MemJson)); // 引用的__cap__必须设为0
		__parent__ = (MemJson*)((parent_mem & 0xFFFFFFFF00000000)>>32);
		__mem__ = (uint32_t)(parent_mem & 0x00000000FFFFFFFF);
		MemJson::Check((void*)__mem__, &__cnt__);
	}

	//MemJson MemJson::Copy()
	//MemJson MemJson::Move()

	bool MemJson::IsRoot() const
	{
		return (nullptr == __parent__);
	}

	bool MemJson::IsQuote() const
	{
		return (__parent__ && __parent__->__mem__ == __mem__);
	}

	const MemJson* MemJson::GetRoot() const
	{
		const MemJson* p_root = this;
		while (p_root->__parent__)
		{
			p_root = p_root->__parent__;
		}
		return p_root;
	}

    uint32_t MemJson::Mem() const
    {
		//if (IsRoot()) { return __mem__; }
		//else {return (GetRoot()->Mem() + __mem__);}
		return __mem__;
    }

    uint32_t MemJson::Len() const
    {
		uint32_t* p_len = (uint32_t*)Mem();
		//if (IsRoot() || __cnt__ > 0)
		//{
		//	return *p_len;
		//}
		//return 0;
		return *p_len;
    };

    uint32_t MemJson::Cnt() const
    {
        //if (__cnt__ == 0)
        //{
        //	XTY x_temp(__mem__);
        //	for (uint32_t i = 0;; ++i)
        //	{
        //		if (((uint32_t)x_temp.p - __mem__) + x_temp.n >= __len__)
        //			return i; // 已经到最后一个键
        //		int xvlen = strlen(x_temp.k) + 1 + sizeof(x_temp.n) + x_temp.n;
        //		x_temp.Read(xvlen + ((uint32_t)x_temp.k), i);
        //	}
        //}
        return __cnt__;
    }

    MemJson& MemJson::Put(const char* szKey, bool bVal)
    {
        __Put(szKey, 'B', &bVal, sizeof(bVal));
        return *this;
    }

    MemJson& MemJson::Put(const char* szKey, int iVal)
    {
        __Put(szKey, 'I', &iVal, sizeof(iVal));
        return *this;
    }

    MemJson& MemJson::Put(const char* szKey, double dVal)
    {
        __Put(szKey, 'F', &dVal, sizeof(dVal));
        return *this;
    }

    //MemJson& MemJson::Put(const char* szKey, char* sVal)
    //{
    //    if(sVal) Put(szKey, "%s", sVal);
    //    else __Put(szKey, 'N', 0, nullptr);
    //    return *this;
    //}

	MemJson& MemJson::Put(const char* szKey, char* sVal)
	{
		__Put(szKey, 'S', sVal, strlen(sVal) + 1);
		return *this;
	}

    MemJson& MemJson::Puts(const char* szKey, const char* szFmt, ...)
    {
        if (szFmt == nullptr)
        {
            __Put(szKey, 'N', nullptr, 0);
        }
        else
        {
            va_list body;
            va_start(body, szFmt);
            _Put(szKey, szFmt, body);
            va_end(body);
        }
        return *this;
    }

    MemJson& MemJson::Put(const char* szKey, void* pBin, uint32_t nBin)
    {
        __Put(szKey, 'H', pBin, nBin);
        return *this;
    }

    MemJson& MemJson::_Put(const char* szKey, const char* szFmt, va_list body)
    {
        char* pData = (char*)malloc(MemJson::COB);
        int nData = vsnprintf(pData, MemJson::COB, (char*)szFmt, body) + 1;
        if (nData > MemJson::COB)
        {
            pData = (char*)realloc(pData, nData);
            vsnprintf(pData, nData, (char*)szFmt, body);
        }
        pData[nData] = 0;
        __Put(szKey, 'S', pData, nData);
        free(pData);

        return *this;
    }

	uint32_t MemJson::__Put(const char* szKey, char t, void* pBin, uint32_t nBin)
	{
		const char *pKey = (szKey ? szKey : "\0");
		int klen = strlen(pKey) + 1;
		uint32_t newlen = klen + sizeof(XTY::t) + sizeof(XTY::n) + nBin;
		uint32_t p_mem = 0;

		XTY x = Get(pKey);
		if (x.IsValid())
		{
			p_mem = x._p_;
			move_memory(&p_mem, x._n_, newlen); // p_mem传入的是x.k的起始值，因此后面需要重新修正x.k
		}
		else
		{
			p_mem = __mem__ + (*(uint32_t*)__mem__); // ????
			++__cnt__;
			move_memory(&p_mem, 0, newlen);
		}
		memcpy((void*)p_mem, pKey, klen);
		*(char*)(p_mem + klen) = t;
		*(uint32_t*)(p_mem + klen + sizeof(XTY::t)) = nBin;
		memcpy((void*)(p_mem + klen + sizeof(XTY::t) + sizeof(XTY::n)), pBin, nBin);

        return p_mem;
    }

    MemJson& MemJson::Put(const char* szKey, MemJson& dx)
    {
        __Put(szKey, 'M', (void*)dx.Mem(), dx.Len());
        return *this;
    }

    MemJson& MemJson::Add(bool bVal)
    {
        return Put("", bVal);
    }

    MemJson& MemJson::Add(int iVal)
    {
        return Put("", iVal);
    }

    MemJson& MemJson::Add(double dVal)
    {
        return Put("", dVal);
    }

	MemJson& MemJson::Add(char* sVal)
	{
		__Put("", 'S', sVal, strlen(sVal) + 1);
		return *this;
	}

    MemJson& MemJson::Adds(const char* szFmt, ...)
    {
        if (szFmt == nullptr)
        {
            __Put("", 'N', nullptr, 0);
        }
        else
        {
            va_list body;
            va_start(body, szFmt);
            _Put("", szFmt, body);
            va_end(body);
        }
        return *this;
    }

    MemJson& MemJson::Add(void* pBin, uint32_t nBin)
    {
        return Put("", pBin, nBin);
    }

    MemJson& MemJson::Add(MemJson& dx)
    {
        return Put("", dx);
    }

    void MemJson::move_memory(uint32_t* p_mem, uint32_t len, uint32_t xlen)
    {
        int diff = xlen - len;

        MemJson* p_root = this;
        while (p_root->__parent__ && p_root->__parent__->__mem__ != p_root->__mem__) // p_root->__parent__->__mem__ == p_root->__mem__表示this是根节点的引用
        { // 处理当前链所有子节点，根节点留在while外处理
			uint32_t* p_len = (uint32_t*)p_root->__mem__;
			p_root->__cap__ = *p_len + diff;
            *p_len = p_root->__cap__; // 更新当前节点的__len__
            *(uint32_t*)(p_root->__mem__ - sizeof(XTY::n)) = p_root->__cap__; // 更新当前节点在所属XTY的n
            p_root = p_root->__parent__;
        }

		MemJson* p_rroot = nullptr;
		if (IsQuote())
		{
			p_rroot = p_root;
			p_root = p_root->__parent__;
		}

        // 搬运数据，经过上面的迭代，以下代码只针对根节点操作
        if (diff < 0)
        { // 数据收缩，前移mem+len处的数据
			uint32_t* p_len = (uint32_t*)p_root->__mem__;
            memmove((void*)((*p_mem) + xlen), (void*)((*p_mem) + len), *p_len - ((*p_mem) + len - p_root->__mem__));
			*p_len += diff;
            int _collect_bytes = p_root->__cap__ - *p_len;
            if (_collect_bytes > MemJson::GC_SIZE)
            { // 垃圾回收
                _collect_bytes -= (MemJson::GC_SIZE * (_collect_bytes / MemJson::GC_SIZE));
                p_root->__cap__ -= _collect_bytes;
				p_root->__mem__ = (uint32_t)realloc((void*)p_root->__mem__, p_root->__cap__); // 收缩数据一般不会改变内存指针
            }
        }
        else if (diff > 0)
        { // 数据扩充，后移mem处的数据
			uint32_t* p_len = (uint32_t*)p_root->__mem__;
            uint32_t len_old = *p_len;
            *p_len += diff;
            uint32_t ofs = (*p_mem) - p_root->__mem__; // 计算偏移，因为realloc后__mem__可能会变
            if (*p_len > p_root->__cap__)
            {
                p_root->__cap__ += (diff > MemJson::COB ? diff : MemJson::COB);
                p_root->__mem__ = (uint32_t)realloc((void*)p_root->__mem__, p_root->__cap__);
                p_len = (uint32_t*)p_root->__mem__;
            }
            (*p_mem) = p_root->__mem__ + ofs;  // 更新并回传给调用者
            
            memmove((void*)((*p_mem) + xlen), (void*)((*p_mem) + len), len_old - ofs - len);
        }

		if (p_rroot)
		{
			//p_rroot->__cap__ = 0; // p_root->__cap__;
			p_rroot->__mem__ = p_root->__mem__;
			p_root->__cnt__ = p_rroot->__cnt__;
		}
    }

    bool MemJson::Del(const char *szKey)
    {
        XTY x = this->Get(szKey);
        return Del(x.i);
    }

    bool MemJson::Del(int iKey)
    {
        XTY x = this->Get(iKey);
        if (x.IsValid())
        {
            --__cnt__;
            move_memory(&x._p_, x._n_, 0); // 一旦x._p_被move_memory更改，则x失效
            return true;
        }
        return false;
    }

    void MemJson::Clear()
    {
        __cnt__ = 0;
		move_memory(&__mem__, Len(), MemJson::MIN_LEN);
    }

    bool MemJson::IsArr()
    {
		return !(__cnt__ > 0 && Get((uint32_t)0).k[0]);
    }

	bool MemJson::IsJSON()
	{
		return true;
	}

    MemJson MemJson::operator[](const char* szKey)
    {
        return (*this)[Get(szKey).i]; // ???
    }

    MemJson MemJson::operator[](int iKey)
    {
        XTY x = Get(iKey);
        if (x.IsValid() && x.t == 'M')
        {
			return (uint64_t)(((uint64_t)this <<32) | x.v);
        }
        return (uint64_t)0;
    }

    MemJson MemJson::operator()(const char* szKey)
    {
		XTY x = Get(szKey);
		if (!x.IsValid() || x.t != 'M')
		{
			uint32_t p_mem = __Put(szKey, 'M', (void*)&MemJson::MIN_LEN, MemJson::MIN_LEN);
			x.Read(p_mem, __cnt__ - 1);
		}
		return (uint64_t)(((uint64_t)this << 32) | x.v);
    }

    MemJson::XTY MemJson::Get(uint32_t iKey) const
    {
        if (__mem__ != 0 && iKey >= 0 && iKey < __cnt__)
        {
            XTY x_temp(__mem__ + MemJson::MIN_LEN);
            for (uint32_t i = 0;;)
            {
                if (i == iKey) return x_temp;
                if (++i >= __cnt__) break;
                x_temp.Read((uint32_t)x_temp._p_ + x_temp._n_, i);
            }
        }

        return XTY();
    }

    MemJson::XTY MemJson::Get(const char* szKey) const
    {
        if (szKey && szKey[0] && __cnt__ != 0)
        {
            XTY x_temp(__mem__ + MemJson::MIN_LEN);
            for (uint32_t i = 0;;)
            {
                if (!x_temp.IsValid())
                    break;
                if (strcmp(x_temp.k, szKey) == 0)
                    return x_temp;
                if (++i >= __cnt__)
                    break;
                x_temp.Read((uint32_t)x_temp._p_ + x_temp._n_, i);
            }
        }

        return XTY();
    }

    int MemJson::LastError()
    {
        return __err__;
    }

    bool MemJson::IsValid()
    {
        return MemJson::Check((void*)__mem__, &__cnt__);
    }

    bool MemJson::Build(void* pMem, int nLen)
    {
        if (!IsRoot() || IsQuote() || !pMem || nLen < *(uint32_t*)pMem)
        {
            return false;
        }

        if (__mem__) { free((void*)__mem__); }
        if (__buff__) { free((void*)__buff__); }

        __cap__ = *(uint32_t*)pMem;
        __mem__ = (uint32_t)malloc(__cap__);
        memcpy((void*)__mem__, pMem, __cap__);
        __parent__ = nullptr;
        __err__ = 0;
        __buff__ = nullptr;
        MemJson::Check((void*)__mem__, &__cnt__);
        return true;
    }

	void MemJson::init()
	{
		memset(this, 0, sizeof(MemJson));
		__cap__ = MemJson::MIN_LEN;
		__mem__ = (uint32_t)malloc(__cap__);
		*(uint32_t*)__mem__ = MemJson::MIN_LEN;
	}

    bool MemJson::Check(void* pMem, uint32_t* pnCnt)
    {
        if (!pMem) return false;


		uint32_t cnt = 0;
		uint32_t len = *(uint32_t*)pMem;
		if (pMem && MemJson::MIN_LEN == len)
		{
			if (pnCnt != nullptr) *pnCnt = 0;
			return true;
		}

        XTY x_temp((uint32_t)pMem + MemJson::MIN_LEN);
        int sumlen = MemJson::MIN_LEN;
        do
        {
            if (!x_temp.IsValid())
                return false;
            sumlen += x_temp._n_;
            if (sumlen > len) return false;

            ++cnt;
            if (sumlen == len) break;

            x_temp.Read((uint32_t)pMem + sumlen, cnt);
        } while (true);
        if (pnCnt != nullptr) *pnCnt = cnt;
        return true;
    }

#if __SUPPORT_STD_JSON__
#include "cJSON/cJSON.h"
    bool parse_cJSON(MemJson* pMemJson, cJSON* item)
    {
        for (cJSON* it = item; it != NULL; it = it->next)
        {
            switch (it->type)
            {
            case cJSON_False:
            case cJSON_True:
            {
                pMemJson->Put(it->string, (bool)it->valueint);
            }break;
            case cJSON_NULL:
            {
                pMemJson->Put(it->string, nullptr);
            }break;
            case cJSON_Number:
            { // 统一使用double
                pMemJson->Put(it->string, it->valuedouble);
            }break;
            case cJSON_String:
            {
                pMemJson->Put(it->string, it->valuestring);
            }break;
            case cJSON_Array:
            case cJSON_Object:
            {
                MemJson dx;
                parse_cJSON(&dx, it->child);
                pMemJson->Put(it->string, dx);
            }break;
            case cJSON_Raw:
            case cJSON_Invalid:
            default:break;
            }
        }
        return true;
    }

    void print_cJSON(MemJson* pMemJson, cJSON* item, bool prtBin)
    {
        for (uint32_t i = 0; i < pMemJson->Cnt(); i++)
        {
            MemJson::XTY x = pMemJson->Get(i);
            char szMemKey[64] = { 0 };
            if (prtBin && (!x.k || x.k[0] == 0))
            {
                sprintf(szMemKey, "mem_0x%08X", x.v);
                x.k = szMemKey;
            }
            switch (x.t)
            {
            case 'N': cJSON_AddNullToObject(item, x.k); break;
            case 'M':
            {
				cJSON* p_item = nullptr;
				MemJson mj = (*pMemJson)[x.i];
				if (mj.IsArr()) { p_item = cJSON_AddArrayToObject(item, x.k); } 
				else { p_item = cJSON_AddObjectToObject(item, x.k); }
                print_cJSON(&mj, p_item, prtBin);
            }break;
            case 'V':
            { // 使用索引值访问pMemJson，不要使用键名，因为子元素可能无键名
                print_cJSON((MemJson*)&((*pMemJson)[x.i]), cJSON_AddArrayToObject(item, x.k), prtBin);
            }break;
            case 'I': cJSON_AddNumberToObject(item, x.k, x.I); break;
            case 'F': cJSON_AddNumberToObject(item, x.k, x.F); break;
            case 'S': cJSON_AddStringToObject(item, x.k, x.S); break;
            case 'B': cJSON_AddBoolToObject(item, x.k, x.I); break;
            case 'H':
            {
                if (prtBin)
                {
                    char szVal[64] = { 0 };
                    sprintf(szVal, "stream_%d", x.n);
                    cJSON_AddStringToObject(item, x.k, szVal);
                }
            }break;
            default:break;
            }
        }
    }

    bool MemJson::Parse(const char* pJson)
    {
        Clear();

        cJSON* root = cJSON_Parse(pJson);
        if (root)
        {
            parse_cJSON(this, root->child);
            cJSON_Delete(root);
            return true;
        }
        return false;
    }

    bool MemJson::Print(char* pJson, int nJson, bool isFmt, bool prtBin)
    {
        cJSON* root = nullptr;
        if (this->IsArr())
        {
			root = cJSON_CreateArray();
        }
        else
        {
			root = cJSON_CreateObject();
        }
        print_cJSON(this, root, prtBin);

        bool isSucc = cJSON_PrintPreallocated(root, pJson, nJson, isFmt);
        cJSON_Delete(root);
        return isSucc;
    }

    char* MemJson::Print(bool isFmt, int* pnJson, bool prtBin)
    {
        if (pnJson) *pnJson = 0;
        if (__buff__ != nullptr) { free(__buff__); }
        int buffsize = this->Len() * 2;
        __buff__ = (char*)malloc(buffsize);
        if (Print(__buff__, buffsize, isFmt, prtBin))
        {
            if (pnJson) *pnJson = strlen(__buff__);
            return __buff__;
        }

        return nullptr;
    }
#endif
}
