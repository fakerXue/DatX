#include "MemJson.h"


namespace x2lib
{
    MemJson::MemJson()
    {
        Build(nullptr, 0);
    }

    MemJson::MemJson(const MemJson& dx)
    {
        memcpy(this, &dx, sizeof(MemJson));
        if (dx.__parent__ == nullptr)
        {
            this->__parent__ = this; // ��ֹ����
        }
    }

    MemJson& MemJson::operator=(const MemJson& dx)
    {
        memcpy(this, &dx, sizeof(MemJson));
        if (dx.__parent__ == nullptr)
        {
            this->__parent__ = this; // ��ֹ����
        }
        return *this;
    }

    MemJson::MemJson(unsigned int _mem_, unsigned int _len_)
    {
        Build((void*)_mem_, _len_);
    }

    MemJson::MemJson(const char* szKey, const char* szFmt, ...)
    {
        Build(nullptr, 0);
        va_list body;
        va_start(body, szFmt);
        _Put(szKey, szFmt, body);
        va_end(body);
    }

    MemJson::MemJson(const char* szKey, int nBin, void* pBin)
    {
        Build(nullptr, 0);
        __Put(szKey, 'H', nBin, pBin);
    }
    
	MemJson::MemJson(const char* pszJson)
    {
        Parse(pszJson);
    }

    MemJson::~MemJson()
    {
        if (__parent__ == nullptr)
        { // �Ը��ڵ�����ͷ�
            free((void*)__mem__);
        }
        if (__buff__ != nullptr)
        {
            free((void*)__buff__);
        }
    }

    unsigned int MemJson::Mem() const
    {
        return __mem__;
    }

    unsigned int MemJson::Len() const
    {
        return __len__;
    };

    unsigned int MemJson::Cnt() const
    {
        //if (__cnt__ == 0)
        //{
        //	XTY x_temp(__mem__);
        //	for (unsigned int i = 0;; ++i)
        //	{
        //		if (((unsigned int)x_temp.p - __mem__) + x_temp.n >= __len__)
        //			return i; // �Ѿ������һ����
        //		int xvlen = strlen(x_temp.k) + 1 + sizeof(x_temp.n) + x_temp.n;
        //		x_temp.Read(xvlen + ((unsigned int)x_temp.k), i);
        //	}
        //}
        return __cnt__;
    }

    MemJson& MemJson::Put(const char* szKey, bool bVal)
    {
        __Put(szKey, 'B', sizeof(bVal), &bVal);
        return *this;
    }

    MemJson& MemJson::Put(const char* szKey, int iVal)
    {
        __Put(szKey, 'I', sizeof(iVal), &iVal);
        return *this;
    }

    MemJson& MemJson::Put(const char* szKey, double dVal)
    {
        __Put(szKey, 'F', sizeof(dVal), &dVal);
        return *this;
    }

    //MemJson& MemJson::Put(const char* szKey, char* sVal)
    //{
    //    if(sVal) Put(szKey, "%s", sVal);
    //    else __Put(szKey, 'N', 0, nullptr);
    //    return *this;
    //}

