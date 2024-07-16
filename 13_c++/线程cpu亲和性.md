```c++
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <errno.h>
#include <string.h>

void func(int a, int b)
{
	std::thread::id threadId = std::this_thread::get_id();
	printf("\033[0;35m""threadID = 0x%lx""\033[0m\n", *(std::thread::native_handle_type*)(&threadId));

	int cpu = sched_getcpu();
	printf("\033[0;35m""func current cpu = %d""\033[0m\n", cpu);

	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(1, &mask);

	if (pthread_setaffinity_np(*(std::thread::native_handle_type*)(&threadId),sizeof(cpu_set_t), &mask) != 0)
	{
		printf("\033[0;35m""set affinity fail""\033[0m\n");
		return;
	}

	cpu = sched_getcpu();
	printf("\033[0;35m""func current cpu = %d""\033[0m\n", cpu);
}

int main(int argc, char const *argv[])
{
	int cpu_count = sysconf(_SC_NPROCESSORS_CONF);
	printf("\033[0;35m""cpu_count = %d""\033[0m\n", cpu_count);

	int cpu = sched_getcpu();
	printf("\033[0;35m""main current cpu = %d""\033[0m\n", cpu);

	std::thread th(func, 1, 2);

	//std::thread::id threadId = th.get_id();
	//printf("\033[0;35m""native_handle = 0x%lx""\033[0m\n", th.native_handle());

	// 0 当前进程
	//if (sched_setaffinity(0, sizeof(mask), &mask) == -1) 
	//{
	//	printf("\033[0;35m""setting thread affinity fail:%s""\033[0m\n", strerror(errno));
	//	
	//	return;
	//}
	th.join();
	return 0;
}
```