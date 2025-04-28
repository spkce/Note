# dl_iterate_phdr
需包含 ‌<link.h>‌ 头文件,并启用 GNU 扩展宏 _GNU_SOURCE
```c++
#define _GNU_SOURCE   // 必须定义以启用 GNU 扩展功能
#include <link.h>     // 包含 dl_iterate_phdr 的函数声明和结构体定义
```
函数原型：
```c++
int dl_iterate_phdr(
    int (*callback)(struct dl_phdr_info *info, size_t size, void *data),
    void *data
);
```
callback‌：回调函数，用于处理每个共享库的信息‌。
‌data‌：用户自定义数据，透传给回调函数‌。

callback 函数的入参
struct dl_phdr_info‌：包含共享库基地址、名称、ELF 程序头等信息。
‌size‌：dl_phdr_info 结构体大小（兼容性字段）。
‌data‌：透传的 dl_iterate_phdr 的 data 参数‌

## dl_phdr_info 结构体
```c++
struct dl_phdr_info {  
    ElfW(Addr) dlpi_addr;    // 动态库的基地址（加载到内存的起始地址）  
    const char *dlpi_name;   // 动态库的路径名（未加载时为 NULL 或空字符串）  
    const ElfW(Phdr) *dlpi_phdr; // 指向 ELF 程序头数组的指针  
    ElfW(Half) dlpi_phnum;   // 程序头数组的长度（即段数量）  
};
//‌ElfW 宏‌：根据架构自动适配 ELF 数据类型（如 32 位为 Elf32_Addr，64 位为 Elf64_Addr）‌
```
dlpi_addr‌：态库在进程虚拟内存中的基地址，用于计算符号或段的内存地址。
dlpi_name：动态库的完整路径（如 /lib/x86_64-linux-gnu/libc.so.6），若为可执行文件自身则可能为空
dlpi_phdr‌：指向 ELF 程序头数组，描述动态库的段信息（如代码段、数据段）‌

## Phdr类型指针
```c++
typedef struct {  
    uint32_t p_type; 
    uint32_t p_flags;   // 段权限（可读、可写、可执行）  
    uint64_t p_offset;  // 段在 ELF 文件中的偏移量  
    uint64_t p_vaddr;   // 段在内存中的虚拟地址（需加基地址 dlpi_addr）  
    uint64_t p_paddr;   // 物理地址（通常与 p_vaddr 相同）  
    uint64_t p_filesz;  // 段在文件中的大小  
    uint64_t p_memsz;   // 段在内存中的大小（可能包含未初始化的数据）  
    uint64_t p_align;   // 对齐要求（内存和文件的对齐方式）  
} Elf64_Phdr;  
```
p_type‌：标识段的用途，常见类型包括：
PT_LOAD：需加载到内存的段（如代码段 .text、数据段 .data）
PT_DYNAMIC：动态链接信息段（如依赖库列表）‌
PT_INTERP：动态链接器路径（如 /lib64/ld-linux-x86-64.so.2）‌

‌p_flags‌：权限标志位，如 PF_R（可读）、PF_W（可写）、PF_X（可执行）‌

