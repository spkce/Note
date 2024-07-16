# 重载运算符

## 比较

函数 | 说明 | 算式
---------|:----------|:---------:
 `__lt__(self,other)` | 定义小于号的行为 | x<y
 `__le__(self,other)` | 定义小于等于号的行为 | x<=y
 `__eq__(self,other)` | 定义等于号的行为 | x==y
 `__ne__(self,other)` | 定义不等号的行为 | x!=y
 `__gt__(self,other)` | 定义大于号的行为 | x>y
 `__ge__(self,other)` | 定义大于等于号的行为 | x>=y

## 数学运算

函数 | 说明 | 算式
---------|:----------|:---------:
 `__add__(self,other)` | 定义加法的行为 | x+y
 `__sub__(self,other)` | 定义减法的行为 | x-y
 `__mul__(self,other)` | 定义乘法的行为 | x*y
 `__truediv__(self,other)` | 定义真除法的行为 | x/y
 `__floordiv__(self,other)` | 定义整数除法的行为 | x//y
 `__mod__(self,other)` | 定义取模算法的行为 | x%y
 `__divmod__(self,other)` | 同时返回商，余数 | 
 `__pow__(self,other)` | 定义幂运算的行为 | x**y
 `__lshift__(self,other)` | 定义按位左移位的行为 | x<<y
 `__rshift__(self,other)` | 定义按位右移位的行为 | x>>y
 `__and__(self,other)` | 定义按位与操作的行为 | x&y
 `__xor__(self,other)` | 定义按位异或操作的行为 | x^y
 `__or__(self,other)` | 定义按位或操作的行为 | `x|y`

 函数 | 说明 | 算式
---------|:----------|:---------:
 `__iadd__(self,other)` | 定义赋值加法的行为 | x+=y
 `__isub__(self,other)` | 定义赋值减法的行为 | x-=y
 `__imul__(self,other)` | 定义赋值乘法的行为 | x*=y
 `__itruediv__(self,other)` | 定义赋值真除法的行为 | x/=y
 `__ifloordiv__(self,other)` | 定义赋值整数除法的行为 | x//=y
 `__imod__(self,other)` | 定义赋值取模算法的行为 | x%=y
 `__ipow__(self,other)` | 定义赋值幂运算的行为 | x**=y
 `__ilshift__(self,other)` | 定义赋值按位左移位的行为 | x<<=y
 `__irshift__(self,other)` | 定义赋值按位右移位的行为 | x>>=y
 `__iand__(self,other)` | 定义赋值按位与操作的行为 | x&=y
 `__ixor__(self,other)` | 定义赋值按位异或操作的行为 |  x^=y
 `__ior__(self,other)` | 定义赋值按位或操作的行为 | `x|=y`




## 反向运算

以` __radd()__`为例：

执行 x+y 时, Python 首先尝试调用左边对象的方法`x.__add__(y)` 。但这可能因为两个原因而失败。
1. 方法`x.__add__()` 没有实现.
2. 方法`x.__add__()` 已经实现，但是返回一个NotImplemented 值，表明数据类型不兼容。

如果失败了，Python 试图通过调用`y.__radd__(x)` 对右操作数进行反向加法 来解决这个问题

>如果反向加法被实现，Python 知道它不会遇到非交换性操作的潜在问题。如果它只是执行`y.__add__(x)` 而不是`x.__add__(y)` ，结果将是错误的，因为当定义为自定义操作时，该操作可能是非交换性的。这就是为什么需要y.__radd__(x) 。
>因此，`x.__add__(y)` 和`x.__radd__(y)` 之间的区别是，前者计算x + y ，而后者计算y + x  两者都调用各自定义在对象x 上的方法。


函数 | 说明 | 算式
---------|:----------|:---------:
 `__radd__(self,other)` | 定义加法的行为 |
 `__rsub__(self,other)` | 定义减法的行为 |
 `__rmul__(self,other)` | 定义乘法的行为 |
 `__rtruediv__(self,other)` | 定义真除法的行为 | 
 `__rfloordiv__(self,other)` | 定义整数除法的行为 | 
 `__rmod__(self,other)` | 定义取模算法的行为 |
 `__rdivmod__(self,other)` | 同时返回商，余数 | 
 `__rpow__(self,other)` | 定义幂运算的行为 |
 `__rlshift__(self,other)` | 定义按位左移位的行为 | 
 `__rrshift__(self,other)` | 定义按位右移位的行为 | 
 `__rand__(self,other)` | 定义按位与操作的行为 | 
 `__rxor__(self,other)` | 定义按位异或操作的行为 |
 `__ror__(self,other)` | 定义按位或操作的行为 | 


 ## 一元操作符

 函数 | 说明 | 算式
---------|:----------|:---------:
 `__pos__(self,other)` | 定义正号的行为 | +x
 `__neg__(self,other)` | 定义负号的行为 | -x
 `__abs__(self,other)` | 定义绝对值的行为 | `|x|`
 `__invert__(self,other)` | 定义按位求反的行为 | ~x

 

