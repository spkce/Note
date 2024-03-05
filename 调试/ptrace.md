# ptrace函数

```c++
#include <sys/ptrace.h>       
long ptrace(enum __ptrace_request request, pid_t pid, void *addr, void *data);
```

request：要执行的操作类型；
pid：被追踪的目标进程ID；
addr：被监控的目标内存地址；
data：保存读取出或者要写入的数据。


__ptrace_request 选值
```c++
PTRACE_ME
PTRACE_PEEKTEXT
PTRACE_PEEKDATA
PTRACE_PEEKUSER
PTRACE_POKETEXT
PTRACE_POKEDATA
PTRACE_POKEUSER
PTRACE_GETREGS
PTRACE_GETFPREGS,
PTRACE_SETREGS
PTRACE_SETFPREGS
PTRACE_CONT
PTRACE_SYSCALL,
PTRACE_SINGLESTEP
PTRACE_DETACH
```