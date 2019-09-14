## item 69 建立合理的错误处理策略，并严格遵守
应该在开发的早期便设立实际、一致、合理的错误处理策略，并严格遵守。策略必须遵循以下内容：

* 错误鉴别： 哪些情况属于错误
* 错误严重程度： 每个错误的严重性或紧急性
* 错误检查：记载哪些代码负责检查它（item 70）
* 错误传递：用什么机制在模块中报告和传递错误
* 错误处理： 标明负责处理它的代码（item 74）
* 错误报告： 怎样将错误计入日记或通知用户。

错误处理机制应该只在模块边界改变，每一个模块都应该在内部使用同一种错误处理机制。
如果模块内外所使用的的策略不同，则所有模块的入口函数都要进行错误的转换。

## item 70 区别错误与非错误
**问题**：什么是错误？
错误就是违反约定。


>**名词解释：**
**前置条件**：调用函数之前必须为真的条件，前置条件并不是入参的检查，而是对内部状态的检查，对某一些必须满足的条件的检查。
**后置条件**：函数顺利执行完毕之后必须为真的条件,可以看做式对函数返回值的一个期望
**不变式**：各个数据之前的关系

例子：
class CTime
```C++
//计时器类（伪代码）
class CTime
{
public:
	CTime(void* pOwr)
	:m_pUser(pOwr),m_iSecond(0) {}

	int AddTime(int iSeconds)
	{
		int iRet = (iSeconds > 0);
		m_iSecond += iSeconds;
		return iRet;
	}
private:
	void *m_pUser;
	int m_iSecond;
};

```
1. 构造函数建立了环境供成员函数在其中操作数据。也就是说，构造函数建立了不变式。可以看出m_iSecond的范围值[0, +∞),若iSeconds传入值为负数，则可能破坏不变式，使计时出错。
2. CTime::AddTime需要有对象创建便是前置条件
```C++
assert(m_pUser != NULL);
```
3. CTime::AddTime正确运行后iRet为true, 因此的后置条件为
```C++
assert(iRet != 0);
```

错误有三种类型：</br>
1. 违反或无法满足前置条件。
2. 无法满足后置条件。
3. 无法重新建立不变式。

## item 71 设计和编写错误安全代码
函数的安全性：
1. 基本保证:要么完全成功, 函数到达期望状态, 要么失败，状态处于一个有效但不可预测的状态。
2. 强保证：要么完全成功，函数到达期望状态，要么失败，函数返回调用前的状态
3. 不会失败：保证完全成功，不会失败。

例子：std::vector::insert(),因为vector<T>内部存储空间是联想到，插入一个元素需要将已有值进行移动，即：移动→插入。在插入过程中若是出错，恢复原有的存储内容便是提供强保证，而实际上由于代价高昂，仅仅提供基本保证


错误安全代码设计要求：</br>
* 一般而言，部分函数应该提供不会失败保证，如提供构造、析构、释放等动作的函数。
* 每个函数都应该尽可能的提供强保证，如果难以提供强保证或者代价过于昂贵，那么提供基本保证。但是程序的设计必须要处理基本保证出错时带来的状态。
* 若无法满足基本保证，那么就是Bug

**问题**：PageXXX.cpp关于控件部分的代码是强保证、基本保证还是Bug？</br>
回答：控件在CPageXXX类的构造函数中创建于堆区，但是相应对应的没有在析构函数中没有删除，存在泄漏问题。在整个Challenge生命周期中页面功能页面基本只创建一次，内存会被操作系统回收，故属于基本保证。

## item 72 优先使用异常报告错误

## item 73 通过值抛出，通过引用捕获

在抛出异常的时候，要避免抛出指针。
* 抛出指向堆区的指针→需要额外进行动态内存的管理
* 抛出指向栈区的指针→捕获时栈地址可能无效

若一定要抛出指针，可以考虑智能指针。

不要使用值捕获，否则会有切片问题
```C++
#include "stdio.h"
class IException
{
  public:
	IException() { }
	~IException() {}
};

class CException : public IException
{
  public:
	CException() : IException() { m_num = 1; }
	~CException() {}
	void Dump() { printf("CException::m_num = %d\n", m_num); }
	int m_num;
};

void fun(void)
{
	CException e;
	throw e;
}

int main(int argc, char const *argv[])
{
	try
	{
		fun();
	}
	catch (IException e)
	{
		e.Dump();
	}
	return 0;
}
```
```shell
spice@ubuntu:~/Desktop/hgfs/Source/temp/try_catch$ g++ main.cpp 
main.cpp: In function ‘int main(int, const char**)’:
main.cpp:35:5: error: ‘class IException’ has no member named ‘Dump’
   e.Dump();
```

