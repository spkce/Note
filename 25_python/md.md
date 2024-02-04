# with as
如果你想要让你创建的对象能被 with 使用，那么你这个对象必须要有 __enter__ () 和 __exit__() 这两个方法。

as 后面就是得到__enter__的返回
```python
class Sample:
	def __enter__(self):
		return self
	def __exit__(self, type, value, trace):
		print ("type:", type)
		print ("value:", value)
		print ("trace:", trace)

	def do_something(self):
		bar = 1/0
		return bar + 10

with Sample() as sample:
  sample.do_something()
```
触发异常后__exit__会被调用：
```shell
type: <class 'ZeroDivisionError'>
value: division by zero
trace: <traceback object at 0x7f7dd2aa4748>
Traceback (most recent call last):
  File "module.py", line 17, in <module>
    sample.do_something()
  File "module.py", line 11, in do_something
    bar = 1/0
ZeroDivisionError: division by zero
```