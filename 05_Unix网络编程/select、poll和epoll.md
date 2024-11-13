# select 和 epoll 的区别

1. I/O 模型不同：select 使用轮询模型，每次遍历所有的文件描述符集合，效率低下；而 epoll 是基于事件驱动的模型，通过回调函数只处理活跃的文件描述符，效率更高。
2. 文件描述符数量限制不同：select 的文件描述符数量被限制在 1024 左右，对于大规模的连接，需要使用 fd_set 多次扩展或者使用其他的多路复用技术；而 epoll 可以支持数万个文件描述符，因此适用于高并发场景。
3. 触发方式不同：select 和 epoll 对可读事件和可写事件的触发方式不同。select 只能检测是否可以从文件读取数据或将数据写入文件，无法区分数据是进入还是出去，无法保证数据完整性；而 epoll 通过边沿触发（ET）模式，只有当状态发生变化时才会触发事件，能够确保数据完整性。
4. 内核实现机制不同：select 是系统调用，每次调用都需要把全部描述符集合从用户态传输到内核态，并且需要判断每个描述符是否就绪；epoll 则是内核空间与用户空间共享一块内存，只需要在内核空间注册一个事件表，然后等待事件的通知即可，不需要对已注册的文件描述符进行遍历。

epoll 是更加高效和灵活的多路复用技术，尤其适用于处理大规模并发连接的场景，所以通常在企业级应用里面用得比较多。但是在一些特定的场景下，select还是有优势的，比如说处理少量文件描述符或者跨平台知识的情况下，select是一个更好的选择。

# select 的运用
```c++
#include <sys/select.h>
#include <sys/time.h>
int select(int maxfd, fd_set* readset, fd_set* writeset, fd_set* exceptset, 
const struct timeval* timeout);
//maxfd：监视对象文件描述符数量。
//readset：将所有关注“是否存在待读取数据”的文件描述符注册到fd_set变量，并传递其地址值。
//writeset： 将所有关注“是否可传输无阻塞数据”的文件描述符注册到fd_set变量，并传递其地址值。
//exceptset：将所有关注“是否发生异常”的文件描述符注册到fd_set变量，并传递其地址值。
//timeout：调用select后，为防止陷入无限阻塞状态，传递超时信息。
```
```c++
FD_ZERO(fd_set* fdset)： 将fd_set变量的所有位初始化为0。
FD_SET（int fd, fd_set* fdset）：在参数fd_set指向的变量中注册文件描述符fd的信息。
FD_CLR(int fd, fd_set* fdset)：参数fd_set指向的变量中清除文件描述符fd的信息。
FD_ISSET(int fd, fd_set* fdset)：若参数fd_set指向的变量中包含文件描述符fd的信息，则返回真
```
使用实例：
```c++
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 30

int main(int argc, char* argv[])
{
    fd_set reads,temps;
    int result, str_len;
    char buf[BUF_SIZE];
    struct timeval timeout;
    FD_ZERO(&reads);
    FD_SET(0, &reads);//监视文件描述符0的变化, 即标准输入的变化
    /*超时不能在此设置！
    因为调用select后，结构体timeval的成员tv_sec和tv_usec的值将被替换为超时前剩余时间.
    调用select函数前，每次都需要初始化timeval结构体变量.
    timeout.tv_sec = 5;
    timeout.tv_usec = 5000;*/
    while(1)
    {
        /*将准备好的fd_set变量reads的内容复制到temps变量，因为调用select函数后，除了发生变化的fd对应位外，
        剩下的所有位都将初始化为0，为了记住初始值，必须经过这种复制过程。*/
        temps = reads;
        //设置超时
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        //调用select函数. 若有控制台输入数据，则返回大于0的整数，如果没有输入数据而引发超时，返回0.
        result = select(1, &temps, 0, 0, &timeout);
        if(result == -1)
        {
            perror("select() error");
            break;
        }
        else if(result == 0)
        {
            puts("timeout");
        }
        else
        {
            //读取数据并输出
            if(FD_ISSET(0, &temps))
            {
                str_len = read(0, buf, BUF_SIZE);
                buf[str_len] = 0;
                printf("message from console: %s", buf);
            }
        }
    }

    return 0;
}
```
select监控的文件描述符主要受到以下因素的影响‌：
‌1. 文件描述符的数量限制‌：select函数监控的文件描述符数量受到限制。在Linux系统中，select函数监控的文件描述符数量不能超过FD_SETSIZE，这个值通常在头文件中定义，默认为1024。如果需要监控的文件描述符数量超过这个限制，程序将无法正常工作‌。
2. ‌性能问题‌：当监控的文件描述符数量较多时，select的性能会受到影响。select采用轮询模型，当文件描述符数量较多时，select需要遍历所有文件描述符，这会导致性能下降‌。
3. ‌多线程环境下的竞态条件‌：在多线程环境下，不同线程可能会同时对同一个文件描述符进行操作，导致竞态条件的发生。这会影响select函数的工作效果，可能导致程序出现错误或异常‌。
4. ‌阻塞问题‌：select函数在执行时会阻塞，直到至少有一个文件描述符变为可读或可写。如果其中一个线程在select函数中阻塞，可能会影响其他线程的执行。
5. ‌普通文件描述符的行为差异‌：对于普通文件描述符，select的行为与socket或串口略有不同。普通文件是否可读主要依赖于文件的当前读取位置以及文件的实际内容。如果一个普通文件已经打开且没有到达文件尾(EOF)，select会将其标记为可读状态。一旦文件被读取到EOF，之后的select调用（若没有新的数据写入文件）将不会将其标记为可读，除非文件被重新写入或文件指针被重置‌
# poll
poll是一种IO多路复用技术，它使用一个动态数组来存储所有需要监控的文件描述符，并通过轮询的方式检查这些文件描述符的状态。poll的优点在于它不限制文件描述符的数量，但缺点是随着文件描述符数量的增加，性能会下降，因为需要遍历所有文件描述符来检查是否有事件发生‌
```c++
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```
fds是一个struct pollfd结构体数组，包含需要监控的文件描述符及其事件；nfds是数组中文件描述符的数量；timeout是等待事件发生的最大时间（毫秒）‌。
使用方法：设置文件描述符的事件类型（如可读、可写、错误），调用poll函数，根据返回值处理事件‌。
# epoll
```c++
#include <sys/epoll.h>
//文件描述符的创建
int epoll_create ( int size ); //创建poll句柄后，它就是会占用一个fd值/必须调用close()关闭，否则可能导致fd被耗尽
```
```c++
/*
fd：要操作的文件描述符
op：指定操作类型
操作类型：
EPOLL_CTL_ADD：往事件表中注册fd上的事件
EPOLL_CTL_MOD：修改fd上的注册事件
EPOLL_CTL_DEL：删除fd上的注册事件
*/
struct epoll_event
{
__unit32_t events; // epoll事件
epoll_data_t data; // 用户数据
};
typedef union epoll_data
{
void ptr; //指定与fd相关的用户数据
int fd; //指定事件所从属的目标文件描述符
uint32_t u32;
uint64_t u64;
} epoll_data_t;
int epoll_ctl ( int epfd, int op, int fd, struct epoll_event event );
```

