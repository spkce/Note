# <center>AIO</center>

## glibc的AIO
```c++
struct aiocb
{
    //要异步操作的文件描述符
    int aio_fildes;
    //用于lio操作时选择操作何种异步I/O类型
    int aio_lio_opcode;
    //异步读或写的缓冲区的缓冲区
    volatile void *aio_buf;
    //异步读或写的字节数
    size_t aio_nbytes;
    //异步通知的结构体
    struct sigevent aio_sigevent;

```
例子：
```c++
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<assert.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<fcntl.h>
#include<aio.h>
 
 
#define BUFFER_SIZE 1024
 
int MAX_LIST = 2;
 
int main(int argc,char **argv)
{
    //aio操作所需结构体
    struct aiocb rd;
 
    int fd,ret,couter;
 
    fd = open("test.txt",O_RDONLY);
    if(fd < 0)
    {
        perror("test.txt");
    }
 
    //将rd结构体清空
    bzero(&rd,sizeof(rd));

    //为rd.aio_buf分配空间
    rd.aio_buf = malloc(BUFFER_SIZE + 1);
 
    //填充rd结构体
    rd.aio_fildes = fd;
    rd.aio_nbytes =  BUFFER_SIZE;
    rd.aio_offset = 0;
 
    //进行异步读操作
    ret = aio_read(&rd);
    if(ret < 0)
    {
        perror("aio_read");
        exit(1);
    }
 
    couter = 0;
//  循环等待异步读操作结束 也可以使用aio_suspend处理
    while(aio_error(&rd) == EINPROGRESS)
    {
        printf("第%d次\n",++couter);
    }
    //获取异步读返回值
    ret = aio_return(&rd);
    printf("\n\n返回值为:%d",ret);
    return 0;
}
```

## libaio
linux下有aio封装，aio_*系列的调用是glibc提供的，是glibc用线程+阻塞调用来模拟的，性能很差，为了能更多的控制io行为，可以使用更为低级libaio。

```c++
int io_setup(int maxevents, io_context_t *ctxp);
int io_destroy(io_context_t ctx);
int io_submit(io_context_t ctx, long nr, struct iocb *ios[]);
int io_cancel(io_context_t ctx, struct iocb *iocb, struct io_event *evt);
int io_getevents(io_context_t ctx_id, long min_nr, long nr, struct io_event *events, struct timespec *timeout);
```
宏:这五个宏定义都是操作struct iocb的结构体。struct iocb是libaio中很重要的一个结构体，用于表示IO，但是其结构略显复杂，为了保持封装性不建议直接操作其元素而用上面五个宏定义操作。
```c++
void io_set_callback(struct iocb *iocb, io_callback_t cb);
void io_prep_pwrite(struct iocb *iocb, int fd, void *buf, size_t count, long long offset);
void io_prep_pread(struct iocb *iocb, int fd, void *buf, size_t count, long long offset);
void io_prep_pwritev(struct iocb *iocb, int fd, const struct iovec *iov, int iovcnt, long long offset);
void io_prep_preadv(struct iocb *iocb, int fd, const struct iovec *iov, int iovcnt, long long offset);
```

libaio的使用并不复杂，过程为：libaio的初始化，io请求的下发和回收，libaio销毁。

### libaio的初始化和销毁

观察libaio五个主要API，都用到类型为io_context的变量，这个变量为libaio的工作空间。不用具体去了解这个变量的结构，只需要了解其相关操作。创建和销毁libaio分别用到io_setup（也可以用io_queue_init，区别只是名字不一样而已）和io_destroy。
```c++
int io_setup(int maxevents, io_context_t *ctxp);
```
这里也需要open需要操作的文件，注意设置O_DIRECT标志
```c++
int io_destroy(io_context_t ctx);
```

### libaio读写请求的下发和回收

libaio的读写请求都用io_submit下发。下发前通过io_prep_pwrite和io_prep_pread生成iocb的结构体，做为io_submit的参数。这个结构体中指定了读写类型、起始扇区、长度和设备标志符。

libaio的初始化不是针对一个具体设备进行初始，而是创建一个libaio的工作环境。读写请求下发到哪个设备是通过open函数打开的设备标志符指定。

读写请求下发之后，使用io_getevents函数等待io结束信号
io_getevents返回events的数组，其参数events为数组首地址，nr为数组长度（即最大返回的event数），min_nr为最少返回的events数。timeout可填NULL表示无等待超时。io_event结构体的声明为：
```c++
struct io_event {
    PADDEDptr(void *data, __pad1);
    PADDEDptr(struct iocb *obj,  __pad2);
    PADDEDul(res,  __pad3);
    PADDEDul(res2, __pad4);
};
//res为实际完成的字节数；res2为读写成功状态，0表示成功；
//obj为之前下发的struct iocb结构体。这里有必要了解一下struct iocb这个结构体的主要内容：
//iocbp->iocb.u.c.nbytes 字节数
//iocbp->iocb.u.c.offset 偏移
//iocbp->iocb.u.c.buf 缓冲空间
//iocbp->iocb.u.c.flags 读写

```
struct iocb除了自带的元素外，还留有供用户自定义的元素，包括回调函数和void *的data指针。如果在请求下发前用io_set_callback绑定用户自定义的回调函数，那么请求返回后就可以显示的调用该函数。回调函数的类型为：
```c++
void callback_function(io_context_t ctx, struct iocb *iocb, long res, long res2);
```
另外，还可以通过iocbp->data指针挂上用户自己的数据。