    MemJson& MemJson::Put(const char* szKey, const char* szFmt, ...)
    {
        if (szFmt == nullptr)
        {
            __Put(szKey, 'N', 0, nullptr);
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

    MemJson& MemJson::Put(const char* szKey, unsigned int nBin, void* pBin)
    {
        __Put(szKey, 'H', nBin, pBin);
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
        __Put(szKey, 'S', nData, pData);
        free(pData);

        return *this;
    }

	unsigned int MemJson::__Put(const char* szKey, char t, unsigned int nBin, void* pBin)
	{
		if (__cnt__ == 0)
		{ // ֻ������׸�Ԫ��ʱ�趨��ǰ�ڵ�����
			if (szKey == nullptr || szKey[0] == 0)
			{ // Add
			  //if (__type__ != 'V' && __type__ != '?')
			  //{
			  //    return *(MemJson*)nullptr; // ǿ�Ƴ������
			  //}
				__type__ = 'V';
			}
			else
			{ // Put
			  //if (__type__ != 'K' && __type__ != '?')
			  //{
			  //    return *(MemJson*)nullptr; // ǿ�Ƴ������
			  //}
				__type__ = 'K';
			}
		}

		const char *pKey = (szKey ? szKey : "\0");
		int klen = strlen(pKey) + 1;
		unsigned int newlen = klen + sizeof(XTY::t) + sizeof(XTY::n) + nBin;
		unsigned int p_mem = 0;

		XTY x = this->Get(pKey);
		if (x.IsValid())
		{
			p_mem = x._p_;
			move_memory(&p_mem, x._n_, newlen);
		}
		else
		{
			if (__mem__ == 0/* || __len__ == 0*/) newlen += sizeof(MemJson::__len__) + sizeof(MemJson::__type__); // ���ڵ��״������ڴ�
			p_mem = __mem__ + __len__;
			move_memory(&p_mem, 0, newlen);
			memcpy((void*)p_mem, pKey, klen);
			++__cnt__;
		}

		*(char*)(p_mem + klen) = t;
		*(unsigned int*)(p_mem + klen + sizeof(XTY::t)) = nBin;
		memcpy((void*)(p_mem + klen + sizeof(XTY::t) + sizeof(XTY::n)), pBin, nBin);

        return p_mem;
    }

    MemJson& MemJson::Put(const char* szKey, MemJson& dx)
    {
        __Put(szKey, dx.__type__, dx.Len(), (void*)dx.Mem());
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

    MemJson& MemJson::Add(const char* szFmt, ...)
    {
        if (szFmt == nullptr)
        {
            __Put("", 'N', 0, nullptr);
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

    MemJson& MemJson::Add(unsigned int nBin, void* pBin)
    {
        return Put("", nBin, pBin);
    }

    MemJson& MemJson::Add(MemJson& dx)
    {
        return Put("", dx);
    }

    void MemJson::move_memory(unsigned int* p_mem, unsigned int len, unsigned int xlen)
    {
        int diff = xlen - len;

        MemJson* p_root = this;
        while (p_root->__parent__ && p_root->__parent__ != p_root) // ��ͨ���������������ĸ��ڵ�ʱ�������__parent__ == this
        {
            p_root->__len__ += diff;
            *(unsigned int*)p_root->__mem__ = p_root->__len__; // ���µ�ǰ�ڵ�__len__
            *(unsigned int*)(p_root->__mem__ - sizeof(XTY::n)) = p_root->__len__; // ���µ�ǰ�ڵ�������XTY��n
            p_root->__cap__ = p_root->__len__; // �Ǹ��ڵ㱣��__cap__==__len__
            p_root = p_root->__parent__;
        }

        // �������ݣ���������ĵ��������´���ֻ��Ը��ڵ����
        if (diff < 0)
        { // ����������ǰ��mem+len��������
            memmove((void*)((*p_mem) + xlen), (void*)((*p_mem) + len), p_root->__len__ - ((*p_mem) + len - p_root->__mem__));
            p_root->__len__ += diff;
            *(unsigned int*)p_root->__mem__ = p_root->__len__;
            int _collect_bytes = p_root->__cap__ - p_root->__len__;
            if (_collect_bytes > MemJson::GC_SIZE)
            { // ��������
                _collect_bytes -= (MemJson::GC_SIZE * (_collect_bytes / MemJson::GC_SIZE));
                p_root->__cap__ -= _collect_bytes;
                realloc((void*)__mem__, __cap__);
            }
        }
        else if (diff > 0)
        { // �������䣬����mem��������
            unsigned int newlen = p_root->__len__ + diff;
            unsigned int __mem__ofs_ = (*p_mem) - p_root->__mem__; // ����ƫ�ƣ���Ϊrealloc��__mem__���ܻ��
            if (newlen > p_root->__cap__)
            {
                p_root->__cap__ += (diff > MemJson::COB ? diff : MemJson::COB);
                p_root->__mem__ = (unsigned int)realloc((void*)p_root->__mem__, p_root->__cap__);
            }
            *(unsigned int*)p_root->__mem__ = newlen; // ֻ�ܷ���realloc��ʹ�ÿ��Դ���δ��ʼ���ĸ��ڵ�
            if ((*p_mem) == 0) { (*p_mem) = p_root->__mem__ + sizeof(MemJson::__len__) + sizeof(MemJson::__type__); }
            else { (*p_mem) = p_root->__mem__ + __mem__ofs_; }  // ���²��ش���������

            if (len != 0) { memmove((void*)((*p_mem) + xlen), (void*)(*p_mem), p_root->__len__ - __mem__ofs_); }
            p_root->__len__ = newlen;
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
            move_memory(&x._p_, x._n_, 0); // һ��x._p_��move_memory���ģ���xʧЧ
            --__cnt__;
            return true;
        }
        return false;
    }

    void MemJson::Clear()
    {       
		//move_memory(&__mem__, __len__, sizeof(MemJson::__len__) + sizeof(MemJson::__type__));
		move_memory(&__mem__, __len__, 0);
		//*(char*)(__mem__ + sizeof(MemJson::__len__)) = 'K';
		//*(unsigned int*)(__mem__) = sizeof(MemJson::__len__) + sizeof(MemJson::__type__);
        __cnt__ = 0;
    }

    bool MemJson::IsArr()
    {
        return (__type__ == 'V');
    }

    MemJson MemJson::operator[](const char* szKey)
    {
        return (*this)[Get(szKey).i];
    }

    MemJson MemJson::operator[](int iKey)
    {
        MemJson dx;
        XTY x = Get(iKey);
        if (x.IsValid())
        {
            if ((x.t == 'K' || x.t == 'V') && MemJson::IsValid((void*)x.v, x.n, &dx.__cnt__))
            {
                dx.__parent__ = this;
                dx.__type__ = x.t;
                dx.__mem__ = x.v;
                dx.__cap__ = dx.__len__ = x.n;
            }
#if 0
            else
            { // �ÿ�
                move_memory(&x.v, x.n, sizeof(MemJson::__len__) + sizeof(MemJson::__type__));
            }
#endif
        }
#if 0
        else
        {
            unsigned int len = sizeof(MemJson::__len__) + sizeof(MemJson::__type__);
            unsigned char val[sizeof(MemJson::__len__) + sizeof(MemJson::__type__)];
            *((unsigned int*)&(val[0])) = len;
            val[sizeof(MemJson::__len__)] = 'K'; // �µĽڵ�Ĭ������ΪK��
            XTY x = __Put(szKey, 'V', len, val); // ����һ��ֵΪsizeof(MemJson::__len__) + sizeof(MemJson::__type__)�ļ�ֵ��
            dx.__mem__ = x.v;
            dx.__cap__ = dx.__len__ = sizeof(MemJson::__len__) + sizeof(MemJson::__type__);
        }
#endif

        return dx;
    }

    MemJson MemJson::operator()(const char* szKey)
    {
        return (*this)(Get(szKey).i);
    }

    MemJson MemJson::operator()(int iKey)
    {
        MemJson dx;
        dx.__parent__ = this;
        XTY x = Get(iKey);
        if (x.IsValid())
        {
            dx.__type__ = x.t;
            if ((x.t == 'K' || x.t == 'V' || x.IsNull()) && MemJson::IsValid((void*)x.v, x.n, &dx.__cnt__))
            {
                dx.__mem__ = (unsigned int)x.v;
                dx.__cap__ = dx.__len__ = x.n;
            }
            else
            { // �ÿ�
                dx.__cap__ = dx.__len__ = sizeof(MemJson::__len__) + sizeof(MemJson::__type__);
                move_memory(&x.v, x.n, dx.__len__);
                dx.__mem__ = (unsigned int)x.v;
            }
        }
        else
        {
            unsigned int len = sizeof(MemJson::__len__) + sizeof(MemJson::__type__);
            unsigned char val[sizeof(MemJson::__len__) + sizeof(MemJson::__type__)];
            *((unsigned int*)&(val[0])) = len;
            val[sizeof(MemJson::__len__)] = 'K'; // �µĽڵ�Ĭ������ΪK��
			unsigned int p_mem = __Put(x.k, 'K', len, val);
			x.Read(p_mem, __cnt__ - 1);
            dx.__mem__ = x.v; // ����һ��ֵΪsizeof(MemJson::__len__) + sizeof(MemJson::__type__)�ļ�ֵ��
            dx.__cap__ = dx.__len__ = len;
        }

        return dx;
    }

    MemJson::XTY MemJson::Get(unsigned int iKey)
    {
        if (__mem__ != 0 && iKey >= 0 && iKey < __cnt__)
        {
            XTY x_temp(__mem__ + sizeof(MemJson::__len__) + sizeof(MemJson::__type__));
            for (unsigned int i = 0;;)
            {
                if (i == iKey) return x_temp;
                if (++i >= __cnt__) break;
                x_temp.Read((unsigned int)x_temp._p_ + x_temp._n_, i);
            }
        }

        return XTY();
    }

    MemJson::XTY MemJson::Get(const char* szKey)
    {
        if (szKey && szKey[0] && __mem__ != 0 && __cnt__ != 0)
        {
            XTY x_temp(__mem__ + sizeof(MemJson::__len__) + sizeof(MemJson::__type__));
            for (unsigned int i = 0;;)
            {
                if (!x_temp.IsValid())
                    break;
                if (strcmp(x_temp.k, szKey) == 0)
                    return x_temp;
                if (++i >= __cnt__)
                    break;
                x_temp.Read((unsigned int)x_temp._p_ + x_temp._n_, i);
            }
        }

        return XTY();
    }

    int MemJson::LastError()
    {
        return __err__;
    }

    bool MemJson::Build(void* pMem, int nLen)
    {
        if (!MemJson::IsValid(pMem, nLen, &__cnt__))
        {
            memset(this, 0, sizeof(MemJson));
            return false;
        }
        move_memory(&__mem__, __len__, nLen); // ����֤��������
        memcpy((void*)__mem__, pMem, nLen);
        __len__ = nLen;
        __type__ = *(char*)(__mem__ + sizeof(MemJson::__len__) + sizeof(MemJson::__type__));
        return true;
    }

    bool MemJson::IsValid()
    {
        return MemJson::IsValid((void*)__mem__, __len__, &__cnt__);
    }

    bool MemJson::IsValid(void* pMem, int nLen, unsigned int* pnCnt)
    {
        if (pnCnt != nullptr) *pnCnt = 0;
        if (!pMem || nLen == 0 || (*(unsigned int*)pMem) != nLen)
            return false;

        XTY x_temp((unsigned int)pMem + sizeof(MemJson::__len__) + sizeof(MemJson::__type__));
        unsigned int i = 0;
        int sumlen = sizeof(MemJson::__len__) + sizeof(MemJson::__type__);
        do
        {
            if (!x_temp.IsValid())
                return false;
            sumlen += x_temp._n_;
            if (sumlen > nLen) return false;

            ++i;
            if (sumlen == nLen) break;

            x_temp.Read((unsigned int)pMem + sumlen, i);
        } while (true);
        if (pnCnt != nullptr) *pnCnt = i;
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
            { // ͳһʹ��double
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
        for (unsigned int i = 0; i < pMemJson->Cnt(); i++)
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
            case 'K':
            { // ʹ������ֵ����pMemJson����Ҫʹ�ü�������Ϊ��Ԫ�ؿ����޼���
                print_cJSON((MemJson*)&((*pMemJson)(x.i)), cJSON_AddObjectToObject(item, x.k), prtBin);
            }break;
            case 'V':
            { // ʹ������ֵ����pMemJson����Ҫʹ�ü�������Ϊ��Ԫ�ؿ����޼���
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
            if (root->type == cJSON_Object)
            {
                __type__ = 'K';
            }
            else if (root->type == cJSON_Array)
            {
                __type__ = 'V';
            }
            parse_cJSON(this, root->child);
            cJSON_Delete(root);
            return true;
        }
        return false;
    }

    bool MemJson::Print(char* pJson, int nJson, bool isFmt, bool prtBin)
    {
        cJSON* root = nullptr;
        if (__type__ == 'K')
        {
            root = cJSON_CreateObject();
        }
        else if (__type__ == 'V')
        {
            root = cJSON_CreateArray();
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
        int buffsize = __len__ * 2;
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
