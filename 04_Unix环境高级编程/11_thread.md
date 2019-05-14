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
	const struct timespec * abstime);
```

### 读写锁
读写锁用pthread_rwlock_t表示</br>
与互斥量相比，读写锁有更高的并发性。互斥量只有加锁和不加锁的状态，且同时只有一个线程可以加锁。读写锁有三种状态：写锁、读锁、无锁。</br>
写锁状态时, 任何一个试图加锁的线程都会进入阻塞; 而在读锁状态时，所有读模式的线程都可以得到访问权，任何写模式的线程都会被阻塞。</br>
若读锁状态下出现写加锁，后面的所有读锁会被阻塞, 避免读模式长期占用锁，而写模式一直得不到满足。</br>
静态读写锁初始化可以用PTHREAD_RWLOCK_INITIALIZER</br>
动态读写锁初始化必须用pthread_rwlock_init,在释放内存空间之前必须pthread_rwlock_destroy
```
//初始化读写锁:函数返回0成功;否则返回错误编号
int pthread_rwlock_init(pthread_rwlock_t *rwlock,
	const pthread_rwlockattr_t *attr);
//注销读写锁:函数返回0成功;否则返回错误编号
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
```
attr为NULL时，表示读写锁初始化为默认属性

```
//加读锁:函数返回0成功;否则返回错误编号
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
//尝试加读锁:函数返回0成功;否则返回错误编号
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
//加读锁(带超时):函数返回0成功;否则返回错误编号
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock,
	const struct timespec *abstime);
//加写锁:函数返回0成功;否则返回错误编号
int pthread_rwlock_wrlock(pthread_rwlock_t *__rwlock);
//尝试写锁:函数返回0成功;否则返回错误编号
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
//加写锁(带超时):函数返回0成功;否则返回错误编号
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock,
	const struct timespec *abstime);
//解锁
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
```

### 条件变量
条件变量给多线程提供一种会和的场所，条件本身由互斥量保护的，线程在改变条件状态之前必须先锁住互斥量，其他线程在获取互斥量之前不会察觉到这种改变。</br>
条件变量由pthread_cond_t表示。</br>静态条件变量可以用常量PTHREAD_COND_INITIALIZER初始化</br>
动态条件变量必须在释放内存前调用pthread_cond_destroy

```
//初始化条件变量:函数返回0成功;否则返回错误编号
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
//注销条件变量:函数返回0成功;否则返回错误编号
int pthread_cond_destroy(pthread_cond_t *__cond);
```
attr为NULL时，表示条件变量初始化为默认属性

```
//等待条件变量为真：函数返回0成功;否则返回错误编号
int pthread_cond_wait(pthread_cond_t *cond,pthread_mutex_t *mutex)
//等待条件变量为真(带超时)：函数返回0成功;否则返回错误编号
int pthread_cond_timedwait(pthread_cond_t *cond,
	pthread_mutex_t *mutex,const struct timespec *abstime)
```
pthread_cond_wait参数中传入的互斥量对条件变量进行保护，在调用时候把<font color=#fff0000 size=3>锁住的互斥量</font>传给函数, 函数会自动把线程放到等待条件的线程列表上，<font color=#fff0000 size=3>对互斥量解锁</font>。pthread_cond_wait返回时<font color=#fff0000 size=3>互斥量再次被锁上</font>
```
//通知线程条件变量为真：函数返回0成功;否则返回错误编号
int pthread_cond_signal(pthread_cond_t *cond);
//通知线程条件变量为真：函数返回0成功;否则返回错误编号
int pthread_cond_broadcast(pthread_cond_t *cond)
```
pthread_cond_signal函数至少能唤醒一个等待该条件的线程，pthread_cond_broadcast唤醒所有等待该条件的线程</br>
<font color=#fff0000 size=3>注意，一定要改变条件状态以后再给线程发信号</font>

### 自旋锁
自旋锁和互斥量类似，互斥量在等待锁时是处于休眠状态的，而自旋锁一直处于忙等待状态。因此较之互斥量在线程上花费较少，而较为耗费CPU资源。适用于锁被持有时间较短的情况。</br>
*在应用层中自旋锁的用处较少，在底层较多*</br>
自旋锁用pthread_spinlock_t表示
```
//初始化自旋锁:函数返回0成功;否则返回错误编号
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
//注销自旋锁:函数返回0成功;否则返回错误编号
int pthread_spin_destroy(pthread_spinlock_t *lock);
```
自旋锁只有一种属性，只在支持线程进程共享同步的平台上用得到。pshared表示进程共享属性，表明自旋锁如何获取的，若设置为PTHREAD_PROCESS_SHARED,则自旋锁能被可以访问锁底层内存的线程获取;否则pshared设置为PTHREAD_PROCESS_PRIVATE,自旋锁只能被初始化该锁的线程访问
```
//获取自旋锁:函数返回0成功;否则返回错误编号
int pthread_spin_lock(pthread_spinlock_t *lock)
//尝试自旋锁:函数返回0成功;否则返回错误编号
int pthread_spin_trylock(pthread_spinlock_t *lock)
//解锁:函数返回0成功;否则返回错误编号
int pthread_spin_unlock(pthread_spinlock_t *lock)
```

### 屏障
屏障是用户协调多个线程并行工作的同步机制。屏障允许每个线程等待，直达所有的合作线程都到达某一个点，然后从该点执行。prthread_join()就是一种特殊屏障：允许一个线程等待直到另一个线程退出。</br>
屏障用pthread_barrier_t表示
```
//初始化屏障:函数返回0成功;否则返回错误编号
int pthread_barrier_init(pthread_barrier_t *barrier,
	const pthread_barrierattr_t *attr, unsigned int count)
//注销屏障:函数返回0成功;否则返回错误编号
int pthread_barrier_destroy(pthread_barrier_t *barrier)
```
初始化屏障时可以使用count参数指定，在允许所有程序继续允许之前，必须到达屏障的线程数。attr指定线程屏障的属性，若为NULL则默认属性
```
//等待其他线程到达：成功返回0或PTHREAD_BARRIER_SERIAL_THREAD;失败返回错误编号
int pthread_barrier_wait(pthread_barrier_t *__barrier)
```
到达pthread_barrier_wait的线程在其他线程没有到达时会进入休眠</br>
对于一个任意线程，pthread_barrier_wait函数返回PTHREAD_BARRIER_SERIAL_THREAD。剩下的线程看到的返回值是0。这使得一个线程可以作为主线程，它可以工作在其他所有线程已完成的工作结果上