```c++
#include <stdlib.h>
#include <stdio.h>
#include <libaio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libaio.h>

int srcfd = -1;
int odsfd = -1;

#define AIO_BLKSIZE 1024
#define AIO_MAXIO 64

static void wr_done(io_context_t ctx, struct iocb *iocb, long res, long res2)
{
	if (res2 != 0)
	{
		printf("aio write error\n");
	}
	if (res != iocb->u.c.nbytes)
	{
		printf("write missed bytes expect %d got %d\n", iocb->u.c.nbytes, res);
		exit(1);
	}

	free(iocb->u.c.buf);
	free(iocb);
}

static void rd_done(io_context_t ctx, struct iocb *iocb, long res, long res2)
{
	/*library needs accessors to look at iocb*/
	int iosize = iocb->u.c.nbytes;
	char *buf = (char *)iocb->u.c.buf;
	off_t offset = iocb->u.c.offset;
	int tmp;
	char *wrbuff = NULL;

	if (res2 != 0)
	{
		printf("aio read\n");
	}
	if (res != iosize)
	{
		printf("read missing bytes expect %d got %d", iocb->u.c.nbytes, res);
		exit(1);
	}

	/*turn read into write*/
	tmp = posix_memalign((void **)&wrbuff, getpagesize(), AIO_BLKSIZE);
	if (tmp < 0)
	{
		printf("posix_memalign222\n");
		exit(1);
	}

	snprintf(wrbuff, iosize + 1, "%s", buf);

	printf("wrbuff-len = %d:%s\n", strlen(wrbuff), wrbuff);
	printf("wrbuff_len = %d\n", strlen(wrbuff));
	free(buf);

	io_prep_pwrite(iocb, odsfd, wrbuff, iosize, offset);
	io_set_callback(iocb, wr_done);

	if (1 != (res = io_submit(ctx, 1, &iocb)))
		printf("io_submit write error\n");

	printf("\nsubmit  %d  write request\n", res);
}

void main(int args, void *argv[])
{
	int length = sizeof("abcdefg");
	char *content = (char *)malloc(length);
	io_context_t myctx;
	int rc;
	char *buff = NULL;
	int offset = 0;
	int num, i, tmp;

	if (args < 3)
	{
		printf("the number of param is wrong\n");
		exit(1);
	}

	if ((srcfd = open(argv[1], O_RDWR)) < 0)
	{
		printf("open srcfile error\n");
		exit(1);
	}

	printf("srcfd=%d\n", srcfd);

	lseek(srcfd, 0, SEEK_SET);
	write(srcfd, "abcdefg", length);

	lseek(srcfd, 0, SEEK_SET);
	read(srcfd, content, length);

	printf("write in the srcfile successful,content is %s\n", content);

	if ((odsfd = open(argv[2], O_RDWR)) < 0)
	{
		close(srcfd);
		printf("open odsfile error\n");
		exit(1);
	}

	memset(&myctx, 0, sizeof(myctx));
	io_queue_init(AIO_MAXIO, &myctx);

	struct iocb *io = (struct iocb *)malloc(sizeof(struct iocb));
	int iosize = AIO_BLKSIZE;
	//预对齐内存的分配
	tmp = posix_memalign((void **)&buff, getpagesize(), AIO_BLKSIZE);
	if (tmp < 0)
	{
		printf("posix_memalign error\n");
		exit(1);
	}
	if (NULL == io)
	{
		printf("io out of memeory\n");
		exit(1);
	}

	io_prep_pread(io, srcfd, buff, iosize, offset);

	io_set_callback(io, rd_done);

	rc = io_submit(myctx, 1, &io);

	if (rc < 0)
		printf("io_submit read error\n");

	printf("\nsubmit  %d  read request\n", rc);

	//m_io_queue_run(myctx);

	struct io_event events[AIO_MAXIO];
	io_callback_t cb;

	num = io_getevents(myctx, 1, AIO_MAXIO, events, NULL);
	printf("\n%d io_request completed\n\n", num);

	for (i = 0; i < num; i++)
	{
		cb = (io_callback_t)events[i].data;
		struct iocb *io = events[i].obj;

		printf("events[%d].data = %x, res = %d, res2 = %d\n", i, cb, events[i].res, events[i].res2);
		cb(myctx, io, events[i].res, events[i].res2);
	}
}
```

### libaio 驱动

file_operations包含三个和AIO相关的成员函数：
```c++
ssize_t (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, loff_t);//初始化一个异步的读取操作   
ssize_t (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, loff_t);//初始化一个异步的写入操作 
int (*aio_fsync) (struct kiocb *, int datasync); //异步刷新待处理的数据   
```
一般而言，块设备和网络设备在linux核心代码中实现了，而字符设备一般不需要实现AIO支持。