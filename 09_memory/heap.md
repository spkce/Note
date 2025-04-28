## 1. malloc_state
```c++
struct malloc_state
{
  /* Serialize access.  */
  mutex_t mutex;
  
  /* Flags (formerly in max_fast).  */
  int flags;

  /* Fastbins */
  mfastbinptr fastbinsY[NFASTBINS];

  /* Base of the topmost chunk -- not otherwise kept in a bin */
  mchunkptr top;

  /* The remainder from the most recent split of a small request */
  mchunkptr last_remainder;

  /* Normal bins packed as described above */
  mchunkptr bins[NBINS * 2 - 2];

  /* Bitmap of bins */
  unsigned int binmap[BINMAPSIZE];

  /* Linked list */
  struct malloc_state *next;

  /* Linked list for free arenas.  */
  struct malloc_state *next_free;

  /* Memory allocated from the system in this arena.  */
  INTERNAL_SIZE_T system_mem;
  INTERNAL_SIZE_T max_system_mem;
};
```
### arene
主线程和子线程有自己独立的 arena，那么是不是无论有多少个线程，每个线程都有自己独立的 arena 呢？答案是否定的。事实上，arena的个数是跟系统中处理器核心个数相关的，如下表所示：

systems	|number of arena
----------|---------
32bits	|2 x number of cpu cores + 1
64bits	|8 x number of cpu cores + 1
* 当主线程首次调用 malloc 的时候会直接为它分配一个 main arena，而不需要任何附加条件。
* 当子线程 1 和子线程 2 首次调用 malloc 的时候，glibc 实现的 malloc 会分别为每个子线程创建一个新的 thread arena。此时，各个线程与 arena 是一一对应的。但是，当用户线程 3 调用 malloc 的时候就出现问题了。因为此时 glibc 的 malloc 能维护的 arena 个数已经达到上限，无法再为子线程 3 分配新的 arena 了，那么就需要重复使用已经分配好的 3 个 arena 中的一个（main arena, arena1 或者 arena2）。那么该选择哪个 arena 进行重复利用呢？glibc 的 malloc 遵循以下规则：

1. 首先循环遍历所有可用的 arena，在遍历的过程中，它会尝试加锁该 arena。如果成功加锁（该 arena 当前对应的线程并未使用堆内存则表示可加锁），比如将 main arena 成功锁住，那么就将 main arena 返回给用户，即表示该 arena 被子线程 3 共享使用。
3. 如果没能找到可用的 arena，那么就将子线程 3 的 malloc 操作阻塞，直到有可用的 arena 为止。
3. 现在，如果子线程 3 再次调用 malloc 的话，glibc 的 malloc 就会先尝试使用最近访问的 arena（此时为 main arena）。如果此时 main arena 可用的话，就直接使用，否则就将子线程 3 阻塞，直到 main arena 再次可用为止。

## 2. top 
当一个 chunk 处于一个arena的最顶部（即最高内存地址处）的时候，就称之为 top chunk。该 chunk 并不属于任何 bin ，而是在当前的 heap 所有的 free chunk （无论那种 bin）都无法满足用户请求的内存大小的时候，将此 chunk 当做一个应急消防员，分配给用户使用。如果 top chunk 的大小比用户请求的大小要大的话，就将该 top chunk 分作两部分：用户请求的 chunk 和 剩余的部分（成为新的 top chunk）。否则，就需要扩展 heap 或分配新的 heap 了，在 main arena 中通过 sbrk 扩展 heap，而在thread arena 中通过 mmap 分配新的 heap。
## 3. bin
bin是一种记录free chunk的链表数据结构。系统针对不同大小的free chunk，将bin分为了4类：
1. Fast bin;
2. Unsorted bin; 
3. Small bin; 
4. Large bin。

`struct malloc_state`中有定义`fastbinsY[NFASTBINS]` 用于fast bin。NFASTBINS的大小为10。
`struct malloc_state`中有定义`mchunkptr bins[NBINS * 2 - 2]`(NBINS为128) bin。
bin[0] :  : 用于unsorted bin 1个
bin[1] - bin[62] : small bin 62个
bin[63] -  bin[125] ：用于large bins 63个
仅仅使用126个（1+62+63）最后一个不用
每个 bin 在 bins 数组中占据两个连续的位置，分别存储链表的头指针和尾指针， 专门用于计算位置的宏bin_at
```c++
#define bin_at(m, i) \
  (mbinptr) (((char *) &((m)->bins[((i) - 1) * 2]))			      \
             - offsetof (struct malloc_chunk, fd))
```
## 4. Fast bin
用于存储快速释放块链表，当用户释放一块不大于max_fast（32bit, user size = 64, chunk size = 80; 64bit, chunk size 160B）的chunk（一般小内存）的时候，会默认会被放到fast bins上。

