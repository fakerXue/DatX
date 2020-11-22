#include "MemJson.h"
#include "cJSON/cJSON.h"

using namespace x2lib;

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
        {
            MemJson dx;
            parse_cJSON(&dx, it->child);
            pMemJson->Put(it->string, dx);
        }break;
        case cJSON_Object:
        {
            MemJson dx;
            parse_cJSON(&dx, it->child);
            pMemJson->Add(dx);
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
        char szMemKey[64] = {0};
        if (prtBin && (!x.k || x.k[0] == 0))
        {
            sprintf(szMemKey, "mem_0x%08X", x.v);
            x.k = szMemKey;
        }
        switch (x.t)
        {
        case 'N': cJSON_AddNullToObject(item, x.k); break;
        case 'K': 
        { // 使用索引值访问pMemJson，不要使用键名，因为子元素可能无键名
            print_cJSON((MemJson*)&((*pMemJson)(x.i)), cJSON_AddObjectToObject(item, x.k), prtBin);
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
                char szVal[64] = {0};
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
    if(root)
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
    int nBytesFilled = 0;
    cJSON* root = nullptr;
    if (__type__ == 'K')
    {
        root = cJSON_CreateObject();
    }
    else if(__type__ == 'V')
    {
        root = cJSON_CreateArray();
    }
    print_cJSON(this, root, prtBin);

    bool isSucc = cJSON_PrintPreallocated(root, pJson, nJson, isFmt);
    cJSON_Delete(root);
    return isSucc;
}
