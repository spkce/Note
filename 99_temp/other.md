std::future


​```mermaid
classDiagram
    class 动物
    动物 : String 标签
    动物 : 吃()
​```


.dis 反汇编文件
```c++
fromelf -c a.out > a.dis
```