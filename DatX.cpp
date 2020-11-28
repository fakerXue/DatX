#include "DatX.h"

using namespace x2lib;

DatX::DatX()
{
    Build(nullptr, 0);
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
    __Put(szKey,'H' ,nBin, pBin);
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

DatX& DatX::Put(const char* szKey, bool bVal)
{
    __Put(szKey, 'B', sizeof(bVal), &bVal);
    return *this;
}

DatX& DatX::Put(const char* szKey, int iVal)
{
    __Put(szKey, 'I', sizeof(iVal), &iVal);
    return *this;
}

DatX& DatX::Put(const char* szKey, double dVal)
{
    __Put(szKey, 'F', sizeof(dVal), &dVal);
    return *this;
}

//DatX& DatX::Put(const char* szKey, char* sVal)
//{
//    if(sVal) Put(szKey, "%s", sVal);
//    else __Put(szKey, 'N', 0, nullptr);
//    return *this;
//}

DatX& DatX::Put(const char* szKey, const char* szFmt, ...)
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

DatX& DatX::Put(const char* szKey, unsigned int nBin, void* pBin)
{
    __Put(szKey, 'H', nBin, pBin);
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
    __Put(szKey, 'S', nData, pData);
    free(pData);

    return *this;
}

DatX::XTY DatX::__Put(const char* szKey, char t, unsigned int nBin, void* pBin)
{
    if (__cnt__ == 0)
    { // ֻ������׸�Ԫ��ʱ�趨��ǰ�ڵ�����
        if (szKey == nullptr || szKey[0] == 0)
        { // Add
            //if (__type__ != 'V' && __type__ != '?')
            //{
            //    return *(DatX*)nullptr; // ǿ�Ƴ������
            //}
            __type__ = 'V';
        }
        else
        { // Put
            //if (__type__ != 'K' && __type__ != '?')
            //{
            //    return *(DatX*)nullptr; // ǿ�Ƴ������
            //}
            __type__ = 'K';
        }
    }

    const char *pKey = (szKey?szKey:"\0");
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
        if (__mem__ == 0/* || __len__ == 0*/) newlen += sizeof(DatX::__len__) + sizeof(DatX::__type__); // ���ڵ��״������ڴ�
        p_mem = __mem__ + __len__;
        move_memory(&p_mem, 0, newlen);
    }

    memcpy((void*)p_mem, pKey, klen);
    *(char*)(p_mem + klen) = t;
    *(unsigned int*)(p_mem + klen + sizeof(XTY::t)) = nBin;
    memcpy((void*)(p_mem + klen + sizeof(XTY::t) + sizeof(XTY::n)), pBin, nBin);
    x.Read(p_mem, __cnt__);
    ++__cnt__;

    return x;
}

DatX& DatX::Put(const char* szKey, DatX& dx)
{
    __Put(szKey, dx.__type__ , dx.Len(), (void*)dx.Mem());
    return *this;
}

DatX& DatX::Add(bool bVal)
{
    return Put("", bVal);
}

DatX& DatX::Add(int iVal)
{
    return Put("", iVal);
}

DatX& DatX::Add(double dVal)
{
    return Put("", dVal);
}

DatX& DatX::Add(const char* szFmt, ...)
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

DatX& DatX::Add(unsigned int nBin, void* pBin)
{
    return Put("", nBin, pBin);
}

DatX& DatX::Add(DatX& dx)
{
    return Put("", dx);
}

void DatX::move_memory(unsigned int* p_mem, unsigned int len, unsigned int xlen)
{
    int diff = xlen - len;

    DatX* p_root = this;
    while (p_root->__parent__)
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
        memmove((void*)(*p_mem), (void*)((*p_mem) + len), p_root->__len__ - ((*p_mem) + len - p_root->__mem__));
        p_root->__len__ += diff;
        *(unsigned int*)p_root->__mem__ = p_root->__len__;
        int _collect_bytes = p_root->__cap__ - p_root->__len__;
        if (_collect_bytes > DatX::GC_SIZE)
        { // ��������
            _collect_bytes -= (DatX::GC_SIZE * (_collect_bytes / DatX::GC_SIZE));
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
            p_root->__cap__ += (diff > DatX::COB ? diff : DatX::COB);
            p_root->__mem__ = (unsigned int)realloc((void*)p_root->__mem__, p_root->__cap__);
        }
        *(unsigned int*)p_root->__mem__ = newlen; // ֻ�ܷ���realloc��ʹ�ÿ��Դ���δ��ʼ���ĸ��ڵ�
        if ((*p_mem) == 0) { (*p_mem) = p_root->__mem__ + sizeof(DatX::__len__) + sizeof(DatX::__type__); }
        else { (*p_mem) = p_root->__mem__ + __mem__ofs_; }  // ���²��ش���������

        if (len != 0) { memmove((void*)((*p_mem) + xlen), (void*)(*p_mem), p_root->__len__ - __mem__ofs_); }
        p_root->__len__ = newlen;
    }

    update_nodes_ptr(); // ��̫���������κ�һ�μ�ֵ�Եı䶯����Ҫ�����нڵ�ָ����и���
}

bool DatX::Del(const char *szKey)
{
    XTY x = this->Get(szKey);
    return Del(x.i);
}

bool DatX::Del(int iKey)
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

void DatX::Clear()
{
    move_memory(&__mem__, __len__, sizeof(DatX::__len__) + sizeof(DatX::__type__));
    __cnt__ = 0;
}

bool DatX::IsArr()
{
    return (__type__ == 'V');
}

DatX& DatX::operator[](const char* szKey)
{
    return (*this)[Get(szKey).i];
}

