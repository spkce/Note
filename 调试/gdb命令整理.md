
<font color=#fff0000 size=3>2月20日</font>
## 1. break 变量
普通断点
```
(gdb) b main
```
条件断点
```
(gdb) break 断点 if 条件
```
列出所有断点
```
(gdb) info b
```
增加断点条件
```
(gdb) condition 断点编号 条件
```
删除断点, clear 可以多个方式删除，但是delete只能删除断点的编号
```
(gdb) clear 
(gdb) clear 函数名
(gdb) clear 行号
(gdb) clear 文件名：行号
(gdb) clear 文件名：函数名
(gdb) d 断点编号
```
断点命令
```
(gdb) commands 断点编号
	命令
	...
	end
```
断点的禁用、启用
```
disable 断点编号
enable 断点编号
```
例子：</br>
测试用程序
```C++
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int a = 0;
int b = 10;
int main(int argc, char* argv[])
{
	for (int i = 0; i < 10; i++)
	{
		if (a == 5)
		{
			a = b / 2;
		}
		a++;
		sleep(1);	

	}
}
```
普通断点
```
(gdb) b main
Breakpoint 1 at 0x400535: file gdb.cpp, line 9.
(gdb) r
Starting program: /mnt/hgfs/Source/temp/gdb/a.out 

Breakpoint 1, main (argc=1, argv=0x7fffffffdef8) at gdb.cpp:9
9		for (int i = 0; i < 10; i++)
```
条件断点
```
(gdb) b gdb.cpp:16 if a == 3
Breakpoint 1 at 0x400571: file gdb.cpp, line 16.
(gdb) r
Starting program: /mnt/hgfs/Source/temp/gdb/a.out 

Breakpoint 1, main (argc=1, argv=0x7fffffffdef8) at gdb.cpp:16
16			sleep(1);	
(gdb) p a
$1 = 3
```
列出所有断点
```
(gdb) b main
Breakpoint 1 at 0x400535: file gdb.cpp, line 9.
(gdb) info b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000000000400535 in main(int, char**) 
                                                   at gdb.cpp:9
```
增加断点条件
```
(gdb) b gdb.cpp:16
Breakpoint 1 at 0x400571: file gdb.cpp, line 16.
(gdb) r
Starting program: /mnt/hgfs/Source/temp/gdb/a.out 

Breakpoint 1, main (argc=1, argv=0x7fffffffdef8) at gdb.cpp:16
16			sleep(1);	
(gdb) p a
$1 = 1
(gdb) info b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000000000400571 in main(int, char**) 
                                                   at gdb.cpp:16
	breakpoint already hit 1 time
(gdb) cond 1 a > 3
(gdb) c
Continuing.

Breakpoint 1, main (argc=1, argv=0x7fffffffdef8) at gdb.cpp:16
16			sleep(1);	
(gdb) p a
$2 = 4
```
```
(gdb) b main
Breakpoint 1 at 0x400535: file gdb.cpp, line 9.
(gdb) info b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000000000400535 in main(int, char**) 
                                                   at gdb.cpp:9
(gdb) d 1
(gdb) info b
No breakpoints or watchpoints.

```
```
(gdb) b main
Breakpoint 1 at 0x400535: file gdb.cpp, line 9.
(gdb) info b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000000000400535 in main(int, char**) 
                                                   at gdb.cpp:9
(gdb) commands 1
Type commands for breakpoint(s) 1, one per line.
End with a line saying just "end".
>p b
>end
(gdb) r
Starting program: /mnt/hgfs/Source/temp/gdb/a.out 

Breakpoint 1, main (argc=1, argv=0x7fffffffdef8) at gdb.cpp:9
9		for (int i = 0; i < 10; i++)
$1 = 10
```
断点的禁用、启用
```
(gdb) b main
Breakpoint 1 at 0x400535: file gdb.cpp, line 9.
(gdb) disable 1
(gdb) r
Starting program: /mnt/hgfs/Source/temp/gdb/a.out 
[Inferior 1 (process 3117) exited normally]
```

