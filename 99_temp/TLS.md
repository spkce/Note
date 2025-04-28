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



### aarch64 tls 寄存器
uint64_t read_tpidr_el0() {
    uint64_t val;
    asm volatile("mrs %0, tpidr_el0" : "=r"(val));
    return val;
}



# 通过代码 Hook 内存分配函数‌‌3
‌自定义 malloc/free‌：
通过 LD_PRELOAD 注入自定义库，拦截内存操作并记录 tcache 状态。
void* malloc(size_t size) {
    void *ptr = orig_malloc(size);
    log_tcache_state();  // 记录 tcache 状态到日志
    return ptr;
}

# 线程控制块（TCB） 
struct pthread
glibc\nptl\descr.h

线程栈起始地址和大小
```c++
 /* If nonzero, pointer to the area allocated for the stack and guard. */
  void *stackblock;
  /* Size of the stackblock area including the guard.  */
  size_t stackblock_size;
```

#

内存布局上看，

x86_64 低 -> 高， （tls）fs_base 指向pthred结构地址
```
__thread变量区域, pthred结构(struct pthread)，栈区（栈底）
```

aarch64 低 -> 高，（tls）TPIDR_EL0 指向__thread变量区域
```
__thread变量区域，pthred结构(struct pthread)，栈区（栈底）
```

（可能有问题）主线程的 TLS 位于进程初始化时分配的栈区域末尾；子线程的 TLS 通过 mmap 分配的独立栈空间末尾存放‌

#
__thread修饰的变量必须是POD（Plain Old Data）类型，不支持class等高级语言特性。__thread变量在线程生命周期一直存在，在线程销毁时释放。需要注意的是，由于__thread并不能指定销毁方法，当我们定义一个__thread修饰的指针变量，并在线程运行中malloc内存后，线程结束仅会将__thread变量指针置NULL，需要开发者手动free内存。

# pthread同时提供了以下API实现TLS的功能
```c++
//create key
pthread_key_t key = 0;
pthread_key_create(&key, NULL); 
 
//write
struct kanchuan_struct data;
pthread_setspecific(key, &data);
 
//read
struct kanchuan_struct* = (struct kanchuan_struct *)pthread_getspecific(key)
```
每一个进程都有一个全局数组__pthread_keys来管理pthread_key_t
```c++
//nptl/internaltypes.h:
/* Thread-local data handling.  */
struct pthread_key_struct
{
  /* Sequence numbers.  Even numbers indicated vacant entries.  Note
     that zero is even.  We use uintptr_t to not require padding on
     32- and 64-bit machines.  On 64-bit machines it helps to avoid
     wrapping, too.  */
  uintptr_t seq;
 
  /* Destructor for the data.  */
  void (*destr) (void *);
};
 
 
//sysdeps/unix/sysv/linux/bits/local_lim.h
/* This is the value this implementation supports.  */
#define PTHREAD_KEYS_MAX 1024
 
 
//nptl/pthread_keys.c
/* Table of the key information.  */
struct pthread_key_struct __pthread_keys[PTHREAD_KEYS_MAX];
```

# __thread和pthread specific API对比

存储区域/寻址方式不同
pthread specific API定义的数据，是通过struct pthread结构体的specific_1stblock数组和specific二级数组寻址，而__thread变量则是通过fp寄存器偏移寻址。

性能/效率不同
由于__thread是通过fp寄存器偏移寻址，性能比pthread specific API高。

能存储的数据不同
__thread只能修饰POD类型变量，对于指针类型的数据，有申请内存时需要手动销毁；而pthread specific API支持传入销毁方法，支持所有数据类型。

支持的数据个数不同
理论上只要栈不被占满，__thread可以无限定义（存疑？）；而pthread specific API只能创建PTHREAD_KEYS_MAX个key，但可以通过结构体等的方式，使用一个key存储多个值。
