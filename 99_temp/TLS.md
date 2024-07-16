# Thread Local Storage（线程局部存储,TLS）
```c++
#include<iostream>
#include<pthread.h>
#include<unistd.h>

using namespace std;
__thread int iVar = 100;

void* Thread1(void *arg)
{
    iVar += 200;
    cout<<"Thead1 Val : "<<iVar<<endl;
}

void* Thread2(void *arg)
{
    iVar += 400;
    sleep(1);
    cout<<"Thead2 Val : "<<iVar<<endl;
}

int main()
{
    pthread_t pid1, pid2;
    pthread_create(&pid1, NULL, Thread1, NULL);
    pthread_create(&pid2, NULL, Thread2, NULL);

    pthread_join(pid1, NULL);
    pthread_join(pid2, NULL);

    return 0;
}
```
两个线程pid1、pid2都用了iVar，但是各自都拥有各自的iVar并计算相应的结果，这样就达到了线程基本的数据独立。