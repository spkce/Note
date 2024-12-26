# STL的六大部件
分别是容器（container）
迭代器（iterator）
适配器（adaptor）
分配器（allocator）
仿函数（functor）
算法（algorithm）

## STL的容器
顺序容器:主要有vector、deque、list、forward_list和array。
其中forward_list和array是C++11引入的。

关联容器:主要有set、map、multiset、multimap、unordered_set、unordered_map、unordered_multiset、unordered_multiamp。
其中后四种是C++11新引入的

## STL的迭代器
输入迭代器（Input Iterator）
输出迭代器（Output Iterator）
前向迭代器（Forward Iterator）
双向迭代器（Bidirectional Iterator）
随机访问迭代器（Random Access Iterator）

输入和输出迭代器分别用于读取和写入数据，前向迭代器只能向前访问而不能向后访问（forward_list），双向迭代器既可向前也可向后（list），随机访问迭代器可以通过下标访问任何合法的位置（vector）

## STL的适配器
适配器是一种设计模式。主要起到将不同的接口统一起来的作用。STL中的容器适配器如stack和queue，通过调用容器的接口，实现适配器所需的功能

## STL中常见的算法
一般分为三类，查找、排序和数值操作。
查找常用的有std::find、std::find_if、std::find_first_of等。
排序主要用std::sort及其家族的一系列算法。
数值操作主要用std::accumulate求和。