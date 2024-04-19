# /proc

### statm

```shell
spkce@pc-spkce:~/source/tmp$ cat /proc/1/statm
2238 52 41 113 0 2080 0
```
很简单地返回7组数字，每一个的单位都是一页 （常见的是4KB）
分别是
size:任务虚拟地址空间大小
Resident：正在使用的物理内存大小
Shared：共享页数
Trs：程序所拥有的可执行虚拟内存大小
Lrs：被映像倒任务的虚拟内存空间的库的大小
Drs：程序数据段和用户态的栈的大小
dt：脏页数量

### maps
maps 文件可以查看某个进程的代码段、栈区、堆区、动态库、内核区对应的虚拟地址

```shell
spkce@pc-spkce:~/source/tmp$ cat /proc/1/maps
00200000-00225000 r--p 00000000 00:00 1779                       /init
00225000-00226000 r--p 00025000 00:00 1779                       /init
00226000-00296000 r-xp 00026000 00:00 1779                       /init
00296000-00297000 r-xp 00096000 00:00 1779                       /init
00297000-0029e000 rw-p 00097000 00:00 1779                       /init
0029e000-002a4000 rw-p 00000000 00:00 0
01700000-01703000 rw-p 00000000 00:00 0                          [heap]
7f57ace40000-7f57ace41000 ---p 00000000 00:00 0
7f57ace41000-7f57ace56000 rw-p 00000000 00:00 0
7ffff9834000-7ffffa034000 rw-p 00000000 00:00 0                  [stack]
7ffffa6aa000-7ffffa6ab000 r-xp 00000000 00:00 0                  [vdso]
```
第一列代表内存段的虚拟地址
第二列代表执行权限，r，w，x不必说，p=私有 s=共享。
第三列代表在进程地址里的偏移量
第四列映射文件的主设备号和次设备号，
第五列映像文件的节点号，即inode
第六列是映像文件的路径


有时候可以通过不断查看某个进程的maps文件，通过查看其虚拟内存（堆区）是否不停增长来简单判断进程是否发生了内存溢出

### status

```shell
spkce@pc-spkce:~/source/tmp$ cat /proc/self/status
Name:   cat										;可执行程序的名字
State:  R (running) 								;任务状态，运行/睡眠/僵死
Tgid:   1513 								;线程组号
Pid:    1513 								;进程id
PPid:   105									;父进程id
TracerPid:      0
Uid:    1000    1000    1000    1000
Gid:    1000    1000    1000    1000
FDSize: 4									;该进程最大文件描述符个数
Groups:
VmPeak: 0 kB								;内存使用峰值
VmSize: 15412 kB							;进程虚拟地址空间大小
VmLck:  0 kB								;进程锁住的物理内存大小，锁住的物理内存无法交换到硬盘
VmHWM:  0 kB							
VmRSS:  928 kB								;进程正在使用的物理内存大小
VmData: 0 kB								;进程数据段大小
VmStk:  0 kB								;进程用户态栈大小
VmExe:  32 kB								;进程代码段大小
VmLib:  0 kB							
VmPTE:  0 kB								;进程页表大小
Threads:        1
SigQ:   0/0
SigPnd: 0000000000000000
ShdPnd: 0000000000000000
SigBlk: 0000000000000000
SigIgn: 0000000000000000
SigCgt: 0000000000000000
CapInh: 0000000000000000
CapPrm: 0000000000000000
CapEff: 0000000000000000
CapBnd: 0000001fffffffff
Cpus_allowed:   ff
Cpus_allowed_list:      0-7
Mems_allowed:   1
Mems_allowed_list:      0
voluntary_ctxt_switches:        150
nonvoluntary_ctxt_switches:     545
```

### meminfo
```shell
spkce@pc-spkce:~/source/tmp$ cat /proc/meminfo
MemTotal:       16522068 kB	;所有可用RAM大小 （即物理内存减去一些预留位和内核的二进制代码大小）
MemFree:         4603360 kB	;LowFree与HighFree的总和，被系统留着未使用的内存
Buffers:           34032 kB	;用来给文件做缓冲大小
Cached:           188576 kB	;被高速缓冲存储器（cache memory）用的内存的大小（等于 diskcache minus SwapCache ）
SwapCached:            0 kB	;被高速缓冲存储器（cache memory）用的交换空间的大小,已经被交换出来的内存，但仍然被存放在swapfile中。用来在需要的时候很快的被替换而不需要再次打开I/O端口
Active:           167556 kB	;在活跃使用中的缓冲或高速缓冲存储器页面文件的大小,除非非常必要否则不会被移作他用
Inactive:         157876 kB	;在不经常使用中的缓冲或高速缓冲存储器页面文件的大小，可能被用于其他途径.
Active(anon):     103104 kB
Inactive(anon):    17440 kB
Active(file):      64452 kB
Inactive(file):   140436 kB
Unevictable:           0 kB
Mlocked:               0 kB
SwapTotal:      19660924 kB	;交换空间的总大小
SwapFree:       17340740 kB	;未被使用交换空间的大小
Dirty:                 0 kB	;等待被写回到磁盘的内存大小
Writeback:             0 kB	;正在被写回到磁盘的内存大小
AnonPages:        102824 kB	;未映射页的内存大小
Mapped:            71404 kB	;已经被设备和文件等映射的大小
Shmem:             17720 kB
Slab:              13868 kB	;内核数据结构缓存的大小，可以减少申请和释放内存带来的消耗
SReclaimable:       6744 kB	;可收回Slab的大小
SUnreclaim:         7124 kB	;不可收回Slab的大小（SUnreclaim+SReclaimable＝Slab）
KernelStack:        2848 kB	;内核栈区大小
PageTables:         2524 kB	;管理内存分页页面的索引表的大小
NFS_Unstable:          0 kB	;不稳定页表的大小
Bounce:                0 kB
WritebackTmp:          0 kB
CommitLimit:      515524 kB
Committed_AS:    3450064 kB
VmallocTotal:     122880 kB	;可以vmalloc虚拟内存大小
VmallocUsed:       21296 kB	;已经被使用的虚拟内存大小
VmallocChunk:      66044 kB
HardwareCorrupted:     0 kB
AnonHugePages:      2048 kB
HugePages_Total:       0	;大页面数目
HugePages_Free:        0	;空闲大页面数目
HugePages_Rsvd:        0
HugePages_Surp:        0
Hugepagesize:       2048 kB	;大页面一页大小
DirectMap4k:       12280 kB
DirectMap4M:      897024 kB
```