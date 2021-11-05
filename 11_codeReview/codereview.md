## 1. 内存的问题
## 1.1 strncpy和memcpy使用选择
strncpy根据指定长度的字符串内容拷贝，当遇到0时则结束拷贝

memcpy根据指定长度的字符串内容拷贝

strncpy的正确用法：
```c++
const char* src = "123456789";
char dst[5] = {0};
strncpy(dst, src, sizeof(dst) - 1);
```
若 dstlen 大于 srclen时，拷贝动作会在字符串src的'\0'时会终止。
若 dstlen 小于 srclen时，拷贝动作会在完成指定长度拷贝时终止

若改用memcpy。

若 dstlen 大于 srclen时，拷贝动作不会在字符串src的'\0'时会终止，内存访问越界。

所以使用memcpy需要判断src和dst两块内存的长度：


memcpy的正确用法：
```c++
const char* src = "123456789";
char dst[5] = {0};
memcpy(dst, src, Min(sizeof(dst), sizeof("123456789")) - 1);
```

原则：
1. 字符串拷贝用strncpy
2. 除非两块内存大小相等，否则不使用memcpy

### 1.2 末尾填0问题

```c++
const char* src = "123456789";
char dst[10];
memset(dst, 'a', MAX);

strncpy(dst, src, sizeof(dst) - 1);
```
最后一个位置字符没有0，有读越界的风险

故遇到dst脏内存的情况下。需要末尾填0。或者在拷贝前初始化内存

### 1.3 数组名的退化

组名作为参数传递给函数时，数组名会退化为指针

```c++
void fun(char *a, int n)
{
	printf("sizeof(a) = %d", sizeof(a);); // sizeof(a):4
}

int main()
{
	char a[] = {"1234"};
	const char* p = "123456789"
	printf("sizeof(a) = %d", sizeof(a);); // sizeof(a):5
	printf("sizeof(p) = %d", sizeof(p);); // sizeof(p):4
	fun(a, sizeof(a));
	return 0;
}
```

由于数组名在入参时退化,一般而言在传入参数时都会将数组长度同时传入(`void fun(char *a, int n)`)

### 1.4 内存使用建议
1. 避免在栈上申请大块内存
2. 避免频繁申请、释放堆区内存
3. 大量且频繁申请、释放的内存建议使用内存池


## 2 函数入参传值问题

### 2.1 传值用指针
```c++
struct SA 
{
	int a[100];
}
void fun(SA a);
```
SA a作为形参时会发生拷贝

非内置类型变量传参，建议使用指针 `void fun(SA *pA)`。

### 2.2 参数不超过4个
 如果一个函数的参数超过4个，就该考虑该函数是否编写得当。特别是嵌入式软件开发中，APCS规定的寄存器使用，R0~R3用来传递函数的参数。当参数个数超过4个时需要借助堆栈来传递参数。
 
如果不得不使用很多参数，可以定义一个结构体来容纳这些参数。

### 2.3 最小权限原则
最小权限原则：计算环境中的特定抽象层的每个模块如进程、用户或者计算机程序只能访问当下所必需的信息或者资源。


1. 具体到函数的传值，若函数内部对入参内容无修改，指针最好加上const修饰
2. 对于c++ 成员函数，若对成员变量无修改，函数加上const修饰
3. 减少全局变量使用，函数的权限尽量小（对于C函数，能用static修饰的一定使用static；对于c++成员函数，能用private权限的尽量用private）
4. 尽量压后变量的定义

> <<effective c++>> Item 26 尽量推迟变量的定义：变量在函数的内部定义，意味着要付出构造与析构的代价。而假如定义了却没有使用过，意味着我们要浪费这些成本

除此之外：推迟变量的定义也是将变量的上下文环境压缩到最小。


### 2.4 const与指针
常量指针:指针指向的值不能修改
```c++
const char* p = "ssss";
p[1] = '1';//错误
p = NULL; //正确
```

指针常量: 指针变量的值不能修改
```c++
char * const p = "ssss";
p[1] = '1';	//正确
p=NULL;		//错误
```
常指针常量: 指针变量的值、指针指向的值不能修改
```c++
const char * const p = "ssss";
```

### 2.5 指针出参问题

错误的用法：
```c++
void func(int * p)
{
	static int a = 1;
	p = &a;
}

int main()
{
	int* p = NULL;
	func(p); 
}
```

正确的用法：
```c++
void func(int ** p)
{
	static int a = 1;
	*p = &a;
}

int main()
{
	int* p = NULL;
	func(&p); 
}
```

## 3. 浮点数不可使用 == 、!=
错误
```c++
void fun(float a)
{
	if (a == 0)
}
```

正确

```c++
void fun(float a)
{
	if (a < 0.00001 && a > -0.00001)
	{
		....
	}
}
```

