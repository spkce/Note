priority_queue默认使用vector作为底层存储容器，使用了堆算法将vector中的元素构造成了堆，默认大堆。
在任务调度、缓冲区、树的层次遍历和广度优先搜索有用。

默认创建大堆
```c++
#include <iostream>
#include <queue>
int main() {
    std::priority_queue<int> pq;
    return 0;
}
```

创建小堆需要自定义比较函数
```c++
#include <iostream>
#include <queue>
struct Compare {
    bool operator()(int a, int b) {
        return a > b;
    }
};
int main() {
    std::priority_queue<int, std::vector<int>, Compare> pq;
    return 0;
}
```
或者：
```c++
priority_queue<int, vector<int>, less<int>> queMin;
priority_queue<int, vector<int>, greater<int>> queMax;
```

```c++
static bool cmp(pair<int, int>& m, pair<int, int>& n)
{
    return m.second > n.second;
}
priority_queue<pair<int, int>, vector<pair<int, int>>, decltype(&cmp)> q(cmp);
```