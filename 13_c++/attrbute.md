1. __attribute__((Constructor))
在main之前调用函数
```c++
__attribute__((Constructor))
void func()
{
	...
}
```