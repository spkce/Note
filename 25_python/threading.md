```python
import threading
```

# 线程

### 组合方式
```python
class threading.Thread(group=None, target=None, name=None, args=(), kwargs={}, *, daemon=None)
```
* group：必须为None，与ThreadGroup类相关，一般不使用
* target ： callable 对象
* args : 是固定参数，元组
* kwargs : 是可变参数，字典
* name : 线程名字，默认就是 “Thread-N” 的形式，N 是数字
* daemon：用来设定线程是否随主线程退出而退出。daemon设置为Flase时，子线程不会随主线程退出而退出，主线程会一直等着子线程执行完


例子：
```python
import threading
import time

def test(name, i):
	print('name:',name)
	print('test:',i)


thread = threading.Thread(target=test, args=("obj1",1)
thread.start()

for i in range(5):
    print('main ', i)
    time.sleep(1)

```
### 继承方式：
例子：
```python
class TestThread(threading.Thread):

    def __init__(self,name=None):
        threading.Thread.__init__(self,name=name)

    def run(self):
        for i in range(5):
            print(threading.current_thread().name + ' test ', i)
            time.sleep(1)

thread = TestThread(name='TestThread')
thread.start()
```

run 为执行函数


# 互斥锁
```python
mutex = threading.Lock()   #创建锁
mutex.acquire([blocking]) #锁定
mutex.release()            #释放锁
```
* acquire([blocking]):获取锁定，如果有必要，需要阻塞到锁定释放为止。如果提供blocking参数并将它设置为False，当无法获取锁定时将立即返回False;如果成功获取锁定则返回True。
* release():释放一个锁定。当锁定处于未锁定状态时，或者从与原本调用acquire()方法的不同线程调用此方法,将出现错误。