重新抛出异常e时，应该只写为throw，而不是throw e, throw e会粗暴地去除异常对象的多态。
```C++
#include "stdio.h"
class IException
{
  public:
	IException() { m_count = 0; }
	~IException() {}
	int m_count;
};

class CException : public IException
{
  public:
	CException() : IException() { m_num = 99; }
	~CException() {}
	void Dump() { printf("CException::m_num = %d\n", m_num); }

	int m_num;
};

void fun1(void)
{
	CException e;
	throw e;
}

void fun2(void)
{
	try
	{
		fun1();
	}
	catch (IException &e)
	{
		e.m_count++;
		throw e;
	}
}

int main(int argc, char const *argv[])
{
	try
	{
		fun2();
	}
	catch (CException &e)
	{
		e.m_num++;
		e.Dump();
	}
	return 0;
}
```
运行结果：
```shell
spice@ubuntu:~/Desktop/hgfs/Source/temp/try_catch$ ./a.out 
terminate called after throwing an instance of 'IException'
Aborted (core dumped)
```
```C++
#include "stdio.h"
class IException
{
  public:
	IException() { m_count = 0; }
	~IException() {}
	int m_count;
};

class CException : public IException
{
  public:
	CException() : IException() { m_num = 99; }
	~CException() {}
	void Dump() { printf("CException::m_num = %d\n", m_num); }

	int m_num;
};

void fun1(void)
{
	CException e;
	throw e;
}

void fun2(void)
{
	try
	{
		fun1();
	}
	catch (IException &e)
	{
		e.m_count++;
		throw ; // throw e 修改为 throw
	}
}

int main(int argc, char const *argv[])
{
	try
	{
		fun2();
	}
	catch (CException &e)
	{
		e.m_num++;
		e.Dump();
	}
	return 0;
}
```
运行结果：
```shell
spice@ubuntu:~/Desktop/hgfs/Source/temp/try_catch$ ./a.out 
CException::m_num = 100
```
## item 74 正确地报告、处理和转换错误
只要函数检查出一个它自己无法解决而且函数也无法继续执行的错误，就应该报告错误
```C++
int* fun(void)
{
	int p;
	try
	{
		p = new int[4*1024*1024];
	}
	catch (const std::exception e)
	{
		throw e;
	}
	return p;
}
```

以下情况需要转换错误：
* 要添加高层语意
```C++
int* fun(void)
{
	int p;
	try
	{
		p = new int[4*1024*1024];
	}
	catch (const std::exception e)
	{
		throw "virtual memory exhaustion";
	}
	return p;
}
```
* 要改变错误的处理机制
```C++
#include <errno.h>
int* fun(void)
{
	
	errno = 0;
	int p;
	p = new(std::nothrow) int[4*1024*1024];
	
	if (p == NULL)
	{
		errno = EINVAL;
	}
	return p;
}

```

## item 75 避免使用异常规范

什么是异常规范？
```C++
//异常规范伪代码
int* fun(void) throw(int)//表明只能抛出int型异常
{
	......
}
int* fun(void) throw()//表明不抛出异常
{
	......
}
```
异常规范并不灵活,如果抛出异常规范以外类型的异常对象，程序会被中止。（结果无可挽回的）
```C++
void fun(void) throw(int)
{
	throw "1";
}

int main(int argc, char const *argv[])
{
	try
	{
		fun();
	}
	catch (const char* e)
	{
		printf("%s",e);
	}
	return 0;
}
```
运行结果：
```shell
spice@ubuntu:~/Desktop/hgfs/Source/temp/try_catch$ ./a.out 
terminate called after throwing an instance of 'char const*'
Aborted (core dumped)
```
无法为函数模板编写出有用的异常规范，因为一般无法说清它们所操作的类型可能抛出哪些异常。

异常规范几乎完全无用(出错即终止)的异常规范而付出性能开销

如果基类中使用了异常规范,派生类必须使用相应的异常规范
