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
PTRACE_TRACEME	//本进程被其父进程所跟踪。其父进程应该希望跟踪子进程
PTRACE_PEEKTEXT	//从内存地址中读取一个字节，内存地址由addr给出
PTRACE_PEEKDATA	//同上
PTRACE_PEEKUSER	//可以检查用户态内存区域(USER area),从USER区域中读取一个字节，偏移量为addr
PTRACE_POKETEXT	//往内存地址中写入一个字节。内存地址由addr给出
PTRACE_POKEDATA	//往内存地址中写入一个字节。内存地址由addr给出
PTRACE_POKEUSER	//往USER区域中写入一个字节，偏移量为addr
PTRACE_GETREGS	//读取寄存器
PTRACE_GETFPREGS	//读取浮点寄存器
PTRACE_SETREGS	//设置寄存器
PTRACE_SETFPREGS	//设置浮点寄存器
PTRACE_CONT	//重新运行
PTRACE_SYSCALL	//
PTRACE_SINGLESTEP	//设置单步执行标志
PTRACE_ATTACH	//追踪指定pid的进程
PTRACE_DETACH	//结束追踪
```

PTRACE_PEEKTEXT和PTRACE_PEEKDATA是用于在被跟踪进程的内存中读取数据的ptrace系统调用的请求选项。在Linux中，由于没有单独的文本和数据地址空间，因此这两个请求目前是等效的


## 单步
```c++
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

int main()
{
	pid_t child = fork();
	if (child == 0)
	{
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execl("/home/spkce/source/tmp/a.out", "/home/spkce/source/tmp/a.out", NULL);
	}
	else
	{
		int status;
		struct user_regs_struct regs;
		while (1)
		{
			wait(&status);
			if (!WIFSTOPPED(status))
			{
				break;
			}
			ptrace(PTRACE_GETREGS, child, NULL, &regs);
			printf("EIP: 0x%llx RSI:0x%llx RDI::0x%llx\n", regs.rip, regs.rsi, regs.rdi);
			ptrace(PTRACE_SINGLESTEP, child, NULL, NULL);
		}
	}
	return 0;
}
```