## ElfW(Dyn) 
ElfW(Dyn) 是 ‌ELF 文件格式中用于动态链接的核心数据结构‌，主要用于描述动态段（.dynamic 段）中的动态链接信息
```c++
typedef struct {
    ElfW(Sxword) d_tag;    // 动态条目类型标识（如 DT_STRTAB、DT_SYMTAB）
    union {
        ElfW(Xword) d_val; // 数值类型值（如大小、地址偏移）
        ElfW(Addr)  d_ptr; // 指针类型值（如内存地址）
    } d_un;
} ElfW(Dyn);
```
### 动态段（.dynamic 段）的作用
DT_STRTAB:	字符串表地址（如符号名称）
DT_SYMTAB:	符号表地址
DT_REL/DT_RELA:	重定位表地址
DT_INIT:	初始化函数地址
DT_FINI:	终止函数地址
DT_NEEDED:	依赖的共享库名称
动态链接器（如 ld-linux.so）通过遍历这些条目加载依赖库、解析符号并完成重定位‌
动态段查看
```
$ readelf -d a.out
Dynamic section at offset 0x7cc0 contains 29 entries:
  Tag        Type                         Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libstdc++.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [libgcc_s.so.1]
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
 0x000000000000000c (INIT)               0x2000
 0x000000000000000d (FINI)               0x530c
 0x0000000000000019 (INIT_ARRAY)         0x8cb0
 0x000000000000001b (INIT_ARRAYSZ)       8 (bytes)
 0x000000000000001a (FINI_ARRAY)         0x8cb8
 0x000000000000001c (FINI_ARRAYSZ)       8 (bytes)
 0x000000006ffffef5 (GNU_HASH)           0x3b0
 0x0000000000000005 (STRTAB)             0x768
 0x0000000000000006 (SYMTAB)             0x3d8
 0x000000000000000a (STRSZ)              998 (bytes)
 0x000000000000000b (SYMENT)             24 (bytes)
 0x0000000000000015 (DEBUG)              0x0
 0x0000000000000003 (PLTGOT)             0x8ed0
 0x0000000000000002 (PLTRELSZ)           720 (bytes)
 0x0000000000000014 (PLTREL)             RELA
 0x0000000000000017 (JMPREL)             0xd60
 0x0000000000000007 (RELA)               0xc70
 0x0000000000000008 (RELASZ)             240 (bytes)
 0x0000000000000009 (RELAENT)            24 (bytes)
 0x000000000000001e (FLAGS)              BIND_NOW
 0x000000006ffffffb (FLAGS_1)            Flags: NOW PIE
 0x000000006ffffffe (VERNEED)            0xba0
 0x000000006fffffff (VERNEEDNUM)         3
 0x000000006ffffff0 (VERSYM)             0xb4e
 0x000000006ffffff9 (RELACOUNT)          3
 0x0000000000000000 (NULL)               0x0
```

## ElfW(Rel) 
 ‌普通重定位项‌ 的基础结构体，包含以下字段（以 32 位为例）
