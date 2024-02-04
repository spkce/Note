万能引用完美转发？萃取技术退化技术？

c++中为什么不提倡使用vector<bool>？



IO框架 Boost.asio boost.fiber boost.beast 组合起来就是一个基于协程的io框架

堆栈跟踪 Boost.Stacktrace 需要配合libbacktrace库

json 一个只有头文件的json库，不多说很强大很现代c++


http网络库：libhv大部分http网络库都只支持linux系统，想找个跨平台的http库，能在windows上开发，linux上部署。github上star最高的cpp-httplib是个玩具。之前一直用libmicrohttpd+libcurl，这两个都是C写的，都是我自己封装成C++来用。由于libmicrohttpd是LGPL协议的，所以后来换成了libhv。libhv既有支持服务端也有客户端，还没有第三方依赖，至少目前没发现更好的。