### 4.1 fast bin 链表
每个fast bin都是一个单链表(只使用fd指针)。在fast bin中无论是添加还是移除fast chunk，都是对“链表尾”进行操作，而不会对某个中间的fast chunk进行操作。fastbinsY数组中每个fastbin元素均指向了该链表的rear end（尾结点），而尾结点通过其fd指针指向前一个结点，依次类推
Fastbins中的fd指针指向的是下一个Chunk的用户数据区起始地址，而非Chunk头部。Chunk头部位于用户数据区前16字节（prev_size和size各占8字节）。直接访问fd指向的地址会尝试读取用户数据区，而非Chunk元数据，可能导致无效访问。

### 4.2 fast bin的分布
10个fast bin中所包含的fast chunk size是按照步进8字节排列的，即第一个fast bin中所有fast chunk size均为16字节，第二个fast bin中为24字节，依次类推。在进行malloc初始化的时候，最大的fast chunk size被设置为80字节(chunk unused size为64字节)，因此默认情况下大小为16到80字节的chunk被分类到fast chunk。
链表索引宏定义： fastbin_index ； 通过此宏能判断某一个fastchunk属于哪一个fastbin链表
```c++
#define fastbin_index(sz) \
  ((((unsigned int) (sz)) >> (SIZE_SZ == 8 ? 4 : 3)) - 2)
```
### 4.3 fast bin不会对free chunk进行合并操作
鉴于设计fast bin的初衷就是进行快速的小内存分配和释放，因此系统将属于fast bin的chunk的P(未使用标志位)总是设置为1，这样即使当fast bin中有某个chunk同一个free chunk相邻的时候，系统也不会进行自动合并操作，而是保留两者。虽然这样做可能会造成额外的碎片化问题，但瑕不掩瑜。

### 4.4 malloc操作
用户通过malloc请求的大小属于fast chunk的大小范围(注意：用户请求size加上16字节就是实际内存chunk size)。在初始化的时候fast bin支持的最大内存大小以及所有fast bin链表都是空的，所以当最开始使用malloc申请内存的时候，即使申请的内存大小属于fast chunk的内存大小(即16到80字节)，它也不会交由fast bin来处理，而是向下传递交由small bin来处理，如果small bin也为空的话就交给unsorted bin处理

### 4.5 free操作
先通过chunksize函数根据传入的地址指针获取该指针对应的chunk的大小；然后根据这个chunk大小获取该chunk所属的fast bin，然后再将此chunk添加到该fast bin的链尾即可。整个操作都是在_int_free函数中完成。

## 5. unsorted bin
当释放较小或较大的chunk的时候，如果系统没有将它们添加到对应的bins中，系统就将这些chunk添加到unsorted bin中。

unsortedbin采用的遍历顺序是FIFO

## 6. Small Bin 
小于1024字节（0x400）的chunk称之为small chunk，small bin就是用于管理small chunk的。就内存的分配和释放速度而言，small bin比larger bin快，但比fast bin慢。

内存释放操作就将新释放的chunk添加到链表的front end(前端)，分配操作就从链表的rear end(尾端)中获取chunk。
### 6.1 Small Bin的分布
单个smallbin链表中的chunk大小都是相同的，第一个small bin链中chunk的大小为32字节，后续每个small bin中chunk的大小依次增加两个机器字长（32位相差8字节，64位相差16字节）

### 6.2 合并操作
相邻的free chunk需要进行合并操作，即合并成一个大的free chunk。

## 7. Large chunk
大于等于1024字节（0x400）的chunk称之为large chunk，large bin就是用于管理这些largechunk的。

