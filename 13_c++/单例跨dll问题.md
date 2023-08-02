
```c++

template<typename T>
class Singleton
{
public:
	static T& instance()
	{
		static T inst;
		return inst;
	}
}

```


static变量的唯一性是动态库级别的，不同库包含同一截代码的话就会有多份static实例。
出现问题的情况往往是，这截代码是实现在头文件里的，所以被不同模块引用时就生成了多份代码。

解决办法就是按照一般DLL导出接口的规则，在获取static变量的地方：对于实现单例的dll，使用dllexport。

1. 对于实现单例的dll，使用dllexport。对于MSVC来说就是__declspec(dllexport)，对于GCC/clang就是__attribute__ ((dllexport))
2. 对于引用单例的dll，使用dllimport。对于MSVC/clang来说就是__declspec(dllimport)，对于GCC/clang就是__attribute__ ((dllimport))


