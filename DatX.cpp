#include "DatX.h"

using namespace x2lib;

DatX::DatX()
{
    Build(nullptr, 0);
}

DatX::DatX(void* pMem, int nLen)
{
    Build(pMem, nLen);
}

DatX::DatX(const char* szKey, const char* szFmt, ...)
{
    va_list body;
    va_start(body, szFmt);
    _Put(szKey, szFmt, body);
    va_end(body);
}

DatX::DatX(const char* szKey, int nBin, void* pBin)
{
    __Put(szKey, nBin, pBin);
}

DatX::~DatX()
{
    if (__parent__ == nullptr)
    { // �Ը��ڵ�����ͷ�
        free((void*)__mem__);
    }
}

unsigned int DatX::Mem() const
{
    return __mem__;
}

unsigned int DatX::Len() const
{
    return __len__;
};

unsigned int DatX::Cnt() const
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

DatX& DatX::Put(const char* szKey, int iVal)
{
    return Put(szKey, "%d", iVal);
}

DatX& DatX::Put(const char* szKey, double dVal)
{
    return Put(szKey, "%.6f", dVal);
}

DatX& DatX::Put(const char* szKey, char* sVal)
{
    return Put(szKey, "%s", sVal);
}

DatX& DatX::Put(const char* szKey, const char* szFmt, ...)
{
    va_list body;
    va_start(body, szFmt);
    _Put(szKey, szFmt, body);
    va_end(body);
    return *this;
}

DatX& DatX::Put(const char* szKey, unsigned int nBin, void* pBin)
{
    return __Put(szKey, nBin, pBin);
}

DatX& DatX::__Put(const char* szKey, unsigned int nBin, void* pBin)
{
#if 1 // ��ʱ����__type__��ʹ֧֮�ֻ������飬�������Ƿ��б�Ҫ����
	__type__ = '?';
#else
	if (szKey == nullptr || szKey[0] == 0)
	{ // Add
		if (__type__ != 'V' && __type__ != '?')
		{
			return *(DatX*)nullptr; // ǿ�Ƴ������
		}
		__type__ = 'V';
	}
	else
	{ // Put
		if (__type__ != 'K' && __type__ != '?')
		{
			return *(DatX*)nullptr; // ǿ�Ƴ������
		}
		__type__ = 'K';
	}
#endif

    int klen = strlen(szKey) + 1;
    unsigned int newlen = klen + sizeof(XTY::n) + nBin;
    unsigned int p_mem = 0;

    XTY x = this->Get(szKey);
    if (!x.IsNull())
    {
        p_mem = x._p_;
        move_memory(&p_mem, x._n_, newlen);
        // ��p_mem��move_memory���ģ���xʧЧ�������ں���û��ʹ��x����˺��Ը���
    }
    else
    {
        if (__mem__ == 0/* || __len__ == 0*/) newlen += sizeof(DatX::__len__); // ���ڵ��״������ڴ�
        p_mem = __mem__ + __len__;
        move_memory(&p_mem, 0, newlen);
        //if (p_mem == 0)
        //{
        //    p_mem = __mem__ + sizeof(DatX::__len__);
        //    // __len__ += sizeof(DatX::__len__);
        //}
    }

    memcpy((void*)p_mem, szKey, klen);
    *(unsigned int*)(p_mem + klen) = nBin;
    memcpy((void*)(p_mem + klen + sizeof(XTY::n)), pBin, nBin);
    ++__cnt__;

    return *this;
}

DatX& DatX::_Put(const char* szKey, const char* szFmt, va_list body)
{
    char* pData = (char*)malloc(DatX::COB);
    int nData = vsnprintf(pData, DatX::COB, (char*)szFmt, body) + 1;
    if (nData > DatX::COB)
    {
        pData = (char*)realloc(pData, nData);
        vsnprintf(pData, nData, (char*)szFmt, body);
    }
    pData[nData] = 0;
    __Put(szKey, nData, pData);
    free(pData);

    return *this;
}

DatX& DatX::Put(const char* szKey, DatX& dx)
{
    return __Put(szKey, dx.Len(), (void*)dx.Mem());
}

DatX& DatX::Add(int iVal)
{
    return Add("%d", iVal);
}

DatX& DatX::Add(double dVal)
{
    return Add("%.6f", dVal);
}

DatX& DatX::Add(char* sVal)
{
    return Add("%s", sVal);
}

DatX& DatX::Add(const char* szFmt, ...)
{
    va_list body;
    va_start(body, szFmt);
    DatX& dx = _Put("", szFmt, body);
    va_end(body);
    return dx;
}

DatX& DatX::Add(unsigned int nBin, void* pBin)
{
    return __Put("", nBin, pBin);
}

DatX& DatX::Add(DatX& dx)
{
    return Add(dx.Len(), (void*)dx.Mem());
}

