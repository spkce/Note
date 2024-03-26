
signal_handler 函数中断点可以打入，但是无法命中
```c++
void signal_handler(int signum)
{
	printf("signal %d\n", signum);
}

int main(int argc, char** argv)
{
	signal(SIGINT, signal_handler);
	while (1);
	
	return 0;
}
```

```shell
(gdb) info b
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x00000000080011b8 in signal_handler(int) at a.cpp:19
(gdb) r
Starting program: /mnt/d/source/tmp/a.out

Program received signal SIGINT, Interrupt.
main (argc=1, argv=0x7ffffffee448) at a.cpp:25
```