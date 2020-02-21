
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