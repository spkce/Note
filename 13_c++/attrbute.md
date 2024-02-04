1. __attribute__((constructor))
在main之前调用函数
```c++
__attribute__((constructor))
void func()
{
	...
}
```

2. __cxa_finalize

主函数退出后调用的函数。他会调用

__attribute__((destructor))修饰的函数