### 7.1 Large bin分布
large bin链表的个数为63个，被分为6组。
在这63个largebins中：第一组的32个largebin链依次以64字节步长为间隔，即第一个largebin链中chunksize为1024-1087字节，第二个large bin中chunk size为1088~1151字节。第二组的16个largebin链依次以512字节步长为间隔；第三组的8个largebin链以步长4096为间隔；第四组的4个largebin链以32768字节为间隔；第五组的2个largebin链以262144字节为间隔；最后一组的largebin链中的chunk大小无限制。

组 | 数量 | 步长(Byte)
---------|----------|---------
 1 | 32 | 64
 2 | 16 | 512
 3 | 8 | 4096
 4 | 4 | 32768
 5 | 2 | 262144
 6 | 1 | 不限制

### 7.2 Large bin的链接
* large chunk使用fd_nextsize、bk_nextsize连接起来的。
* large chunk可以添加、删除在large bin的任何一个位置.同一个largebin中每个chunk的大小可以不一样，这些chunk根据一定的范围存储在一个larbin链表中
* 在同一个largebin中：每个chunk的大小不一定相同，因此为了加快内存分配和释放的速度，就将同一个largebin中的所有chunk按照chunksize进行从大到小的排列：最大的chunk放在一个链表的front end，最小的chunk放在rear end；相同大小的chunk按照最近使用顺序排序。

## Last Remainder Chunk
用于提高内存分配的局部性：连续malloc中，各个chunk在内存分布中是相邻的
当用户请求的是一个small chunk，且该请求无法被small bin、unsorted bin满足的时候，就通过binmaps遍历bin查找最合适的chunk，如果该chunk有剩余部分的话，就将该剩余部分变成一个新的chunk加入到unsorted bin中，另外，再将该新的chunk变成新的last remainder chunk。

## 8. malloc_chunk
malloc_chunk 的结构如下定义：
```c++
struct malloc_chunk {

  size_t mchunk_prev_size;  /* Size of previous chunk (if free).  */
  size_t mchunk_size;       /* Size in bytes, including overhead. */

  struct malloc_chunk* fd;         /* double links -- used only if free. */
  struct malloc_chunk* bk;

  /* Only used for large blocks: pointer to next larger size.  */
  struct malloc_chunk* fd_nextsize; /* double links -- used only if free. */
  struct malloc_chunk* bk_nextsize;
};

#define PREV_INUSE 0x1
/* extract inuse bit of previous chunk */
#define prev_inuse(p)       ((p)->mchunk_size & PREV_INUSE)
/* size field is or'ed with IS_MMAPPED if the chunk was obtained with mmap() */
#define IS_MMAPPED 0x2
/* check for mmap()'ed chunk */
#define chunk_is_mmapped(p) ((p)->mchunk_size & IS_MMAPPED)
/* size field is or'ed with NON_MAIN_ARENA if the chunk was obtained
   from a non-main arena.  This is only set immediately before handing
   the chunk to the user, if necessary.  */
#define NON_MAIN_ARENA 0x4
/* Check for chunk from main arena.  */
#define chunk_main_arena(p) (((p)->mchunk_size & NON_MAIN_ARENA) == 0)
```
当chunk空闲时，其M状态是不存在的，只有AP状态（因为M表示是由brk还是mmap分配的内存，而mmap分配的内存free时直接ummap，不会放到空闲链表中。换言之空闲链表中的都死brk分配的，所以不用额外记录）

为了防止碎片化，堆中不存在两个相邻的 free chunk（如果存在，则被堆管理器合并了）

mchunk_size 为 0，很可能这个 chunk 是非法的或者未被正确初始化

### 8.1 allocated chunk：
```c
+-------------------------+ <- this chunk
|         prev_size       |
+-------------------------+
|   size      | N | M | P |
+-------------------------+
|        user data        |
+-------------------------+ <- next chunk 
|        prev_size        |
+-------------------------+
```
* prev_size: 如果前一个 chunk 是 free chunk，则这个内容保存的是前一个 chunk 的大小
如果前一个 chunk 是 allocated chunk，则这个区域保存的是前一个 chunk 的用户数据（一部分而已，主要是为了能够充分利用这块内存空间）。
* size: 保存的是当前这个 chunk 的大小。总共是 32 位，并且最后的 3 位作为标志位：
* P：表示前一个 chunk 是否为 allocated chunk，而当前是不是 chunk allocated 可以通过查询下一个 chunk 的这个标志位来得知）
* M：表示当前 chunk 是否是通过 mmap 系统调用产生的，子线程是 mmap，主线程则是通过 brk。
* N：表示当前 chunk 是否属于 main arena，也就是主线程的 arena。

