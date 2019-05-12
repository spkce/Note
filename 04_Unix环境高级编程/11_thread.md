# 线程
## 线程的标识
线程使用pthread_t表示线程ID, 线程ID只有在进程上下文中才有意义。_而进程ID在整个系统中都是唯一的_</br>
线程ID不能被当做是整数处理
```
//判断线程ID是否相等: 相等返回!0;不等返回0
int pthread_equal(pthread_t tid1, pthread_t tid2);
//获取自身线程ID
pthread_t pthread_self(void);
```

## 线程的创建
```
//创建线程：成功返回0;否则返回错误码
//attr用于指定线程的属性，为空代表创建默认属性线程
//arg为传给线程执行体start_routine的参数
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
	void *(*start_routine) (void *), void *arg);
```
pthread_create成功返回时第一个参数*thread会返回新线程的ID

## 线程的终止
单个线程可有三种方式退出：
* 直接从线程执行体返回，返回值就是线程的退出码。
* 线程可以被同一进程中的其他线程取消。
* 线程自己调用pthread_exit。

```
 void pthread_exit(void *retval);
```
pthread_exit 的参数retval是返回码，其他线程可以通过pthread_join获取。</br>
retval的内容如果在栈上分配，线程退出后栈被回收，其他线程获取返回码时可能会出错

```
int pthread_cancel(pthread_t thread);
```
同一进程的其他线程可以调用该函数**请求**结束一个线程（非阻塞）</br>
但是被取消的线程可以选择忽略或者控制取消的程序

```
void pthread_cleanup_push(void (*routine)(void *), void *arg);
void pthread_cleanup_pop(int execute);
```
与进程的atexit函数类似，线程可以在栈上建立多个线程清理程序（先记录后执行）

## 线程同步
### 互斥量
互斥量用pthread_mutex_t表示</br>
互斥量在使用前必须初始化，若是静态互斥量可以将它设置为PTHREAD_MUTEX_INITIALIZER，也可以用pthread_mutex_init(); 若是动态互斥量只能使用pthread_mutex_init()，且释放内存前必须调用pthread_mutex_destroy()
```
//初始化互斥量：返回0成功;否则返回错误编号
//若attr为NULL则互斥量为默认属性
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
//销毁互斥量：返回0成功;否则返回错误编号
int pthread_mutex_destroy(pthread_mutex_t *mutex);
```
pthread_mutex_lock会导致阻塞。</br>pthread_mutex_unlock为解锁</br>
pthread_mutex_trylock不阻塞线程，若加锁成功返回0;失败返回EBUSY。</br>
pthread_mutex_timedlock基本相当于pthread_mutex_lock,不同点在于pthread_mutex_timedlock可以设置超时时间,阻塞一段时间后返回ETIMEDOUT
```
//加锁：函数返回0成功;否则返回错误编号
int pthread_mutex_lock(pthread_mutex_t *mutex);
//尝试加锁：函数返回0成功;否则返回错误编号
int pthread_mutex_trylock(pthread_mutex_t *mutex);
//解锁：函数返回0成功;否则返回错误编号
int pthread_mutex_unlock(pthread_mutex_t *mutex);
//加锁:函数返回0成功;否则返回错误编号
int pthread_mutex_timedlock(pthread_mutex_t * mutex,
		const struct timespec * abstime)
```

### 读写锁