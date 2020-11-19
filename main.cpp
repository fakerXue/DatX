#include <stdio.h>
#include <string.h>
#include "DatX.h"

using namespace x2lib;

int main()
{
    DatX dx;

    printf("添加100个int元素，100个double元素，100个string元素：\n");
    for (int i = 0; i < 100; i++)
    {
        char szKey[128] = {0};
        sprintf(szKey, "szKey_int_%d", i);
        dx.Put(szKey,i);
    }
    for (int i = 0; i < 100; i++)
    {
        char szKey[128] = { 0 };
        sprintf(szKey, "szKey_float_%d", i);
        dx.Put(szKey, i*0.001);
    }
    for (int i = 0; i < 100; i++)
    {
        char szKey[128] = { 0 };
        sprintf(szKey, "szKey_string_%d", i);
        dx.Put(szKey, "%s_%d", "hello_string",i);
    }

    printf("添加完成，打印结果：\n");
    printf("\t第10~14个元素：%d, %d, %d, %d, %d\n", dx.Get(10).I, dx.Get(11).I, dx.Get(12).I, dx.Get(13).I, dx.Get(14).I);
    printf("\t第110~114个元素：%.6f, %.6f, %.6f, %.6f, %.6f\n", dx.Get(110).F, dx.Get(111).F, dx.Get(112).F, dx.Get(113).F, dx.Get(114).F);
    printf("\t第210~214个元素：%s, %s, %s, %s, %s\n", dx.Get(210).S, dx.Get(211).S, dx.Get(212).S, dx.Get(213).S, dx.Get(214).S);
    printf("\t根节点内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx.Mem(),dx.Len(),dx.Cnt());

    printf("\n\n");
    printf("删除第220~224个元素，"); dx.Del(224); dx.Del(223); dx.Del(222); dx.Del(221); dx.Del(220);
    printf("删除第120~124个元素，"); dx.Del(124); dx.Del(123); dx.Del(122); dx.Del(121); dx.Del(120);
    printf("删除第20~24个元素\n"); dx.Del(24); dx.Del(23); dx.Del(22); dx.Del(21); dx.Del(20);
    printf("删除完成，打印结果：\n");
    printf("\t第10~14个元素：%d, %d, %d, %d, %d\n", dx.Get(10).I, dx.Get(11).I, dx.Get(12).I, dx.Get(13).I, dx.Get(14).I);
    printf("\t第110~114个元素：%.6f, %.6f, %.6f, %.6f, %.6f\n", dx.Get(110).F, dx.Get(111).F, dx.Get(112).F, dx.Get(113).F, dx.Get(114).F);
    printf("\t第210~214个元素：%s, %s, %s, %s, %s\n", dx.Get(210).S, dx.Get(211).S, dx.Get(212).S, dx.Get(213).S, dx.Get(214).S);
    printf("\t根节点内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx.Mem(), dx.Len(), dx.Cnt());

    printf("\n\n");
    printf("添加10个数组，每个数组添加3个元素：\n");
    for (int i = 0; i < 10; i++)
    {
        char szKey[128] = { 0 };
        sprintf(szKey, "szKey_arr_%d", i);
        dx[szKey].Add(100).Add(102.345).Add("hello_array_string_%d",i);
    }
    printf("\t根节点内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx.Mem(), dx.Len(), dx.Cnt());
    printf("获取第1个数组信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx["szKey_arr_1"].Mem(), dx["szKey_arr_1"].Len(), dx["szKey_arr_1"].Cnt());
    for (int i = 0; i < dx["szKey_arr_1"].Cnt(); i++)
    {
        printf("\tszKey_arr_1_%d: %s\n", i, dx["szKey_arr_1"].Get(i).S);
    }
    printf("\n\n");

    printf("刪除第1个数组第0,1个元素：\n");
    dx["szKey_arr_1"].Del(1);
    dx["szKey_arr_1"].Del(0);
    printf("再次获取第1个数组信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx["szKey_arr_1"].Mem(), dx["szKey_arr_1"].Len(), dx["szKey_arr_1"].Cnt());
    for (int i = 0; i < dx["szKey_arr_1"].Cnt(); i++)
    {
        printf("\tszKey_arr_1_%d: %s\n",i, dx["szKey_arr_1"].Get(i).S);
    }
    printf("\t根节点内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx.Mem(), dx.Len(), dx.Cnt());
    printf("\n\n");


    printf("向根节点连续添加键值对[Put]：int,float,string,binary\n");
    dx.Put("szKey_Puts_int", 123).Put("szKey_Puts_float", 13.14).Put("szKey_Puts_string", "hello_Puts").Put("szKey_Puts_Binary1024", 1024, malloc(1024));
    printf("\t打印：%d,%f,%s,[0x%08X,%d]\n", dx.Get("szKey_Puts_int").I, dx.Get("szKey_Puts_float").F, dx.Get("szKey_Puts_string").S, dx.Get("szKey_Puts_Binary1024").v, dx.Get("szKey_Puts_Binary1024").n);
    printf("\t根节点内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx.Mem(), dx.Len(), dx.Cnt());
    printf("\n\n");

    printf("向根节点连续添加数组元素[Add]：int,float,string,binary\n");
	int index0 = dx.Cnt();
    dx.Add(123).Add(13.14).Add("hello_Adds").Add(1024, malloc(1024));
    printf("\t打印：%d,%f,%s,[0x%08X,%d]\n", dx.Get(index0).I, dx.Get(index0+1).F, dx.Get(index0 + 2).S, dx.Get(index0 + 3).v, dx.Get(index0 + 3).n);
    //faker@2020-10-19 18:24:03 是否将数组与对象分开，显得更条理，再增加一个判断当前DatX是否为数组的接口：IsArr();？？？
    printf("\t根节点内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx.Mem(), dx.Len(), dx.Cnt());
    printf("\n\n");


#if 0
    printf("连续添加4个元素：int,float,string,binary\n");
    dx("szKey_Puts").Put("szKey_Puts_int", 123).Put("szKey_Puts_float", (float)13.14).Put("szKey_Puts_string", "hello_Puts").Put("szKey_Puts_Binary1024", (unsigned char*)malloc(1024),1024);
    DatX dxPuts = dx("szKey_Puts");
    printf("\t打印：%d,%f,%s,[0x%08X,%d]\n", dxPuts.Get("szKey_Puts_int").I, dxPuts.Get("szKey_Puts_float").F, dxPuts.Get("szKey_Puts_string").S, dxPuts.Get("szKey_Puts_Binary1024").v, dxPuts.Get("szKey_Puts_Binary1024").n);
    printf("\t内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx.Mem(), dx.Len(), dx.Cnt());
    printf("\n\n");
#else
    printf("向根节点添加一个含有4个键值对元素的子节点[Put]：int,float,string,binary\n");
    dx["szKey_Puts"].Put("szKey_Puts_int", 123).Put("szKey_Puts_float", 13.14).Put("szKey_Puts_string", "hello_Puts").Put("szKey_Puts_Binary1024", 1024, malloc(1024));
    DatX dxPuts = dx["szKey_Puts"];
    printf("\t打印子节点[szKey_Puts]：%d,%f,%s,[0x%08X,%d]\n", dxPuts.Get("szKey_Puts_int").I, dxPuts.Get("szKey_Puts_float").F, dxPuts.Get("szKey_Puts_string").S, dxPuts.Get("szKey_Puts_Binary1024").v, dxPuts.Get("szKey_Puts_Binary1024").n);
    printf("\t根节点内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx.Mem(), dx.Len(), dx.Cnt());
    printf("\n\n");
#endif

    printf("向根节点添加一个含有4个数组元素的子节点[Add]：int,float,string,binary\n");
    dx["szKey_Adds"].Add(123).Add(13.14).Add("hello_Adds").Add(1024, malloc(1024));
    DatX dxAdds = dx["szKey_Adds"];
    printf("\t打印子节点[szKey_Adds]：%d,%f,%s,[0x%08X,%d]\n", dxAdds.Get(0U).I, dxAdds.Get(1).F, dxAdds.Get(2).S, dxAdds.Get(3).v, dxAdds.Get(3).n);
    printf("\t根节点内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx.Mem(), dx.Len(), dx.Cnt());
    printf("\n\n");

    getchar();
    system("pause");

    return 0;
}