```c++
#include <sys/epoll.h>
int epoll_wait ( int epfd, struct epoll_event* events, int maxevents, int timeout );
```
在给定的timeout时间内，当在监控的这些文件描述符中的某些文件描述符上有事件发生时，就返回用户态的进程。这个函数用于等待事件的发生．第二个参数是用户自己开辟的一块事件数组，用于存储就绪的事件，第三个参数为这个数组的最大值，就是第二个参数事件数组的最大值，用户想监听fd的个数，第四个参数为超时时间(0表示立即返回，-1表示永久阻塞，直到有就绪事件)

实例
```c++
#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
 
int main(void)
{
   int epfd,nfds;
   struct epoll_event ev,events[5]; //ev用于注册事件，数组用于返回要处理的事件
   epfd = epoll_create(1); //只需要监听一个描述符
   ev.data.fd = STDIN_FILENO;//表示要监听的文件描述符——标准输入
   ev.events = EPOLLIN ; //监听读状态同时设置ET模式  EPOLLET
   epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev); //注册epoll事件,将标准输入文件描述符加入到 epoll 实例中进行监听
   for(;;)
   {
       nfds = epoll_wait(epfd, events, 5, -1);//等待事件发生，当标准输入有可读事件发生时，会返回就绪的事件列表
       for(int i = 0; i < nfds; i++)
        {
             if(events[i].data.fd==STDIN_FILENO)//判断返回的事件是否是标准输入的事件
             
             {
                char buf[1024]={0};
                read(STDIN_FILENO,buf,sizeof(buf));
                printf("welcome to epoll's word!\n");
             }
            }
    }
}

```

# 优缺点

poll解决的select问题：

`poll` 解决了 `select` 函数的一些限制和问题，主要包括以下几点：

1. **文件描述符数量的限制**：
- 在一些早期的 UNIX 系统中，`select` 函数的参数是一个固定大小的数组，通常由 `FD_SETSIZE` 定义，这限制了同时监视的文件描述符数量。
- 而 `poll` 函数使用了更加灵活的数据结构，内核使用链表来存储监视的文件描述符，因此不受文件描述符数量的限制。

2. **效率问题**：
- `select` 函数在处理大量文件描述符时性能可能会受到影响，因为每次调用都要传递一个大的数组，并且需要轮询所有的文件描述符。
- `poll` 函数的实现方式更加高效，它可以处理更多的文件描述符，并且在文件描述符数量较大时性能更好。

3. **API 的简洁性**：
- `select` 函数的使用相对繁琐，需要设置文件描述符集合的大小限制，并且在处理事件时的参数较多。
- `poll` 函数的使用相对简洁，不需要设置文件描述符集合的大小限制，并且在处理事件时的参数也较少。

综上所述，`poll` 函数通过使用更加灵活的数据结构和简化 API 等方式，解决了 `select` 函数的一些限制和问题，使得在处理大量文件描述符时性能更好，同时也更加易于使用。因此，在开发网络应用时，通常会优先选择使用 `poll` 函数。

epoll解决的poll问题：

`epoll` 解决了 `poll` 函数的一些性能瓶颈和限制，主要包括以下几点：

1. **效率问题**：
- 在处理大量连接时，`poll` 函数的性能可能会受到影响，因为每次调用都需要遍历所有的文件描述符，即使其中只有少数的文件描述符上有事件发生。
- `epoll` 函数使用了事件驱动的方式，只有在文件描述符上发生事件时才会通知应用程序，避免了不必要的遍历开销，因此在处理大量连接时性能更好。

2. **文件描述符数量的限制**：
- 和 `select`、`poll` 类似，`epoll` 函数也没有固定的文件描述符数量限制，因此可以处理更多的文件描述符。

3. **支持边沿触发模式**：
- `epoll` 函数支持边沿触发（Edge Triggered）模式，只有在文件描述符上的状态发生变化时才会触发事件通知，避免了水平触发模式下可能产生的事件重复通知问题。
- 这使得 `epoll` 在处理高并发、高负载的场景下更加灵活和高效。

4. **更少的系统调用**：
- `epoll` 使用了三个系统调用 `epoll_create`、`epoll_ctl` 和 `epoll_wait` 来实现对事件的操作和等待，相比之下，`poll` 函数则需要更多的系统调用