### 8.2 free chunk
```c
+-------------------------+ <- this chunk
|         prev_size       |
+-------------------------+
|   size      | N | M | P |
+-------------------------+
|            fd           |
+-------------------------+
|            bk           |
+-------------------------+
|           unused        |
+-------------------------+ <- next chunk 
|        prev_size        |
+-------------------------+
```
* prev_size: 为了防止碎片化，堆中不存在两个相邻的 free chunk （如果存在，则被堆管理器合并了）。因此对于一个 free chunk ，这个 prev_size 区域中一定包含的上一个 chunk 的部分有效数据或者为了地址对齐所做的填充对齐。
* size: 同 allocated chunk ，表示当前 chunk 的大小，其标志位N，M，P 也同 allocated chunk 一样。
* fd: 前向指针——指向当前 chunk 在同一个 bin（一种用于加快内存分配和释放效率的显示链表）的下一个 chunk
* bk: 后向指针——指向当前 chunk 在同一个 bin 的上一个 chunk


### 8.3 chunk中的空间复用
为了使得 chunk 所占用的空间最小, ptmalloc 使用了空间复用, 一个 chunk 或者正在被使用, 或者已经被 free 掉, 所以 chunk 的中的一些域可以在使用状态和空闲状态表示不同的意义, 来达到空间复用的效果. 空闲时, 一个 chunk 中至少要4个 size_t 大小的空间, 用来存储 prev_size, size , fd 和 bk ， 也就是16 bytes（？？为什么不是6个size_t呢？不是还有fd_nextsize和bk_nextsize吗？——并不是所有bin中都需要这两个指针，比如在fast_bin中，每隔8个Byte就有一个链表，每个链表中的所有chunk的size都是一样的，显然不用这两个指针） chuck 的大小要 align 到8 bytes. 当一个 chunk 处于使用状态时, 它的下一个 chunk 的 prev_size 域肯定是无效的. 所以实际上, 这个空间也可以被当前 chunk 使用. 这听起来有点不可思议, 但确实是合理空间复用的例子. 故而实际上, 一个使用中的 chunk 的大小的计算公式应该是:

in_use_size = ( 用户请求大小 + 8 - 4 ) align to 8 bytes 这里加8是因为需要存储 prev_size 和 size, 但又因为向下一个 chunk “借”了4个bytes, 所以要减去4. 最后, 因为空闲的 chunk 和使用中的 chunk 使用的是同一块空间. 所以肯定要取其中最大者作为实际的分配空间. 即最终的分配空间 chunk_size = max(in_use_size, 16). 这就是当用户请求内存分配时, ptmalloc 实际需要分配的内存大小, 在后面的介绍中. 如果不是特别指明的地方, 指的都是这个经过转换的实际需要分配的内存大小, 而不是用户请求的内存分配大小.

# thread_arena 与堆段的关系
一个 thread_arena 可管理多个堆段（通过 heap_info 链表实现），每个堆段由 mmap 分配，默认最大为 ‌64MB‌（64 位系统）‌。
当当前堆段内存耗尽时，thread_arena 会创建新堆段并更新 heap_info 链表‌。
子线程的堆段大小固定，无法像主线程的堆（通过 brk 扩展）动态调整。若需更多内存，必须创建新堆段‌.

一个线程在第一个malloc时会绑定一个thread_arena，存储在这
```c++
static __thread mstate thread_arena attribute_tls_model_ie;
```

# heap
```c++
typedef struct _heap_info
{
  mstate ar_ptr; /* Arena for this heap. */
  struct _heap_info *prev; /* Previous heap. */
  size_t size;   /* Current size in bytes. */
  size_t mprotect_size; /* Size in bytes that has been mprotected
                           PROT_READ|PROT_WRITE.  */
  size_t pagesize; /* Page size used when allocating the arena.  */
  /* Make sure the following data is properly aligned, particularly
     that sizeof (heap_info) + 2 * SIZE_SZ is a multiple of
     MALLOC_ALIGNMENT. */
  char pad[-3 * SIZE_SZ & MALLOC_ALIGN_MASK];
} heap_info;
```
heap_info结构位于堆段的‌起始位置‌，（该heap不属于子线程或不是子线程第一个heap）紧接其后的是用户可分配的内存区域（即malloc_chunk结构）。
```
| heap_info (元数据) | chunk 1 | chunk 2 | ... | top chunk |
↑                   ↑  
堆段基址          第一个chunk起始地址
```
堆段对齐规则‌
每个通过mmap分配的堆段（heap segment）均按HEAP_MAX_SIZE（如64位系统默认64MB）对齐，确保堆段基址满足 heap_start % HEAP_MAX_SIZE == 0‌。

