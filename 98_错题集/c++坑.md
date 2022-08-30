### 1. cin

cin会被空格截断，若需要空格可以
```c++
char str[100];
cin.getline(str, sizeof(str));
```


一个进程的子线程创建一个子进程。若子线程退出，会发送信号给子进程（若子进程 prctl(PR_SET_PDEATHSIG, SIGINT)）



对于服务器而言，若创建会话套接字后，进程结束后，若该会话（server->client）为TCP状态为time_wait。则会让该端口再次绑定时失败.
而（client->server）会话为time_wait则不会


进程正常退出时，全局对象会被析构，静态局部对象也会析构。但是new出来的对象不会创建.
进程不正常退出时(被kill)，不会执行析构