```c++
typedef struct {
    Elf32_Addr  r_offset;  // 需重定位的位置偏移（相对于节或段）
    Elf32_Word  r_info;    // 符号索引与重定位类型组合值
} Elf32_Rel;               // 对应 SHT_REL 类型的重定位节
```
‌r_offset‌：指向需要重定位的目标地址，具体位置取决于上下文：
在可重定位文件（.o）中，偏移相对于所在节（如 .text 或 .data）的起始地址‌57。
在可执行文件或共享库中，偏移通常为虚拟地址（VMA）‌78。
‌r_info‌：复合字段，包含两部分信息：
‌高 24/28 位（32/64 位）‌：符号在符号表（.symtab 或 .dynsym）中的索引‌78。
‌低 8/4 位（32/64 位）‌：重定位类型（如 R_386_PC32、R_X86_64_64），决定如何计算修正值‌78。
```c++
sym_index = ELF_R_SYM(rela->r_info); // 提取符号索引
```
## ElfW(Rela)
ElfW(Rel)基础上增加 ‌显式加数addend.
r_addend‌：
在重定位计算时直接参与运算，无需从目标位置读取隐式加数‌78。
例如，公式 修正地址 = 符号地址 + r_addend‌
```c++
typedef struct {
    Elf32_Addr  r_offset;
    Elf32_Word  r_info;
    Elf32_Sword r_addend;  // 显式加数，直接参与地址计算
} Elf32_Rela;              // 对应 SHT_RELA 类型的重定位节
```
查看：
```
$ readelf -r a.out

Relocation section '.rela.dyn' at offset 0xc70 contains 10 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000008cb0  000000000008 R_X86_64_RELATIVE                    24e0
000000008cb8  000000000008 R_X86_64_RELATIVE                    24a0
000000009008  000000000008 R_X86_64_RELATIVE                    9008
000000008fd8  002400000006 R_X86_64_GLOB_DAT 0000000000000000 __cxa_finalize@GLIBC_2.2.5 + 0
000000008fe0  001000000006 R_X86_64_GLOB_DAT 0000000000000000 __libc_start_main@GLIBC_2.34 + 0
000000008fe8  001d00000006 R_X86_64_GLOB_DAT 0000000000000000 _ITM_deregisterTM[...] + 0
000000008ff0  002100000006 R_X86_64_GLOB_DAT 0000000000000000 __gmon_start__ + 0
000000008ff8  002200000006 R_X86_64_GLOB_DAT 0000000000000000 _ITM_registerTMCl[...] + 0
000000009010  001b00000001 R_X86_64_64       0000000000000000 __gxx_personality_v0@CXXABI_1.3 + 0
000000009020  002500000005 R_X86_64_COPY     0000000000009020 stderr@GLIBC_2.2.5 + 0

Relocation section '.rela.plt' at offset 0xd60 contains 30 entries:
  Offset          Info           Type           Sym. Value    Sym. Name + Addend
000000008ee8  000100000007 R_X86_64_JUMP_SLO 0000000000000000 _ZNKSt7__cxx1112b[...]@GLIBCXX_3.4.21 + 0
000000008ef0  000200000007 R_X86_64_JUMP_SLO 0000000000000000 __errno_location@GLIBC_2.2.5 + 0
000000008ef8  000300000007 R_X86_64_JUMP_SLO 0000000000000000 printf@GLIBC_2.2.5 + 0
000000008f00  000400000007 R_X86_64_JUMP_SLO 0000000000000000 _ZSt29_Rb_tree_in[...]@GLIBCXX_3.4 + 0
000000008f08  000500000007 R_X86_64_JUMP_SLO 0000000000000000 strstr@GLIBC_2.2.5 + 0
000000008f10  000600000007 R_X86_64_JUMP_SLO 0000000000000000 dl_iterate_phdr@GLIBC_2.2.5 + 0
000000008f18  000700000007 R_X86_64_JUMP_SLO 0000000000000000 strerror@GLIBC_2.2.5 + 0
000000008f20  000800000007 R_X86_64_JUMP_SLO 0000000000000000 _ZSt17__throw_bad[...]@GLIBCXX_3.4 + 0
000000008f28  000900000007 R_X86_64_JUMP_SLO 0000000000000000 __cxa_begin_catch@CXXABI_1.3 + 0
000000008f30  000a00000007 R_X86_64_JUMP_SLO 0000000000000000 _ZNSt7__cxx1112ba[...]@GLIBCXX_3.4.21 + 0
000000008f38  000b00000007 R_X86_64_JUMP_SLO 0000000000000000 munmap@GLIBC_2.2.5 + 0
000000008f40  000c00000007 R_X86_64_JUMP_SLO 0000000000000000 _ZNSt7__cxx1112ba[...]@GLIBCXX_3.4.21 + 0
000000008f48  000d00000007 R_X86_64_JUMP_SLO 0000000000000000 __cxa_guard_release@CXXABI_1.3 + 0
000000008f50  000e00000007 R_X86_64_JUMP_SLO 0000000000000000 _ZSt18_Rb_tree_in[...]@GLIBCXX_3.4 + 0
000000008f58  000f00000007 R_X86_64_JUMP_SLO 0000000000000000 open@GLIBC_2.2.5 + 0
000000008f60  001100000007 R_X86_64_JUMP_SLO 0000000000000000 _ZSt28__throw_bad[...]@GLIBCXX_3.4.29 + 0
000000008f68  001200000007 R_X86_64_JUMP_SLO 0000000000000000 __cxa_atexit@GLIBC_2.2.5 + 0
000000008f70  001300000007 R_X86_64_JUMP_SLO 0000000000000000 _Znwm@GLIBCXX_3.4 + 0
000000008f78  001400000007 R_X86_64_JUMP_SLO 0000000000000000 _ZdlPvm@CXXABI_1.3.9 + 0
000000008f80  001500000007 R_X86_64_JUMP_SLO 0000000000000000 _ZSt18_Rb_tree_de[...]@GLIBCXX_3.4 + 0
000000008f88  001600000007 R_X86_64_JUMP_SLO 0000000000000000 __stack_chk_fail@GLIBC_2.4 + 0
000000008f90  001700000007 R_X86_64_JUMP_SLO 0000000000000000 _ZNSt7__cxx1112ba[...]@GLIBCXX_3.4.21 + 0
000000008f98  001800000007 R_X86_64_JUMP_SLO 0000000000000000 fprintf@GLIBC_2.2.5 + 0
000000008fa0  001900000007 R_X86_64_JUMP_SLO 0000000000000000 __cxa_rethrow@CXXABI_1.3 + 0
000000008fa8  001a00000007 R_X86_64_JUMP_SLO 0000000000000000 __cxa_end_catch@CXXABI_1.3 + 0
000000008fb0  001c00000007 R_X86_64_JUMP_SLO 0000000000000000 fstat@GLIBC_2.33 + 0
000000008fb8  001e00000007 R_X86_64_JUMP_SLO 0000000000000000 _Unwind_Resume@GCC_3.0 + 0
000000008fc0  001f00000007 R_X86_64_JUMP_SLO 0000000000000000 mmap@GLIBC_2.2.5 + 0
000000008fc8  002000000007 R_X86_64_JUMP_SLO 0000000000000000 __cxa_guard_acquire@CXXABI_1.3 + 0
000000008fd0  002300000007 R_X86_64_JUMP_SLO 0000000000000000 close@GLIBC_2.2.5 + 0
```
## ElfW(Sym)
主要用于动态链接及符号解析过程
```c++
typedef struct
{
  Elf32_Word	st_name;		/* Symbol name (string tbl index) */
  Elf32_Addr	st_value;		/* Symbol value */
  Elf32_Word	st_size;		/* Symbol size */
  unsigned char	st_info;		/* Symbol type and binding */
  unsigned char	st_other;		/* Symbol visibility */
  Elf32_Section	st_shndx;		/* Section index */
} Elf32_Sym;

typedef struct
{
  Elf64_Word	st_name;		/* Symbol name (string tbl index) */
  unsigned char	st_info;		/* Symbol type and binding */
  unsigned char st_other;		/* Symbol visibility */
  Elf64_Section	st_shndx;		/* Section index */
  Elf64_Addr	st_value;		/* Symbol value */
  Elf64_Xword	st_size;		/* Symbol size */
} Elf64_Sym;
```