* 通过top chunk反向计算heap_info
获取top chunk的地址（通过arena->top字段）
计算堆段基址：heap_base = top_chunk_addr & ~(HEAP_MAX_SIZE - 1)

top chunk 的更新规则‌
新 heap 分配后，其顶部内存块被设为新的 top chunk，并更新 malloc_state->top 指针指向该位置‌24。
旧 heap 的 top chunk 可能被标记为空闲块（如合并到 unsorted bin）或保留为已分配状态，但不再作为当前活跃的 top chunk。

```c++
typedef struct _heap_info {
  mstate ar_ptr;          // 指向所属 arena 的指针
  struct _heap_info *prev; // 指向前一个 heap_info 的链表指针
  size_t size;            // 当前堆段大小
  // ... 其他字段（如对齐填充）
} heap_info;
```
# main_arena与heap关系
Main thread不含有多个heaps所以也就不含有heap_info结构体。当需要更多堆空间的时候，就通过扩展sbrk的heap segment来获取更多的空间，直到它碰到内存mapping区域为止。
main_arena一般在libc的数据段区
```
                                      main_arena.top_chunk
                                             ↓
|...|malloc chunk|free chunk|malloc chunk|.......|top chunk|
低地址                                              高地址
```
# thread_arena与单一heap关系
在仅仅有一个heap时thead_arena位于该heap中仅次于heap_info的位置。heap_info.prev为null
```
    heap_info.ar_ptr                           thead_arena.top_chunk
          ↓                                           ↓
|heap_info|thead_arena|...|malloc chunk|free chunk|.......|top chunk|
低地址                                                 高地址
```
# thread_arena与多个heap关系
若有多个heap，这些heap地址可能并不相连。thead_arena依然在原heap中。原heap中topchunk会被拆解，空闲chunk被挂到bins中。
新heap的高地址空闲chunk成为新的topchunk。新heap的prev指针指向原heap。
```
    heap_info1.ar_ptr
    heap_info0.ar_ptr                           
           ↓                                           
|heap_info0|thead_arena|...|malloc chunk|free chunk|.......|free chunk|
低地址                                                 高地址
↑
heap_info1.prev                                  thead_arena.top_chunk
                                                      ↓
|heap_info1|thead_arena|malloc chunk|free chunk|.......|top chunk|
```
# tcache
在栈区有指针
```c++
static __thread tcache_perthread_struct *tcache = NULL;
```
```c++
typedef struct tcache_perthread_struct
{
  uint16_t counts[TCACHE_MAX_BINS];
  tcache_entry *entries[TCACHE_MAX_BINS];
} tcache_perthread_struct;
```
‌tcache_perthread_struct每个线程独立维护的结构体.
counts：记录每个tcache bin中当前缓存的chunk数量（上限为7）‌
entries：指向单链表头部的指针数组，每个链表对应不同大小的内存块（16字节递增‌ 的 chunk ）
该结构体在堆区起始位置分配，总大小为0x250字节‌

## tcache_entry
```c++
typedef struct tcache_entry
{
  struct tcache_entry *next;
} tcache_entry;
```
tcache_entry用于链接空闲的chunk结构体，其中next指针指向下一个大小相同的chunk.
这里需要注意的是next指向chunk的data部分，这和fastbin有一些不同，fastbin的fd指向的是下一个chunk的头指针。tcache_entry会复用空闲chunk的data部分.

tcache中chunk不使用arena中chunk的链表进行管理
```c
+-------------------------+ 
|         prev_size       |
+-------------------------+
|   size      | N | M | P |
+-------------------------+
|            next        |
+-------------------------+ 
|        ..........       |
+-------------------------+
+-------------------------+ 
|         prev_size       |
+-------------------------+
|   size      | N | M | P |
+-------------------------+ <- next chunk 
|            next         |
+-------------------------+ 
|        ..........       |
+-------------------------+
```