万能引用完美转发？萃取技术退化技术？

c++中为什么不提倡使用vector<bool>？

IO框架 Boost.asio boost.fiber boost.beast 组合起来就是一个基于协程的io框架

堆栈跟踪 Boost.Stacktrace 需要配合libbacktrace库

json 一个只有头文件的json库，不多说很强大很现代c++

http网络库：libhv大部分http网络库都只支持linux系统，想找个跨平台的http库，能在windows上开发，linux上部署。github上star最高的cpp-httplib是个玩具。之前一直用libmicrohttpd+libcurl，这两个都是C写的，都是我自己封装成C++来用。由于libmicrohttpd是LGPL协议的，所以后来换成了libhv。libhv既有支持服务端也有客户端，还没有第三方依赖，至少目前没发现更好的。

lower_bound和upper_bound

```c++
int main(int argc, char const *argv[])
{
    std::map<uint64_t, uint64_t> mapRec;
    mapRec[0] = 1;
    mapRec[1] = 1;
    mapRec[2] = 1;
    mapRec[4] = 1;

    /*
    lower_bound(k)寻找  k <= x 并返回x的迭代器 
    upper_bound(k)寻找 k < x 并返回x的代器 
    */
    {
        auto it = mapRec.lower_bound(3); // 返回一个迭代器，指向键值 >= _Keyval 的第一个元素
        printf("map (%lu,%lu)\n", it->first, it->second);
    }

    {
        auto it = mapRec.upper_bound(3); //返回一个迭代器，指向键值 > _Keyval 的第一个元素。
        printf("map (%lu,%lu)\n", it->first, it->second);
    }


    std::vector<uint64_t> vecRec(5);
    vecRec[0] = 0;
    vecRec[1] = 1;
    vecRec[2] = 2;
    vecRec[3] = 3;
    vecRec[4] = 4;

    {
        auto it = std::lower_bound(vecRec.begin(), vecRec.end(), 3); 
        printf("vec (%lu)\n", *it);
    }

    {
        auto it = std::upper_bound(vecRec.begin(), vecRec.end(), 3);
        printf("vec (%lu)\n", *it);
    }
    return 0;
}
```

输出结果：

```shell
map (4,1)
map (4,1)
vec (3)
vec (4)
```
