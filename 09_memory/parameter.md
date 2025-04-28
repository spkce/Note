# 获取进程中某一全局变量的地址
## 1. 通过可执行程序获取符号表

在编译和链接过程中，编译器和链接器会收集程序中的所有符号，并将它们添加到符号表中。对于全局变量，符号表会记录其名称以及在程序加载到内存后对应的虚拟地址。
例如：
某一程序如下：
```c++
#include <stdio.h>
#include <malloc.h>

int g_a = 0;
int g_b;

int main(int argc, char const *argv[])
{
    g_a = 0;
    g_b = 0;

    printf("%d %d\n", g_a, g_b);

    return 0;
}
```
编译完后使用readelf读取 符号表
```
spkce@wsl:~/source/trace$ readelf -s trace.out 
.....
Symbol table '.symtab' contains 38 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
.....
    24: 0000000000004018     4 OBJECT  GLOBAL DEFAULT   26 g_b
    25: 0000000000004000     0 NOTYPE  GLOBAL DEFAULT   25 __data_start
    26: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__
    27: 0000000000004008     0 OBJECT  GLOBAL HIDDEN    25 __dso_handle
    28: 0000000000002000     4 OBJECT  GLOBAL DEFAULT   18 _IO_stdin_used
    29: 0000000000004020     0 NOTYPE  GLOBAL DEFAULT   26 _end
    30: 0000000000001060    38 FUNC    GLOBAL DEFAULT   16 _start
    31: 0000000000004010     0 NOTYPE  GLOBAL DEFAULT   26 __bss_start
    32: 0000000000001149    80 FUNC    GLOBAL DEFAULT   16 main
    33: 0000000000004014     4 OBJECT  GLOBAL DEFAULT   26 g_a
.....
    37: 0000000000001000     0 FUNC    GLOBAL HIDDEN    12 _init
```
可以知道g_a地址为0x4014，g_b地址为0x4018
使用gdb加载程序，但是不开始运行，可以查看g_a和g_b地址为0x4014和0x4018。
```
(gdb) p &g_a
$1 = (int *) 0x4014 <g_a>
(gdb) p &g_b
$2 = (int *) 0x4018 <g_b>
```
但是运行后在查看就会发现地址已经变化。
```
(gdb) p &g_a
$3 = (int *) 0x555555558014 <g_a>
(gdb) p &g_b
$4 = (int *) 0x555555558018 <g_b>
```
操作系统运行程序时先将程序加载到内存中某个地址，全局变量的地址因此变化。

## 2. 获取可执行程序加载地址 
### 2.1 /proc/pid/maps文件系统
```
y0371@DESKTOP-ARV5V41:~$ cat /proc/4190/maps
555555554000-555555555000 r--p 00000000 00:54 281474976738066            /mnt/d/source/trace/trace.out
555555555000-555555556000 r-xp 00001000 00:54 281474976738066            /mnt/d/source/trace/trace.out
555555556000-555555557000 r--p 00002000 00:54 281474976738066            /mnt/d/source/trace/trace.out
555555557000-555555558000 r--p 00002000 00:54 281474976738066            /mnt/d/source/trace/trace.out
555555558000-555555559000 rw-p 00003000 00:54 281474976738066            /mnt/d/source/trace/trace.out
7ffff7da3000-7ffff7da6000 rw-p 00000000 00:00 0
7ffff7da6000-7ffff7dce000 r--p 00000000 08:20 51314                      /usr/lib/x86_64-linux-gnu/libc.so.6
7ffff7dce000-7ffff7f56000 r-xp 00028000 08:20 51314                      /usr/lib/x86_64-linux-gnu/libc.so.6
7ffff7f56000-7ffff7fa5000 r--p 001b0000 08:20 51314                      /usr/lib/x86_64-linux-gnu/libc.so.6
7ffff7fa5000-7ffff7fa9000 r--p 001fe000 08:20 51314                      /usr/lib/x86_64-linux-gnu/libc.so.6
7ffff7fa9000-7ffff7fab000 rw-p 00202000 08:20 51314                      /usr/lib/x86_64-linux-gnu/libc.so.6
7ffff7fab000-7ffff7fb8000 rw-p 00000000 00:00 0
7ffff7fbd000-7ffff7fbf000 rw-p 00000000 00:00 0
7ffff7fbf000-7ffff7fc3000 r--p 00000000 00:00 0                          [vvar]
7ffff7fc3000-7ffff7fc5000 r-xp 00000000 00:00 0                          [vdso]
7ffff7fc5000-7ffff7fc6000 r--p 00000000 08:20 51311                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7ffff7fc6000-7ffff7ff1000 r-xp 00001000 08:20 51311                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7ffff7ff1000-7ffff7ffb000 r--p 0002c000 08:20 51311                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7ffff7ffb000-7ffff7ffd000 r--p 00036000 08:20 51311                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7ffff7ffd000-7ffff7fff000 rw-p 00038000 08:20 51311                      /usr/lib/x86_64-linux-gnu/ld-linux-x86-64.so.2
7ffffffde000-7ffffffff000 rw-p 00000000 00:00 0                          [stack]
```
其中/mnt/d/source/trace/trace.out 程序的映射有5段.
```
555555554000-555555555000 r--p 00000000 00:54 281474976738066            /mnt/d/source/trace/trace.out
```
这一段：r--p 表示该区域只读，不可写和执行，是私有映射（写时复制）。通常映射的是程序的只读数据段（.rodata），包含了程序中定义的常量数据，如字符串常量等。这些数据在程序运行期间不会被修改，因此设置为只读以保证数据的安全性。
```
555555555000-555555556000 r-xp 00001000 00:54 281474976738066            /mnt/d/source/trace/trace.out
```
这一段：r-xp 表示该区域可读可执行，但不可写，是私有映射。此段映射的是程序的代码段（.text），包含了程序的机器指令。程序在运行时会从这个区域读取指令并执行，由于代码在运行过程中不需要修改，所以设置为不可写。
```
555555556000-555555557000 r--p 00002000 00:54 281474976738066            /mnt/d/source/trace/trace.out
```
这可能映射的是一些与程序加载和重定位相关的只读数据，例如重定位表等。这些数据用于帮助操作系统在程序加载时正确地调整代码和数据的地址
```
555555557000-555555558000 r--p 00002000 00:54 281474976738066            /mnt/d/source/trace/trace.out
```
可能映射的是符号表等调试或运行时需要的只读信息，这些信息对于程序的运行和调试有重要作用，但不需要被修改
```
555555558000-555555559000 r--p 00002000 00:54 281474976738066            /mnt/d/source/trace/trace.out
```
这一段映射的是程序的可读写数据段（.data 和 .bss），包含了程序中定义的全局变量和静态变量。这些变量在程序运行期间可以被修改，因此需要可写权限.
从上面可以找出应用程序的映射基地址是0x555555554000，根据g_a 和g_b落在.data和.bss段。
```
(gdb) p &g_a
$3 = (int *) 0x555555558014 <g_a>
(gdb) p &g_b
$4 = (int *) 0x555555558018 <g_b>
```