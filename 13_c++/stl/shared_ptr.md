# shared_ptr
该类型智能指针在实现上采用的是引用计数机制，即便有一个 shared_ptr 指针放弃了堆内存的“使用权”（引用计数减 1），也不会影响其他指向同一堆内存的 shared_ptr 指针（只有引用计数为 0 时，堆内存才会被自动释放）

```c++
std::shared_ptr<MyClass> ptr1 = std::make_shared<MyClass>();
std::shared_ptr<MyClass> ptr2 = ptr1;
std::shared_ptr<MyClass> ptr3 = ptr1;
```
# unique_ptr
具有独占所有权语义，一个对象只能被一个unique_ptr所拥有，当unique_ptr被销毁时，它所指向的对象也会随之被销毁
```c++
std::unique_ptr<MyClass> ptr1 = std::make_unique<MyClass>()
```

# weak_ptr
weak_ptr 是一种不控制所指向对象生存期的智能指针，它指向一个由 shared_ptr 管理的对象，但不会增加对象的引用计数。当需要访问所指向的对象时，可以通过 weak_ptr 临时创建一个 shared_ptr，如果此时对象仍然存在，则可以正常访问，否则返回一个空的 shared_ptr。这样就可以在不增加引用计数的情况下访问对象，从而打破循环引用

在这个例子中，A 类和 B 类中的 shared_ptr 成员被替换成了 weak_ptr，从而打破了循环引用，使得 A 和 B 对象在不再被其他 shared_ptr 引用时能够正确地被析构。
```c++
#include <iostream>
#include <memory>

class A;
class B;

class A {
public:
    std::weak_ptr<B> b;
    ~A() { std::cout << "A destructor called." << std::endl; }
};

class B {
public:
    std::weak_ptr<A> a;
    ~B() { std::cout << "B destructor called." << std::endl; }
};

int main() {
    std::shared_ptr<A> a = std::make_shared<A>();
    std::shared_ptr<B> b = std::make_shared<B>();
    a->b = b;
    b->a = a;

    // 通过weak_ptr的lock函数获取shared_ptr来访问对象
    if (auto shared_b = a->b.lock()) {
        std::cout << "Accessed B object through weak_ptr." << std::endl;
    }

    return 0;
}
```