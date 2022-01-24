# <center>AIO</center>

## glibc��AIO
```c++
struct aiocb
{
    //Ҫ�첽�������ļ�������
    int aio_fildes;
    //����lio����ʱѡ����������첽I/O����
    int aio_lio_opcode;
    //�첽����д�Ļ������Ļ�����
    volatile void *aio_buf;
    //�첽����д���ֽ���
    size_t aio_nbytes;
    //�첽֪ͨ�Ľṹ��
    struct sigevent aio_sigevent;

```
���ӣ�
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
    //aio��������ṹ��
    struct aiocb rd;
 
    int fd,ret,couter;
 
    fd = open("test.txt",O_RDONLY);
    if(fd < 0)
    {
        perror("test.txt");
    }
 
    //��rd�ṹ�����
    bzero(&rd,sizeof(rd));

    //Ϊrd.aio_buf����ռ�
    rd.aio_buf = malloc(BUFFER_SIZE + 1);
 
    //���rd�ṹ��
    rd.aio_fildes = fd;
    rd.aio_nbytes =  BUFFER_SIZE;
    rd.aio_offset = 0;
 
    //�����첽������
    ret = aio_read(&rd);
    if(ret < 0)
    {
        perror("aio_read");
        exit(1);
    }
 
    couter = 0;
//  ѭ���ȴ��첽���������� Ҳ����ʹ��aio_suspend����
    while(aio_error(&rd) == EINPROGRESS)
    {
        printf("��%d��\n",++couter);
    }
    //��ȡ�첽������ֵ
    ret = aio_return(&rd);
    printf("\n\n����ֵΪ:%d",ret);
    return 0;
}
```

## libaio
linux����aio��װ��aio_*ϵ�еĵ�����glibc�ṩ�ģ���glibc���߳�+����������ģ��ģ����ܺܲΪ���ܸ���Ŀ���io��Ϊ������ʹ�ø�Ϊ�ͼ�libaio��

```c++
int io_setup(int maxevents, io_context_t *ctxp);
int io_destroy(io_context_t ctx);
int io_submit(io_context_t ctx, long nr, struct iocb *ios[]);
int io_cancel(io_context_t ctx, struct iocb *iocb, struct io_event *evt);
int io_getevents(io_context_t ctx_id, long min_nr, long nr, struct io_event *events, struct timespec *timeout);
```
��:������궨�嶼�ǲ���struct iocb�Ľṹ�塣struct iocb��libaio�к���Ҫ��һ���ṹ�壬���ڱ�ʾIO��������ṹ���Ը��ӣ�Ϊ�˱��ַ�װ�Բ�����ֱ�Ӳ�����Ԫ�ض�����������궨�������
```c++
void io_set_callback(struct iocb *iocb, io_callback_t cb);
void io_prep_pwrite(struct iocb *iocb, int fd, void *buf, size_t count, long long offset);
void io_prep_pread(struct iocb *iocb, int fd, void *buf, size_t count, long long offset);
void io_prep_pwritev(struct iocb *iocb, int fd, const struct iovec *iov, int iovcnt, long long offset);
void io_prep_preadv(struct iocb *iocb, int fd, const struct iovec *iov, int iovcnt, long long offset);
```

libaio��ʹ�ò������ӣ�����Ϊ��libaio�ĳ�ʼ����io������·��ͻ��գ�libaio���١�

### libaio�ĳ�ʼ��������

�۲�libaio�����ҪAPI�����õ�����Ϊio_context�ı������������Ϊlibaio�Ĺ����ռ䡣���þ���ȥ�˽���������Ľṹ��ֻ��Ҫ�˽�����ز���������������libaio�ֱ��õ�io_setup��Ҳ������io_queue_init������ֻ�����ֲ�һ�����ѣ���io_destroy��
```c++
int io_setup(int maxevents, io_context_t *ctxp);
```
����Ҳ��Ҫopen��Ҫ�������ļ���ע������O_DIRECT��־
```c++
int io_destroy(io_context_t ctx);
```

### libaio��д������·��ͻ���

libaio�Ķ�д������io_submit�·����·�ǰͨ��io_prep_pwrite��io_prep_pread����iocb�Ľṹ�壬��Ϊio_submit�Ĳ���������ṹ����ָ���˶�д���͡���ʼ���������Ⱥ��豸��־����

libaio�ĳ�ʼ���������һ�������豸���г�ʼ�����Ǵ���һ��libaio�Ĺ�����������д�����·����ĸ��豸��ͨ��open�����򿪵��豸��־��ָ����

��д�����·�֮��ʹ��io_getevents�����ȴ�io�����ź�
io_getevents����events�����飬�����eventsΪ�����׵�ַ��nrΪ���鳤�ȣ�����󷵻ص�event������min_nrΪ���ٷ��ص�events����timeout����NULL��ʾ�޵ȴ���ʱ��io_event�ṹ�������Ϊ��
```c++
struct io_event {
    PADDEDptr(void *data, __pad1);
    PADDEDptr(struct iocb *obj,  __pad2);
    PADDEDul(res,  __pad3);
    PADDEDul(res2, __pad4);
};
//resΪʵ����ɵ��ֽ�����res2Ϊ��д�ɹ�״̬��0��ʾ�ɹ���
//objΪ֮ǰ�·���struct iocb�ṹ�塣�����б�Ҫ�˽�һ��struct iocb����ṹ�����Ҫ���ݣ�
//iocbp->iocb.u.c.nbytes �ֽ���
//iocbp->iocb.u.c.offset ƫ��
//iocbp->iocb.u.c.buf ����ռ�
//iocbp->iocb.u.c.flags ��д

```
struct iocb�����Դ���Ԫ���⣬�����й��û��Զ����Ԫ�أ������ص�������void *��dataָ�롣����������·�ǰ��io_set_callback���û��Զ���Ļص���������ô���󷵻غ�Ϳ�����ʾ�ĵ��øú������ص�����������Ϊ��
```c++
void callback_function(io_context_t ctx, struct iocb *iocb, long res, long res2);
```
���⣬������ͨ��iocbp->dataָ������û��Լ������ݡ�

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
	//Ԥ�����ڴ�ķ���
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

### libaio ����

file_operations����������AIO��صĳ�Ա������
```c++
ssize_t (*aio_read) (struct kiocb *, const struct iovec *, unsigned long, loff_t);//��ʼ��һ���첽�Ķ�ȡ����   
ssize_t (*aio_write) (struct kiocb *, const struct iovec *, unsigned long, loff_t);//��ʼ��һ���첽��д����� 
int (*aio_fsync) (struct kiocb *, int datasync); //�첽ˢ�´����������   
```
һ����ԣ����豸�������豸��linux���Ĵ�����ʵ���ˣ����ַ��豸һ�㲻��Ҫʵ��AIO֧�֡