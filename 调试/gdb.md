格式：x/<n/f/u>  <addr>

n:是正整数，表示需要显示的内存单元的个数，即从当前地址向后显示n个内存单元的内容，一个内存单元的大小由第三个参数u定义。

f:表示addr指向的内存内容的输出格式，s对应输出字符串，此处需特别注意输出整型数据的格式：

x 按十六进制格式显示变量。

d 按十进制格式显示变量。

u 按十六进制格式显示无符号整型。

o 按八进制格式显示变量。

t 按二进制格式显示变量。

a 按十六进制格式显示变量。

c 按字符格式显示变量。

f 按浮点数格式显示变量。

u:就是指以多少个字节作为一个内存单元-unit,默认为4。当然u还可以用被一些字符表示，如b=1 byte, h=2 bytes,w=4 bytes,g=8 bytes.
<addr>:表示内存地址。

整合这个命令的诠释：就是以addr为起始地址，返回n个单元的值，每个单元对应u个字节，输出格式是f。

如：x/ 3uh 0x54320表示：以地址0x54320为起始地址，返回3个单元的值，每个单元有两个字节，输出格式为无符号十六进制。

也就是说返回了3*2=6个字节的数据，以十六进制输出，这6个字节的数据，每两个字节为一个单元输出，共输出3个单元。



bt：显示当前函数调用栈信息，即backtrace。该命令会打印出当前线程的函数调用栈的列表。
info args：显示当前函数的参数列表。
info locals：显示当前函数的局部变量列表。

# 锁定单个线程调试
```
# 查看所有线程
(gdb) info threads
  Id   Target Id         Frame 
  1    LWP 12345 "main"  ...
* 2    LWP 678   "thr1" ...

# 切换到目标线程（LWP=678）
(gdb) thread 2

# 冻结该线程（不影响其他线程运行）
(gdb) interrupt
```

```
# 仅在LWP=678上设置断点
(gdb) break func1 thread 2
```

```
# 恢复特定线程（需非stop模式）
(gdb) continue -a
(gdb) signal SIGCONT --thread 2
```

```
# 仅显示包含"worker"的线程
(gdb) set thread-filter -n worker

# 仅跟踪LWP=678的堆栈
(gdb) thread apply 678 bt
```

#
ptrace(PTRACE_INTERRUPT)


 PTRACE_SEIZE
    与PTRACE_ATTACH不同的是，该请求不会停止tracee的执行

通过 PTRACE_SETOPTIONS 设置事件监听（如 PTRACE_O_TRACECLONE），当进程创建新线程或调用 exec 时自动暂停‌67：

# gdb call命令
call‌ 命令允许在调试过程中‌动态调用程序中的函数‌
以下是他的原理：
```
1. 执行 call 时，GDB 首先暂停被调试进程，保存当前所有寄存器的状态（通过 ptrace 系统调用获取）‌，特别是堆栈指针（SP）和指令指针（IP）。
2. 参数传递
GDB 解析用户输入的参数表达式（如 call func(42, "test")），将其转换为符合目标函数调用约定的格式‌。在 x86-64 架构中，前 6 个整型参数通过 RDI, RSI, RDX, RCX, R8, R9 传递，浮点参数通过 XMM0-XMM7 传递。超出寄存器数量的参数按从右到左顺序压栈。若参数需要动态分配内存（如结构体），GDB 会在目标进程的堆或栈中临时分配空间‌
3. 将目标函数的入口地址写入指令指针寄存器（如 RIP），并调整堆栈指针（SP）以预留返回地址空间‌。某些架构（如 ARM）需手动注入调用指令序列（例如 BL <func_address>）到目标进程内存中，并跳转到该代码块执行‌。
4. 返回值捕获
寄存器读取‌：函数返回值按约定存储到特定寄存器（如 x86-64 的 RAX 或 XMM0）。若返回值为结构体或大对象，GDB 从内存中读取数据（根据 ABI 规则确定内存位置）‌
5. 恢复
根据调用约定（如 cdecl 由调用方清理堆栈），调整 SP 以释放参数占用的空间。：还原调用前的寄存器状态（除用于返回值的寄存器），确保原程序继续执行不受影响‌
```


# eval命令
‌格式化字符串动态生成并执行调试命令‌，特别适用于需要‌批量操作‌或‌动态生成指令‌的调试场景。
```shell
# 在函数 foo、bar、baz 上设置断点
(gdb) eval "break %s", "foo", "bar", "baz"
# 等效于手动输入：
# break foo
# break bar
# break baz
```

# gdb 启动命令 
### --eval-command（-ex）
```shell
# 复杂调试流程自动化
gdb -ex "set env LD_PRELOAD=./my_lib.so" \
    -ex "break malloc" \
    -ex "break free" \
    -ex "run" \
    -ex "bt full" \
    -ex "quit" \
    --args ./memory_leak_demo
```
```shell
# 仅在 SIGSEGV 时打印堆栈
gdb -ex "handle SIGSEGV stop print" \
    -ex "run" \
    -ex "if $_siginfo.si_signo == 11\n bt full\n end" \
    --args ./crash_demo

```
### --command (-x) 
后面跟脚本文件

### --init-command	(-ix)
在 ~/.gdbinit 之前，类似于-x

### --init-eval-command	(-iex)
在 ~/.gdbinit 之前	类似于-ex

# --quiet (-q) 
gdb --quiet 是 GNU 调试器 (GDB) 中用于‌抑制启动信息‌的命令行参数，常用于简化输出或自动化脚本场景

# --batch
执行完所有命令后自动退出，‌抑制非必要输出。不进入 GDB 交互式命令行界面‌。若命令执行成功返回状态码 0，失败返回非零值（便于脚本判断结果）‌


# 手动加载
### sharedlibrary 
强制加载动态库
### set sysroot 或 set solib-absolute-prefix
设置绝对路径前缀
### set solib-search-path
指定 GDB 搜索共享库的路径

### add-symbol-file
手动加载动态库符号
```
(gdb) add-symbol-file libmath_utils.so 0x4000
```
其中 0x4000 为库的加载基地址（需通过 info sharedlibrary 获取）‌