## 4. i++和++i

++i 是先加后赋值；i++ 是先赋值后加
```c++
int a = 1;
int b = 1;
printf("a:%d", a++); // a:1
printf("b:%d", ++b); // b:2
```
## 5. 判断
### 5.1 判断分支的预测问题

```c++
if (...) 
{

} else {

}
```
CPU（arm）分支预测器机制：由于CPU乱序执行指令，对于if条件语句，CPU默认预先加载if 分支内指令到cache并执行，若条件为false，加载else分支指令执行。内核中采用 LIKELY和UNLIKELY宏帮助预测。而应用代码中建议大概率执行的代码放到if分支，小概率执行的代码放到else分支。

### 5.2 if 和 switch

多个条件的情况下 if-else语句会执行多次判断，直到条件满足。

switch根据case 生成一个跳表，根据条件跳转执行，多个条件一次判断。

在多条件分支情况下if-else语句更加耗时。但是switch的跳表耗内存，分支过多有栈溢出风险。

## 7. 非ascii码字符使用
非ascii码字符建议使用com_str
在chinese.json中添加中文
```json
"strKey.ledVersion": "LED屏版本",
```
代码中使用：
```c++
printf("%s", com_str("strKey.ledVersion")); //打印结果：LED屏版本
```
若chinese.json中无添加的中文则打印出key
```c++
printf("%s", com_str("strKey.ledVersion1")); //打印结果：strKey.ledVersion1
```

## 7. 符号
### 7.11. 无符号类型和有符号类型比较
```c++
int a;
unisgned int b;

if (a > b)
....
```

### 7.12. 无符号类型和0比较
```c++
unisgned int a;

if (a >= 0)
....
```

### 7.13 有符号参数作为下标访问数组
```c++
int a = -1;
char b[] = {"ssssss"};

b[a] = 0;
....
```
## 8 宏问题

### 8.1 宏函数与内联函数

宏函数在预编译阶段展开，省去了函数压栈退栈过程。没有类型检查

内联函数（inline）在编译阶段展开，可以和宏函数一样省去压栈退栈过程但是提供类型检查。若内联函数过于复杂，编译器会自动放弃内联。且部分编译器会对于简单的普通函数自动内联。可以理解函数添加inline修饰仅仅是给编译器一个建议，具体是否内联由编译器做最终决定。

对于简单的函数，建议使用内联而不是宏函数

### 8.2 宏定义与常量

C++ 语言可以用const 来定义常量，也可以用#define 来定义常量。但是前者比后者有更多的优点：
1. const 常量有数据类型，而宏常量没有数据类型。编译器可以对前者进行类型安全检查。而对后者只进行字符替换，没有类型安全检查，并且在字符替换可能会产生意料不到的错误（边际效应）。
2. 有些集成化的调试工具可以对const 常量进行调试，但是不能对宏常量进行调试。

> 《高质量C++/C编程指南》 规则5-2-1：在C++ 程序中只使用const 常量而不使用宏常量，即const 常量完全取代宏常量。


## 9 循环
在多重循环中，将最长的循环放在最内层，最短的循环放在最外层，以减少CPU切入循环层的次数

## 10 内存对齐
### 10.1 内存大小
内存默认使用4字节对齐
```c++
//32位系统
#include <stdio.h>
struct
{
    int i;    
    char c1;  
    char c2;  
}x1;

struct{
    char c1;  
    int i;    
    char c2;  
}x2;

int main()
{
    printf("%d\n",sizeof(x1));  // 8
    printf("%d\n",sizeof(x2));  // 12
    return 0;
}
```

结构体定义应该遵循先大后小，这样拥有最紧凑的内存。

### 10.2 内存对齐方式

```c++
#pragme pack(n) //n 可以为 1 2 4 8 16
.......
#pragma pack()
```

## 11 位域和bitset

c可以使用位域直接对bit读写
```c++
struct packed_struct {
  unsigned int f1:1;
  unsigned int f2:1;
  unsigned int f3:1;
  unsigned int f4:1;
  unsigned int type:4;
  unsigned int my_int:9;
} pack;
```
而c++ 提供位操作std::bitset；

对于需要将bit位含义写在注释中，采用位域和std::bitset不失为一种选择


## . 常用API
代码中可以使用以下函数的，建议优先使用而不是造轮子
```c++
// snprintf逆运算
int sscanf(const char *str, const char *format, ...);

//主机序转网络字节序（小端转大端）
uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);

//网络字节转主机序（大端转小端）
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);

//字符串转化为数据
int atoi(const char *str)

//字符串比较
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);

//字符串拼接
char *strncat(char *dest, const char *src, size_t n);

//字符串查找
char *strstr(const char *haystack, const char *needle);
```
字符串处理推荐使用std::string，功能较为强大