DatX& DatX::operator[](int iKey)
{
    DatX* pdx = nullptr;
    XTY x = Get(iKey);
    if (x.IsValid())
    {
        pdx = find_node_ptr(x.v);
        if (pdx != nullptr) return *pdx;
#if 0
        if ((x.t == 'V' || x.IsNull()) && DatX::IsValid((void*)x.v, x.n, &dx.__cnt__))
        {
            dx.__parent__ = this;
            dx.__mem__ = (unsigned int)x.v; // ��()�Ĳ�֮ͬ��
            dx.__len__ = x.n;
            dx.__cap__ = x.n;
            dx.__type__ = 'V';
        }
#endif
#if 0
        else
        { // �ÿ�
            move_memory(&x.v, x.n, sizeof(DatX::__len__) + sizeof(DatX::__type__));
        }
#endif
    }
#if 0
    else
    {
        unsigned int len = sizeof(DatX::__len__) + sizeof(DatX::__type__);
        unsigned char val[sizeof(DatX::__len__) + sizeof(DatX::__type__)];
        *((unsigned int*)&(val[0])) = len;
        val[sizeof(DatX::__len__)] = 'K'; // �µĽڵ�Ĭ������ΪK��
        dx.__mem__ = __Put(x.k, 'V', len, val).v; // ����һ��ֵΪsizeof(DatX::__len__) + sizeof(DatX::__type__)�ļ�ֵ��
        dx.__cap__ = dx.__len__ = sizeof(DatX::__len__) + sizeof(DatX::__type__);
    }
#endif

    return *pdx;
}

DatX& DatX::operator()(const char* szKey)
{
    return (*this)(Get(szKey).i);
}

DatX& DatX::operator()(int iKey)
{
    DatX* pdx = nullptr;
    XTY x = Get(iKey);
    if (x.IsValid())
    {
        pdx = find_node_ptr(x.v);
        if (pdx != nullptr) return *pdx;
        pdx = new DatX();
        //ֻ�ж�pdx�Ƿ���ڣ�������˵��һ����һ��K��V�ڵ㣬ֱ�ӷ��ؼ��ɣ������ٽ��о�������
        pdx->__type__ = 'K';
#if 0
        if ((x.t == 'K' || x.IsNull()) && DatX::IsValid((void*)x.v, x.n, &pdx->__cnt__))
        {
            pdx->__parent__ = this;
            pdx->__mem__ = (unsigned int)x.v;
            pdx->__len__ = x.n;
            pdx->__cap__ = x.n;
        }
        else
#endif
        { // �ÿ�
            pdx->__parent__ = this;
            pdx->__mem__ = (unsigned int)x.v;
            pdx->__cap__ = pdx->__len__ = sizeof(DatX::__len__) + sizeof(DatX::__type__);
            move_memory(&pdx->__mem__, x.n, pdx->__len__);
        }
    }
    else
    {
        pdx = new DatX();
        pdx->__parent__ = this;
        pdx->__type__ = 'K';
        unsigned int len = sizeof(DatX::__len__) + sizeof(DatX::__type__);
        unsigned char val[sizeof(DatX::__len__) + sizeof(DatX::__type__)];
        *((unsigned int*)&(val[0])) = len;
        val[sizeof(DatX::__len__)] = 'K'; // �µĽڵ�Ĭ������ΪK��
        pdx->__mem__ = __Put(x.k, 'K', len, val).v; // ����һ��ֵΪsizeof(DatX::__len__) + sizeof(DatX::__type__)�ļ�ֵ��
        pdx->__cap__ = pdx->__len__ = sizeof(DatX::__len__) + sizeof(DatX::__type__);
        append_node_ptr(pdx);
    }

    return *pdx;
}

DatX::XTY DatX::Get(unsigned int iKey)
{
    if (__mem__ != 0 && iKey >= 0 && iKey < __cnt__)
    {
        XTY x_temp(__mem__ + sizeof(DatX::__len__) + sizeof(DatX::__type__));
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
        XTY x_temp(__mem__ + sizeof(DatX::__len__) + sizeof(DatX::__type__));
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

int DatX::LastError()
{
    return __err__;
}

bool DatX::Build(void* pMem, int nLen)
{
    if (!DatX::IsValid(pMem, nLen, &__cnt__))
    {
        memset(this, 0, sizeof(DatX));
        return false;
    }
    move_memory(&__mem__, __len__, nLen); // ����֤��������
    memcpy((void*)__mem__, pMem, nLen);
    __len__ = nLen;
    __type__ = *(char*)(__mem__ + sizeof(DatX::__len__) + sizeof(DatX::__type__));
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

    XTY x_temp((unsigned int)pMem + sizeof(DatX::__len__) + sizeof(DatX::__type__));
    unsigned int i = 0;
    int sumlen = sizeof(DatX::__len__) + sizeof(DatX::__type__);
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

DatX* DatX::find_node_ptr(unsigned int _v_)
{
    for (DatX* p = __nodes__; p; p = p->__nodes__)
    {
        if (p->__mem__ == _v_) return p;
    }
    return nullptr;
}

void DatX::append_node_ptr(DatX* pdx)
{
    DatX** p = &__nodes__;
    for (; *p ; p = &((*p)->__nodes__));
    *p = pdx;
}

void DatX::remove_node_ptr(DatX* pdx)
{
    DatX** p = &__nodes__;
    for (; *p && (*p)->__nodes__; p = &(*p)->__nodes__)
    {
        if (*p == pdx)
        {
            delete pdx;
            *p = (*p)->__nodes__;
            break;
        }
        else if ((*p)->__nodes__ == pdx)
        {
            delete pdx;
            (*p)->__nodes__ = (*p)->__nodes__->__nodes__;
            break;
        }
    }
}

void DatX::update_nodes_ptr()
{
    
}