## 2. watch 变量
watch设置一个观察点,一旦var变化则中断程序
```gdb
(gdb) watch var
```
rwatch设置一个观察点，一旦被读取则中断程序
```gdb
(gdb) rwatch var
```
awatch设置一个观察点，一旦被读或写则中断程序
```gdb
(gdb) awatch var
```
设置观察点条件：var变量大于value
```gdb
(gdb) cond <watchpoint_number> var>=value
```
info watchpoints列出所有观察点
```gdb
(gdb) info watchpoints
```
例子：</br>
测试用程序
```C++
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int a = 0;
int b = 10;
int main(int argc, char* argv[])
{
	for (int i = 0; i < 10; i++)
	{
		if (a == 5)
		{
			a = b / 2;
		}
		a++;
		sleep(1);	

	}
}
```
watch命令, 在a改变时候中断
```
(gdb) watch a
Hardware watchpoint 1: a
(gdb) r
Starting program: /mnt/hgfs/Source/temp/gdb/a.out 

Hardware watchpoint 1: a

(gdb) r
Starting program: /mnt/hgfs/Source/temp/gdb/a.out 

Hardware watchpoint 1: a

Old value = 0
New value = 1
main (argc=1, argv=0x7fffffffdef8) at gdb.cpp:16
16			sleep(1);	
```
rwatch命令,在b被读取时候中断
```
(gdb) rwatch b
Hardware read watchpoint 1: b
(gdb) r
Starting program: /mnt/hgfs/Source/temp/gdb/a.out 

Hardware read watchpoint 1: b

Value = 10
0x0000000000400553 in main (argc=1, argv=0x7fffffffdef8) at gdb.cpp:13
13				a = b / 2;
```
awatch命令，a被读取和写入时候被中断
```
(gdb) awatch a
Hardware access (read/write) watchpoint 1: a
(gdb) r
Starting program: /mnt/hgfs/Source/temp/gdb/a.out 

Hardware access (read/write) watchpoint 1: a

Value = 0
0x0000000000400548 in main (argc=1, argv=0x7fffffffdef8) at gdb.cpp:11
11			if (a == 5)
(gdb) c
Continuing.

Hardware access (read/write) watchpoint 1: a

Value = 0
0x0000000000400568 in main (argc=1, argv=0x7fffffffdef8) at gdb.cpp:15
15			a++;
(gdb) 
```
info watchpoints命令
```
(gdb)  rwatch b
Hardware read watchpoint 1: b
(gdb)  watch a
Hardware watchpoint 2: a
(gdb) awatch i
No symbol "i" in current context.
(gdb) awatch a
Hardware access (read/write) watchpoint 3: a
(gdb) info watchpoints
Num     Type            Disp Enb Address    What
1       read watchpoint keep y              b
2       hw watchpoint   keep y              a
3       acc watchpoint  keep y              a
```
watch点和断点的删除方法相同
```
(gdb) info watchpoints
Num     Type            Disp Enb Address    What
1       read watchpoint keep y              b
2       hw watchpoint   keep y              a
3       acc watchpoint  keep y              a
(gdb) d 1
(gdb) info watchpoints
Num     Type           Disp Enb Address    What
2       hw watchpoint  keep y              a
3       acc watchpoint keep y              a

```

<font color=#fff0000 size=3>2月21日</font>
## 3. attch命令
查看设备运行信息
```
(gdb) info proc
```
用以下命令可以attach,在实际的测试中，可能需要注意权限问题
```
gdb <program> pid
```
```
gdb <program>
(gdb) attach pid
```
```
(gdb) detach
```
例子：

```C++
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int a = 0;
int b = 10;
int main(int argc, char* argv[])
{
	for (;;)
	{
		if (a == 5)
		{
			a = b / 2;
		}
		a++;
		sleep(1);	

	}
}
```
```
(gdb) info proc
process 4769
cmdline = './a.out'
cwd = '/mnt/hgfs/Source/temp/gdb'
exe = '/mnt/hgfs/Source/temp/gdb/a.out'
```