void DatX::move_memory(unsigned int* p_mem, unsigned int len, unsigned int xlen)
{
    int diff = xlen - len;

    DatX* p_root = this;
    while (p_root->__parent__)
    {
        p_root->__len__ += diff;
        *(unsigned int*)p_root->__mem__ = p_root->__len__; // ���µ�ǰ�ڵ�__len__
        *(unsigned int*)(p_root->__mem__-sizeof(XTY::n)) = p_root->__len__; // ���µ�ǰ�ڵ�������XTY��n
        p_root->__cap__ = p_root->__len__; // �Ǹ��ڵ㱣��__cap__==__len__
        p_root = p_root->__parent__;
    }

    // ��������
    if (diff < 0)
    { // ����������ǰ��mem+len��������
        memmove((void*)(*p_mem), (void*)((*p_mem) + len), p_root->__len__ - ((*p_mem) + len - p_root->__mem__));
        p_root->__len__ += diff;
        *(unsigned int*)p_root->__mem__ = p_root->__len__;
        int _collect_bytes = p_root->__cap__ - (p_root->__len__ + DatX::COB * 4);
        if (_collect_bytes > 0)
        { // ��������
            p_root->__cap__ -= _collect_bytes;
            realloc((void*)__mem__, __cap__);
        }
    }
    else if(diff > 0)
    { // �������䣬����mem��������
        unsigned int newlen = p_root->__len__ + diff;
        unsigned int __mem__ofs_ = (*p_mem) - p_root->__mem__; // ����ƫ�ƣ���Ϊrealloc��__mem__���ܻ��
        if (newlen > p_root->__cap__)
        {
            p_root->__cap__ += (diff > DatX::COB ? diff : DatX::COB);
            p_root->__mem__ = (unsigned int)realloc((void*)p_root->__mem__, p_root->__cap__);
        }
        *(unsigned int*)p_root->__mem__ = newlen; // ֻ�ܷ���realloc��ʹ�ÿ��Դ���δ��ʼ���ĸ��ڵ�
        if ((*p_mem) == 0) { (*p_mem) = p_root->__mem__ + sizeof(DatX::__len__); }
        else { (*p_mem) = p_root->__mem__ + __mem__ofs_;}  // ���²��ش���������

        if (len != 0) {memmove((void*)((*p_mem) + xlen), (void*)(*p_mem), p_root->__len__ - __mem__ofs_);}
        p_root->__len__ = newlen;
    }
}

bool DatX::Del(const char *szKey)
{
    XTY x = this->Get(szKey);
    return Del(x.i);
}

bool DatX::Del(int iKey)
{
    XTY x = this->Get(iKey);
    if (!x.IsNull())
    {
        move_memory(&x._p_, x._n_, 0); // һ��x._p_��move_memory���ģ���xʧЧ
        --__cnt__;
        return true;
    }
    return false;
}

DatX DatX::operator[](const char* szKey)
{
    DatX dx;
    dx.__parent__ = this;

    XTY x = Get(szKey);
    if (!x.IsNull())
    {
        if (DatX::IsValid((void*)x.v, x.n, &dx.__cnt__))
        {
            dx.__mem__ = (unsigned int)x.v; // ��()�Ĳ�֮ͬ��
            dx.__len__ = x.n;
            dx.__cap__ = x.n;
            dx.__type__ = 'V';
        }
    }
    else
    {
        unsigned int len = sizeof(DatX::__len__);
        __Put(szKey, sizeof(len), &len); // ����һ��ֵΪsizeof(DatX::__len__)�ļ�ֵ��
        dx.__mem__ = (unsigned int)Get(szKey).v;
        dx.__cap__ = dx.__len__ = sizeof(DatX::__len__);
    }

    return dx;
}

DatX DatX::operator[](int iKey)
{
    return (*this)[Get(iKey).k];
}
#if 0
DatX DatX::operator()(const char* szKey)
{ // faker@2020-11-14 10:34:13 TODO ���´���������Ҫ����
    DatX dx;
    dx.__parent__ = this;

    XTY x = Get(szKey);
    if (!x.IsNull())
    {
        dx.Build(x.v, x.n);
        return dx;
    }
    else
    {
        unsigned int len = sizeof(DatX::__len__);
        __Put(szKey, (unsigned char*)&len, sizeof(len)); // ����һ��ֵΪsizeof(DatX::__len__)�ļ�ֵ��
        dx.__mem__ = (unsigned int)Get(szKey).v;
        dx.__cap__ = dx.__len__ = sizeof(DatX::__len__);
    }

    return dx;
}

DatX DatX::operator()(int iKey)
{
    return (*this)[Get(iKey).k];
}
#endif

DatX::XTY DatX::Get(unsigned int iKey)
{
    if (__mem__ != 0 && iKey >= 0 && iKey < __cnt__)
    {
        XTY x_temp(__mem__ + sizeof(DatX::__len__));
        for (unsigned int i = 0;;)
        {
            if (i == iKey) return x_temp;
            if (++i >= __cnt__) break;
            x_temp.Read((unsigned int)x_temp._p_ + x_temp._n_, i);
        }
    }

    return XTY();
}

DatX::XTY DatX::Get(const char* szKey)
{
    if (szKey && szKey[0] && __mem__ != 0 && __cnt__ != 0)
    {
        XTY x_temp(__mem__ + sizeof(DatX::__len__));
        for (unsigned int i = 0;;)
        {
            if (x_temp.IsNull())
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

int DatX::LastError()
{
    return __err__;
}

bool DatX::Build(void* pMem, int nLen)
{
    if (!DatX::IsValid(pMem, nLen, &__cnt__))
    {
        memset(this, 0, sizeof(DatX));
        __type__ = '?';
        return false;
    }
    move_memory(&__mem__, __len__, nLen); // ����֤��������
    memcpy((void*)__mem__, pMem, nLen);
    __len__ = nLen;
    if (((char*)(__mem__ + sizeof(DatX::__len__)))[0] == 0) __type__ = 'V';
    else __type__ = 'K';
    return true;
}

bool DatX::IsValid()
{
    return DatX::IsValid((void*)__mem__, __len__, &__cnt__);
}

bool DatX::IsValid(void* pMem, int nLen, unsigned int* pnCnt)
{
    if (pnCnt != nullptr) *pnCnt = 0;
    if (!pMem || nLen == 0 || (*(unsigned int*)pMem) != nLen)
        return false;

    XTY x_temp((unsigned int)pMem + sizeof(DatX::__len__));
    unsigned int i = 0;
    int sumlen = sizeof(DatX::__len__);
    do
    {
        if (x_temp.IsNull())
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

