# 函数dl_iterate_phdr 

```c++
int dl_iterate_phdr(
    int (*callback)(struct dl_phdr_info *info, size_t size, void *data),
    void *data
);
```
遍历当前进程已加载的所有共享对象，并对每个对象调用一次回调函数 callback。

回调函数 callback‌

‌参数‌：
info：指向 dl_phdr_info 结构体的指针，包含共享对象的信息‌13。
size：dl_phdr_info 结构体的大小（用于兼容性判断）‌14。
data：用户传入的 data 参数副本‌
```c++

struct dl_phdr_info {
    ElfW(Addr) dlpi_addr;      // 共享对象基地址
    const char *dlpi_name;     // 对象名称（路径或空指针）
    const ElfW(Phdr) *dlpi_phdr; // 程序头数组指针
    ElfW(Half) dlpi_phnum;     // 程序头数量
};
```

# 函数 mprotect 
‌作用‌：修改指定内存区域的访问权限（如可读、可写、可执行）‌

```c++
#include <sys/mman.h>  
int mprotect(void *addr, size_t len, int prot);  
```
‌参数解析‌
‌addr‌ :目标内存区域的起始地址，必须按内存页对齐（通常是 4KB 的整数倍）
‌len‌:内存区域长度，需是内存页大小的整数倍
‌prot‌：新权限标志，支持按位或组合以下值：
PROT_READ：可读。
PROT_WRITE：可写
PROT_EXEC：可执行
PROT_NONE：禁止访问‌1
‌
返回值‌
‌成功‌：返回 0
‌失败‌：返回 -1，并设置 errno 标识具体错误，常见错误包括：
EACCES：权限冲突（如尝试将只读内存设为可写）
EINVAL：参数无效（如地址未页对齐或长度非页整数倍）

# pthread_atfork
```
int pthread_atfork(void (*prepare)(void), void (*parent)(void), void (*child)(void));
```
注册三个回调函数，在进程调用 fork() 创建子进程时，分别在不同阶段触发，用于解决多线程环境下 fork 导致的资源状态不一致问题‌



# 在 gcc 9.2 环境的开发板中运行 gcc7.5的程序

```
#都是gcc 7.5的库和程序
/xxx/lib/ld-linux-aarch64.so.1 --library-path /xxx/lib/:/xxx/xlib /xxx/test.out
```

# cmake sysroot
-DCMAKE_SYSROOT 是一个用于指定 ‌目标系统根目录（System Root）‌ 的编译选项，尤其在交叉编译（Cross-Compiling）时至关重要。它定义了工具链在编译时查找头文件、库和其他依赖的基准路径



# 控制台终端显示系统信息
```shell
#.bashrc
# spkce@ubuntu24.04:~$
if [ -f /etc/os-release ]; then
    . /etc/os-release
    os_info="${ID}${VERSION_ID}"
else
    os_info="ubuntuxx"
fi

if [ "$color_prompt" = yes ]; then
    PS1='${debian_chroot:+($debian_chroot)}\[\033[01;32m\]\u@'"$os_info"'\[\033[00m\]:\[\033[01;34m\]\w\[\033[00m\]\$ '
else
    PS1='${debian_chroot:+($debian_chroot)}\u@\h:\w\$ '
fi
```

# 查看gcc环境
```
spkce@ubuntu20.04:~$ aarch64-linux-gnu-gcc -print-search-dirs
install: /usr/lib/gcc-cross/aarch64-linux-gnu/9/
programs: =/usr/lib/gcc-cross/aarch64-linux-gnu/9/:/usr/lib/gcc-cross/aarch64-linux-gnu/9/:/usr/lib/gcc-cross/aarch64-linux-gnu/:/usr/lib/gcc-cross/aarch64-linux-gnu/9/:/usr/lib/gcc-cross/aarch64-linux-gnu/:/usr/lib/gcc-cross/aarch64-linux-gnu/9/../../../../aarch64-linux-gnu/bin/aarch64-linux-gnu/9/:/usr/lib/gcc-cross/aarch64-linux-gnu/9/../../../../aarch64-linux-gnu/bin/aarch64-linux-gnu/:/usr/lib/gcc-cross/aarch64-linux-gnu/9/../../../../aarch64-linux-gnu/bin/
libraries: =/usr/lib/gcc-cross/aarch64-linux-gnu/9/:/usr/lib/gcc-cross/aarch64-linux-gnu/9/../../../../aarch64-linux-gnu/lib/aarch64-linux-gnu/9/:/usr/lib/gcc-cross/aarch64-linux-gnu/9/../../../../aarch64-linux-gnu/lib/aarch64-linux-gnu/:/usr/lib/gcc-cross/aarch64-linux-gnu/9/../../../../aarch64-linux-gnu/lib/../lib/:/lib/aarch64-linux-gnu/9/:/lib/aarch64-linux-gnu/:/lib/../lib/:/usr/lib/aarch64-linux-gnu/9/:/usr/lib/aarch64-linux-gnu/:/usr/lib/../lib/:/usr/lib/gcc-cross/aarch64-linux-gnu/9/../../../../aarch64-linux-gnu/lib/:/lib/:/usr/lib/
```
```
spkce@ubuntu20.04:~$ aarch64-linux-gnu-gcc -print-file-name=libc.so
/usr/lib/gcc-cross/aarch64-linux-gnu/9/../../../../aarch64-linux-gnu/lib/../lib/libc.so
```
```
aarch64-linux-gnu-gcc  -v -E -x c /dev/null -o /dev/null
```