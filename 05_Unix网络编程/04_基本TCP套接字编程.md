# <center>四、基本TCP套接字编程</center>

## 1. socket函数
```C++
#include "sys/socket.h"
int socket(int famliy, int type, int protocol); 
```
famliy为协议族，取值：

|	famliy	|	说明	|
|:---------------:|:---------------:|
|AF_INET|IPv4协议|
|AF_INET6|IPv6协议|
|AF_LOCAL|Unix域协议|
|AF_ROUTE|路由套接字|
|AF_KEY|秘钥套接字|

type为套接字类型，取值：
|	famliy	|	说明	|
|:---------------:|:---------------:|
|SOCK_STREAM|字节流套接字|
|SOCK_DGRAM|数据报套接字|
|SOCK_SEQPACKET|有序分组套接字|
|SOCK_RAW|原始套接字|

famliy和type的组合：
<table>
<tr><td></td><td>AF_INET</td><td>AF_INET6</td><td>AF_LOCAL</td><td>AF_ROUTE</td><td>AF_KEY</td></tr>
<tr><td>SOCK_STREAM</td><td>TCP|SCTP</td><td>TCP|SCTP</td><td>是</td><td></td><td></td></tr>
<tr><td>SOCK_DGRAM</td><td>UDP</td><td>UDP</td><td>是</td><td></td><td></td></tr>
<tr><td>SOCK_SEQPACKET</td><td>SCTP</td><td>SCTP</td><td>是</td><td></td><td></td></tr>
<tr><td>SOCK_RAW</td><td>IPv4</td><td>IPv6</td><td></td><td>是</td><td>是</td></tr>
<tr><td></td></tr>
</table>