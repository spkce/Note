### pthread_setaffinity和pthread_getaffinity
用来设置和获取某一线程和CPU的亲和性的。简单来讲就是：某一线程跑在了哪个CPU上以及某个CPU上都跑了哪些线程；表示线程和CPU之间关系的函数

```c++
CPU_ZERO(&cpuset);
CPU_SET(0, &cpuset); /* cpu 0 is in cpuset now */
/* bind process to processor 0 */
if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) !=0)
{}

```

