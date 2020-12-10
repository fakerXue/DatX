# MemJson（之前的DatX）
一种紧凑的数据结构，适合网络传输，配置存储等，一定程度上能替代json，读写均优于json，0秒解析。自带demo演示。
```C++
/*************************************************************************
** Desc		: 一种紧凑的数据结构，适合网络传输，配置存储等，一定程度上能替代json，读写均优于json，0秒解析
**				内存结构如下： {MemJson::__len__,MemJson::__type__,[szKey01,xType01,nBin01,pBin01],[szKey02,xType02,nBin02,pBin02],...}
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
**              11.(i)：获取当前MemJson的第i个子节点，若不存在或非节点则进行创建或置空；
*************************************************************************/
```

![image](https://github.com/fakerXue/DatX/blob/main/TestCase_DatX.png)
![image](https://github.com/fakerXue/DatX/blob/main/TestCase_MemJson.png)