```
spice@ubuntu:~/Desktop/hgfs/Source/temp/gdb$ g++ -g gdb.cpp 
spice@ubuntu:~/Desktop/hgfs/Source/temp/gdb$ ./a.out 


spice@ubuntu:~/Desktop/hgfs/Source/temp/gdb$ sudo gdb a.out
(gdb) shell ps -aux
...
spice      4769  0.0  0.0   4220   712 pts/6    t+   12:37   0:00 ./a.out
...
(gdb) attach 4769
```
```
(gdb) detach
Detaching from program: /mnt/hgfs/Source/temp/gdb/a.out, process 4769
```


# 单步
### 1. next
next 命令（简写为 n）是让 GDB 调到下一条命令去执行，这里的下一条命令不一定是代码的下一行，而是根据程序逻辑跳转到相应的位置

next 命令调试遇到函数调用直接跳过，不进入函数体内部。

### 2. step模式：
step 命令（简写为 s）就是“单步步入”（step into），顾名思义，就是遇到函数调用，进入函数内部。
就是在“step”模式下执行“step”下一指令，可以保证当前的线程不会被抢占，因为其它线程不会执行。而除了“step”以外的指令，如“continue”、“until”和“finish”会导致其它线程重新开始执行。
```
(gdb) show scheduler-locking
```

### 3. finish 和 return
实际调试时，我们在某个函数中调试一段时间后，不需要再一步步执行到函数返回处，希望直接执行完当前函数并回到上一层调用处，就可以使用 finish 命令。与 finish 命令类似的还有 return 命令，return 命令的作用是结束执行当前函数，还可以指定该函数的返回值。

这里需要注意一下二者的区别：

* finish 命令会执行函数到正常退出该函数；
* return 命令是立即结束执行当前函数并返回，也就是说，如果当前函数还有剩余的代码未执行完毕，也不会执行了；

### 4. until
实际调试时，还有一个 until 命令（简写为 u）可以指定程序运行到某一行停下来，想直接跳到第 1839 行，可以直接输入 u 1839

### 5. jump
jump 会让程序执行流跳转到指定位置执行，当然其行为也是不可控制的，例如您跳过了某个对象的初始化代码，直接执行操作该对象的代码，那么可能会导致程序崩溃或其他意外行为。jump 命令可以简写成 j，但是不可以简写成 jmp，其使用有一个注意事项，即如果 jump 跳转到的位置后续没有断点，那么 GDB 会执行完跳转处的代码会继续执行。举个例子：

# 线程调度锁定
set scheduler-locking 命令的语法格式如下：

```
(gdb) set scheduler-locking mode
```
其中，参数 mode 的值有 3 个，分别为 off、on 和 step，它们的含义分别是：

off：不锁定线程，任何线程都可以随时执行；

on：锁定线程，只有当前线程或指定线程可以运行；

step：当单步执行某一线程时，其它线程不会执行，同时保证在调试过程中当前线程不会发生改变。但如果该模式下执行 continue、until、finish 命令，则其它线程也会执行，并且如果某一线程执行过程遇到断点，则 GDB 调试器会将该线程作为当前线程。

查看状态：

```
(gdb) show scheduler-locking
```
# 设置动态库的搜索路径
set solib-search-path设置动态库的搜索路径


# layout
layout：用于分割窗口，可以一边查看代码，一边测试。主要有以下几种用法：
layout src：显示源代码窗口
layout asm：显示汇编窗口
layout regs：显示源代码/汇编和寄存器窗口
layout split：显示源代码和汇编窗口

Ctrl + L：刷新窗口
Ctrl + x，再按1：单窗口模式，显示一个窗口
Ctrl + x，再按2：双窗口模式，显示两个窗口
Ctrl + x，再按a：回到传统模式，即退出layout，回到执行layout之前的调试窗口。