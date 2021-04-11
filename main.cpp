#include "MemJson.h"

using namespace x2lib;

int main()
{
    MemJson dx;
	{
		const char *pJson01 = R"({"aaa":521,"bbb":13.14,"ccc":"hello"})";
		const char *pJson02 = R"(["elem0","elem1","elem2"])";
		const char *pJson03 = R"({"aaa":521,"bbb":13.14,"ccc":"hello","ddd":["elem0","elem1","elem2"]})";
		dx.Parse(pJson01);
		dx("ccc").Parse(pJson02);
		char szJson[1024] = { 0 };
		dx.Print(szJson, 2048, false);
		printf("\t[MemJson->Json]: \n%s\n", szJson);
		printf("\t[MemJson->Json]: \n%s\n", szJson);
	}

    printf("添加100个int元素，100个double元素，100个string元素：\n");
    for (int i = 0; i < 100; i++)
    {
        char szKey[128] = { 0 };
        sprintf(szKey, "szKey_int_%d", i);
        dx.Put(szKey, i);
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
        dx.Put(szKey, "%s_%d", "hello_string", i);
    }

    printf("添加完成，打印结果：\n");
    printf("\t第10~14个元素：%d, %d, %d, %d, %d\n", dx.Get(10).I, dx.Get(11).I, dx.Get(12).I, dx.Get(13).I, dx.Get(14).I);
    printf("\t第110~114个元素：%.6f, %.6f, %.6f, %.6f, %.6f\n", dx.Get(110).F, dx.Get(111).F, dx.Get(112).F, dx.Get(113).F, dx.Get(114).F);
    printf("\t第210~214个元素：%s, %s, %s, %s, %s\n", dx.Get(210).S, dx.Get(211).S, dx.Get(212).S, dx.Get(213).S, dx.Get(214).S);
    printf("\t根节点内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx.Mem(), dx.Len(), dx.Cnt());

	printf("\n\n");
	printf("覆盖第50~54个元素，"); dx.Put("szKey_50", "override_50"); dx.Put("szKey_51", "override_51"); dx.Put("szKey_52", "override_52"); dx.Put("szKey_53", "override_53"); dx.Put("szKey_54", "override_54");
	printf("覆盖第150~154个元素，"); dx.Put("szKey_150", "override_150"); dx.Put("szKey_151", "override_151"); dx.Put("szKey_152", "override_152"); dx.Put("szKey_153", "override_153"); dx.Put("szKey_154", "override_154");
	printf("覆盖第250~254个元素\n"); dx.Put("szKey_250", "override_250"); dx.Put("szKey_251", "override_251"); dx.Put("szKey_252", "override_252"); dx.Put("szKey_253", "override_253"); dx.Put("szKey_254", "override_254");
	printf("\t根节点内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx.Mem(), dx.Len(), dx.Cnt());

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
        dx(szKey).Add(100).Add(102.345).Add("hello_array_string_%d", i);
    }
    printf("\t根节点内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx.Mem(), dx.Len(), dx.Cnt());
    printf("获取第1个数组信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx["szKey_arr_1"].Mem(), dx["szKey_arr_1"].Len(), dx["szKey_arr_1"].Cnt());
    for (unsigned int i = 0; i < dx["szKey_arr_1"].Cnt(); i++)
    {
        printf("\tszKey_arr_1_%d: %s\n", i, dx["szKey_arr_1"].Get(i).S);
    }
    printf("\n\n");

    printf("刪除第1个数组第0,1个元素：\n");
    dx["szKey_arr_1"].Del(1);
    dx["szKey_arr_1"].Del(0);
    printf("再次获取第1个数组信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx["szKey_arr_1"].Mem(), dx["szKey_arr_1"].Len(), dx["szKey_arr_1"].Cnt());
    for (unsigned int i = 0; i < dx["szKey_arr_1"].Cnt(); i++)
    {
        printf("\tszKey_arr_1_%d: %s\n", i, dx["szKey_arr_1"].Get(i).S);
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
    printf("\t打印：%d,%f,%s,[0x%08X,%d]\n", dx.Get(index0).I, dx.Get(index0 + 1).F, dx.Get(index0 + 2).S, dx.Get(index0 + 3).v, dx.Get(index0 + 3).n);
    printf("\t根节点内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx.Mem(), dx.Len(), dx.Cnt());
    printf("\n\n");

    printf("向根节点添加一个含有4个键值对元素的子节点[使用()自动创建子节点]：int,float,string,binary\n");
    dx("szKey_Puts").Put("szKey_Puts_int", 123).Put("szKey_Puts_float", 13.14).Put("szKey_Puts_string", "hello_Puts").Put("szKey_Puts_Binary1024", 1024, malloc(1024));
    MemJson dxPuts = dx["szKey_Puts"];
    printf("\t打印子节点[szKey_Puts]：%d,%f,%s,[0x%08X,%d]\n", dxPuts.Get("szKey_Puts_int").I, dxPuts.Get("szKey_Puts_float").F, dxPuts.Get("szKey_Puts_string").S, dxPuts.Get("szKey_Puts_Binary1024").v, dxPuts.Get("szKey_Puts_Binary1024").n);
    printf("\t根节点内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx.Mem(), dx.Len(), dx.Cnt());
    printf("\n\n");

    printf("向根节点添加一个含有4个数组元素的子节点[使用Put创建子节点]：int,float,string,binary\n");
    MemJson dxNode;
    dxNode.Add(123).Add(13.14).Add("hello_Adds").Add(1024, malloc(1024));
    dx.Put("szKey_Adds", dxNode);
    MemJson dxAdds = dx["szKey_Adds"];
    printf("\t打印子节点[szKey_Adds]：%d,%f,%s,[0x%08X,%d]\n", dxAdds.Get(0U).I, dxAdds.Get(1).F, dxAdds.Get(2).S, dxAdds.Get(3).v, dxAdds.Get(3).n);
    printf("\t根节点内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", dx.Mem(), dx.Len(), dx.Cnt());
    printf("\n\n");


    printf("\n\n\n----------------演示通过MemJson实现MemJson与Json字符串互转【感谢cJSON库】----------------\n\n");
    MemJson memJson;
    char szJson[2048] = { 0 };
    const char *pJson01 = R"({"aaa":521,"bbb":13.14,"ccc":"hello"})";
    printf("1.单级对象：%s\n", pJson01);
    printf("\t[Json->MemJson]：转换%s\n", memJson.Parse(pJson01)?"成功":"失败！！！");
    memJson.Print(szJson, sizeof(szJson), true);
    printf("\t[MemJson->Json]: \n%s\n",szJson);
    printf("\t内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", memJson.Mem(), memJson.Len(), memJson.Cnt());
    printf("\n\n");

    const char *pJson02 = R"(["elem0","elem1","elem2"])";
    printf("2.单级数组：%s\n", pJson02);
    printf("\t[Json->MemJson]：转换%s\n", memJson.Parse(pJson02) ? "成功" : "失败！！！");
    memJson.Print(szJson, sizeof(szJson), true);
    printf("\t[MemJson->Json]: \n%s\n", szJson);
    printf("\t内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", memJson.Mem(), memJson.Len(), memJson.Cnt());
    printf("\n\n");

    const char *pJson03 = R"({"aaa":521,"bbb":13.14,"ccc":"hello","ddd":["elem0","elem1","elem2"]})";
    printf("3.对象包数组：%s\n", pJson03);
    printf("\t[Json->MemJson]：转换%s\n", memJson.Parse(pJson03) ? "成功" : "失败！！！");
    memJson.Print(szJson, sizeof(szJson), true);
    printf("\t[MemJson->Json]: \n%s\n", szJson);
    printf("\t内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", memJson.Mem(), memJson.Len(), memJson.Cnt());
    printf("\n\n");

    const char *pJson04 = R"([{"aaa0":521,"bbb0":13.14,"ccc0":"hello"},{"aaa1":521,"bbb1":13.14}])";
    printf("4.数组包对象：%s\n", pJson04);
    printf("\t[Json->MemJson]：转换%s\n", memJson.Parse(pJson04) ? "成功" : "失败！！！");
    memJson.Print(szJson, sizeof(szJson), true);
    printf("\t[MemJson->Json]:\n%s\n", szJson);
    printf("\t内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", memJson.Mem(), memJson.Len(), memJson.Cnt());
    printf("\n\n");

    const char *pJson05 = R"([{"aaa0":521,"bbb0":13.14,"hello"},{"aaa1":521,"bbb1":13.14}])";
    printf("5.混杂数组1：%s\n", pJson05);
    printf("\t[Json->MemJson]：转换%s\n", memJson.Parse(pJson05) ? "成功" : "失败！！！");
    memJson.Print(szJson, sizeof(szJson), true);
    printf("\t[MemJson->Json]:\n%s\n", szJson);
    printf("\t内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", memJson.Mem(), memJson.Len(), memJson.Cnt());
    {
        memJson.Clear();
        MemJson dx0, dx1;
        dx0.Put("aaa0", 521).Put("bbb0", 13.14).Add("hello");
        dx1.Put("aaa1", 521).Put("bbb1", 13.14);
        memJson.Add(dx0).Add(dx1);
        memJson.Print(szJson, sizeof(szJson), true, true);
        printf("直接由MemJson转“Json”:\n%s\n", szJson);
        printf("\t内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", memJson.Mem(), memJson.Len(), memJson.Cnt());
    }
    printf("\n\n");

    const char *pJson06 = R"({"aaa":521,"bbb":13.14,"hello",["elem0","elem1","elem2"],nokey_stream1024,"binary_key":stream2048})";
    printf("6.混杂数组2：%s\n", pJson06);
    printf("\t[Json->MemJson]：转换%s\n", memJson.Parse(pJson06) ? "成功" : "失败！！！");
    memJson.Print(szJson, sizeof(szJson), true);
    printf("\t[MemJson->Json]:\n%s\n", szJson);
    printf("\t内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", memJson.Mem(), memJson.Len(), memJson.Cnt());
    {
        memJson.Clear();
        memJson.Put("aaa", 521).Put("bbb", 13.14).Add("hello");
        MemJson dx0;
        dx0.Add("elem0").Add("elem1").Add("elem2");
        memJson.Add(dx0);
        memJson.Add(1024, malloc(1024));
        memJson.Put("binary_key", 2048, malloc(2048));
        memJson.Print(szJson, sizeof(szJson), true, true);
        printf("直接由MemJson转“Json”:\n%s\n", szJson);
        printf("\t内存信息：内存起始[0x%08X]，内存大小[%d B]，元素个数[%d]\n", memJson.Mem(), memJson.Len(), memJson.Cnt());
    }
    printf("\n-------------------------------演示结束-------------------------------\n");

    getchar();

    return 0;
}