#‌ ELF 文件执行加载机制
1. 触发加载流程
* ‌系统调用入口‌：用户通过 execve() 系统调用启动 ELF 文件，内核调用 do_execve() 函数处理加载请求‌。
* 文件头验证‌：内核读取 ELF 头（Elf_Ehdr）的前 128 字节，校验魔数（0x7F 0x45 0x4C 0x46）和文件类型（如 ET_EXEC 或 ET_DYN），确认其为有效 ELF 文件‌
2. 内存映射与段加载
* 程序头表解析‌ ：内核遍历程序头表，识别所有 PT_LOAD 类型的段。(如代码段 .text、数据段 .data)，并按段权限（读/写/执行）映射到进程的虚拟内存空间‌
* 动态链接器加载 : 若文件为动态链接（存在 PT_INTERP 段），内核加载解释器（如 ld-linux.so），由其负责后续的动态链接操作‌
3. 动态链接与符号解析
* 共享库依赖处理 ‌: 解释器根据 .dynamic 节中的依赖信息（如 DT_NEEDED）加载所有共享库（.so），并解析符号地址‌.
* 重定位与延迟绑定‌：通过 ‌全局偏移表（ GOT ）‌ 和 ‌过程链接表（ PLT ）‌ 实现函数地址的动态解析，首次调用时由解释器完成符号绑定‌
4. 执行入口跳转
* 入口地址设置‌：内核将程序计数器（PC）指向 ELF 头中定义的入口地址（e_entry），或交由解释器接管控制权（动态链接场景）‌

# Section合并
⼀个ELF会有多种不同的Section，在加载到内存的时候，也会进⾏Section合并，形成segment
目的是为了节省从磁盘加载到内存过程中的内存空间，一个页大小是4kb（4096字节），如果不合并前.text大小为4097字节，.data是1字节，那么它们会占3个页面，但是合并过后总大小为4098字节，那么只需要两个页，section在链接时起作用而segment在执行时起作用，segment其实就是往下合并section，两者的关系是segment包含section

合并原则：相同属性，⽐如：可读，可写，可执⾏，需要加载时申请空间等.

# 静态连接
静态链接就是把库中的.o进⾏合并
```
            a.o                              b.o
|ELF Header|.text|.data|.bss|      |ELF Header|.text|.data|.bss|
            ↓             相应节合并              ↓            
|ELF Header|    .text   |   .data    |    .bss   | 
ab.out
```

# 动态连接
动态链接实际上将链接的整个过程推迟到了程序加载的时候。⽐如我们去运⾏⼀个程序，操作系统会⾸先将程序的数据代码连同它⽤到的⼀系列动态库先加载到内存，其中每个动态库的加载地址都是不固定的，操作系统会根据当前地址空间的使⽤情况为它们动态分配⼀段内存。

### 动态链接器
动态链接器（如ld-linux.so）负责在程序运⾏时加载动态库。当程序启动时，动态链接器会解析程序中的动态库依赖，并加载这些库到内存中

动态库也是elf类型的文件，当文件链接动态库的时候，发生以下过程：
1. 通过mm_struct中的变量找到有关共享区的结构体，根据里面的成员指针变量找到路径
2. 根据路径找到磁盘中的数据块加载到内存
3. 发生映射关系关联起来
4. 得到库的起始虚拟地址
5. 数据区会有一个名为.GOT的表记录库函数的偏移量，映射过后，表会根据库的起始虚拟地址进行修改得到完整的访问共享区的地址