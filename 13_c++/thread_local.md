# thread_local
thread_local 是 C++11 为线程安全引进的变量声明符。表示对象的生命周期属于线程存储期。

线程局部存储(Thread Local Storage，TLS)是一种存储期(storage duration)，对象的存储是在线程开始时分配，线程结束时回收，每个线程有该对象自己的实例；如果类的成员函数内定义了 thread_local 变量，则对于同一个线程内的该类的多个对象都会共享一个变量实例，并且只会在第一次执行这个成员函数时初始化这个变量实例

thread_local 也可以用于局部变量的声明，其作用域的约束与局部静态变量类似，但是其存储与局部静态变量不一样，首先是每个线程都有自己的变量实例，其次是其生命周期与线程一致

thread_local 可以用于类的成员变量，但是只能用于静态成员变量。这很容易理解，C++ 不能在对象只有一份拷贝的情况下弄出多个成员变量的实例，但是静态成员就不一样了，每个类的静态成员共享一个实例，改成线程局部存储比较容易实现，也容易理解。

```c++
thread_local int thread_count = 1;
```
# 替代方案
只是想在线程之间传递数据，可以考虑使用线程特定的数据（Thread-Specific Data, TSD）机制，如 POSIX 的 pthread_key_create 和 pthread_setspecific 函