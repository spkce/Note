
```c++

template<typename T>
class Singleton
{
public:
	static T& instance()
	{
		static T inst;
		return inst;
	}
}

```


static变量的唯一性是动态库级别的，不同库包含同一截代码的话就会有多份static实例。
出现问题的情况往往是，这截代码是实现在头文件里的，所以被不同模块引用时就生成了多份代码。

windows
解决办法就是按照一般DLL导出接口的规则，在获取static变量的地方：对于实现单例的dll，使用dllexport。

1. 对于实现单例的dll，使用dllexport。对于MSVC来说就是__declspec(dllexport)，对于GCC/clang就是__attribute__ ((dllexport))
2. 对于引用单例的dll，使用dllimport。对于MSVC/clang来说就是__declspec(dllimport)，对于GCC/clang就是__attribute__ ((dllimport))


linux
编译时加-rdynamic


例如：


```c++
//dy.cpp

#include <stdio.h>
#include "dy.h"

extern "C"
{

void dyFunc()
{
	printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);
	CDy::instance()->dump();
}

} //extern "C"
```

```c++
//dy.h
#ifndef __DY_H__
#define __DY_H__

#include <stdio.h>
class CDy
{
private:
	CDy() {};
	virtual ~CDy() {};
public:
	static CDy* instance()
	{
		static CDy inst;
		return &inst;
	}

	void dump()
	{
		printf("\033[0;35m""this:%p""\033[0m\n", this);
	}

};

extern "C"
{
	void dyFunc();
}
#endif //__DY_H__
```

```c++
//main.cpp
#include <stdio.h>
#include <dlfcn.h>
#include "dy.h"

int main(int argc, char const *argv[])
{
	const char* path = "./libdy.so";

	void *handle = dlopen(path, RTLD_NOW);
	if (!handle)
	{
		printf("\033[0;35m""load %s fail!""\033[0m\n", path);
		return -1;
	}

	dlerror();

	typedef void (*pFun_t)();
	pFun_t pFun = (pFun_t)dlsym(handle, "dyFunc");

	char * err = dlerror();
	if(err)
	{
		printf("\033[0;35m""err: %s!""\033[0m\n", err);
		return -1;
	}

	if (!pFun)
	{
		printf("\033[0;35m""load dyFunc fail!""\033[0m\n");
		return -1;
	}
	pFun();
	CDy::instance()->dump();
	dlclose(handle);
	return 0;
}
```



若不加-rdynamic
```Makefile
CC ?= gcc
CXX ?= g++
AR ?= ar

DYT := libdy.so
TARGET := a.out

all: $(DYT) $(TARGET)

$(DYT) : dy.cpp
	@echo "SO $< -> $@"
	@$(CXX) -shared -fPIC $< -g -o $@ 

$(TARGET) : main.cpp
		@echo "CXX $< -> $@"
		@$(CXX) $< -o $@ -ldl -g
```
执行：
```shell
spkce@pc-spkce:/mnt/d/source/tmp/dyntest$ ./a.out
dy.cpp:19 dyFunc
this:0x7f5ca75ce068
this:0x7f5ca75d3018
```

若加-rdynamic
```Makefile
CC ?= gcc
CXX ?= g++
AR ?= ar

DYT := libdy.so
TARGET := a.out

all: $(DYT) $(TARGET)

$(DYT) : dy.cpp
	@echo "SO $< -> $@"
	@$(CXX) -shared -fPIC $< -g -o $@ 

$(TARGET) : main.cpp
		@echo "CXX $< -> $@"
		@$(CXX) $< -o $@ -ldl -g -rdynamic
```

执行：
```shell
spkce@pc-spkce:/mnt/d/source/tmp/dyntest$ ./a.out 
dy.cpp:19 dyFunc
this:0x7f194413f018
this:0x7f194413f018
```
