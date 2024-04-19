### 1.查找一个应用程序pid
```shell
spkce@pc-spkce:~/source$ ps -u
USER       PID %CPU %MEM    VSZ   RSS TTY      STAT START   TIME COMMAND
spkce       10  0.0  0.0  18348  3156 tty1     S    Feb20   0:00 -bash
spkce      105  0.0  0.0  18468  3300 tty2     S    Feb27   0:02 -bash
spkce     1550  103  0.0  13928  1524 tty2     R    11:41   0:06 ./a.out
spkce     1551  0.0  0.0  18668  1932 tty1     R    11:41   0:00 ps -u
```
### 2. 查找内存分布
```shell
(gdb) shell cat /proc/1550/maps
......
7f11d8a6f000-7f11d8a72000 rw-p 00000000 00:00 0
7f11d8a7d000-7f11d8a7e000 r--p 00000000 00:00 132439             /mnt/d/source/tmp/a.out
7f11d8a7e000-7f11d8a7f000 r-xp 00001000 00:00 132439             /mnt/d/source/tmp/a.out
7f11d8a7f000-7f11d8a80000 r--p 00002000 00:00 132439             /mnt/d/source/tmp/a.out
7f11d8a80000-7f11d8a81000 r--p 00002000 00:00 132439             /mnt/d/source/tmp/a.out
7f11d8a81000-7f11d8a82000 rw-p 00003000 00:00 132439             /mnt/d/source/tmp/a.out
7fffe0939000-7fffe095a000 rw-p 00000000 00:00 0                  [heap]
7fffe7985000-7fffe8185000 rw-p 00000000 00:00 0                  [stack]
7fffe8433000-7fffe8434000 r-xp 00000000 00:00 0                  [vdso]
```

### 3. gdb dump
```
(gdb) attach 1550
Attaching to process 1550
(gdb) dump memory ./memory.dump 0x7fffe7985000 0x7fffe8185000
```
把7fffe7985000-7fffe8185000地址的内存 输出到./memory.dump中
