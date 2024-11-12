## set
set 容器是标准模板库（Standard Template Library, STL）的一部分，设计用来存储唯一的元素，这些元素按照特定顺序排列。它的内部实现基于一种高度平衡的二叉树结构——红黑树（Red-Black Tree），这保证了即使在大量数据面前，操作（如插入、删除、查找）的效率也非常高，时间复杂度保持在 O(log n)。

实现自定义排序：
```c++
#include <set>
#include <iostream>
 
// 自定义比较函数
struct Compare {
    bool operator()(const int a, const int b) const {
        return a > b; // 降序排序
    }
};
 
int main() {
    std::set<int, Compare> customSet = {1, 3, 2, 5, 4};
 
    for (const int &element : customSet) {
        std::cout << element << " ";
    }
    // 输出：5 4 3 2 1
}
```

## unordered_set