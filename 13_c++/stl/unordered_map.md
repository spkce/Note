# 1. 自定义类做unordered_map的key值

有如下程序：
```c++
class CA
{
public:
	CA(int value, const std::string & name)
	:m_value(value)
	,m_name(name)
	{
	}
	virtual ~CA() {}
private:
	int m_value;
	std::string m_name;
};

int main(int argc, char const *argv[])
{
	std::unordered_map<CA, int> m_umap;
	m_umap[CA(1,"xxxx")] = 1;
	return 0;
}
```
编译会有报错：
```shell
b.cpp:33:30: error: use of deleted function ‘std::unordered_map<_Key, _Tp, _Hash, _Pred, _Alloc>::unordered_map() [with _Key = CA; _Tp = int; _Hash = std::hash<CA>; _Pred = std::equal_to<CA>; _Alloc = std::allocator<std::pair<const CA, int> >]’
   33 |  std::unordered_map<CA, int> m_umap;
```

原因是需要CA 需要实现`operator==`函数以及获取hash值的函数
获取hash值函数有多种实现方法：

方案一：
```c++
class CA
{
public:
	CA(int value, const std::string & name)
	:m_value(value)
	,m_name(name)
	{
	}
	virtual ~CA() {}
	
	bool operator==(const CA & o) const 
	{
		return m_name == o.m_name && m_value == o.m_value;
	}
public:
	int m_value;
	std::string m_name;
};

struct CA_hash{
	size_t operator()(const CA & o) const{
		return std::hash<std::string>()(o.m_name) ^ std::hash<int>()(o.m_value);
	}
};
```

方案二：
`operator==`可以改为`equal_to`
```c++
class CA
{
public:
	CA(int value, const std::string & name)
	:m_value(value)
	,m_name(name)
	{
	}
	virtual ~CA() {}
	
public:
	int m_value;
	std::string m_name;
};

namespace std
{
	template<>
	struct hash<CA>{//哈希的模板定制
	public:
		size_t operator()(const CA &o) const 
		{
			return std::hash<std::string>()(o.m_name) ^ std::hash<int>()(o.m_value);
		}
		
	};

	template<>
	struct equal_to<CA>{//等比的模板定制
	public:
		bool operator()(const CA &o1, const CA &o2) const
		{
			return o1.m_value == o2.m_value && o1.m_name == o2.m_name;
		}
		
	};
}
```
方案二也可以简化为：
```c++
template<>
struct std::hash<CA>{//哈希的模板定制
public:
	size_t operator()(const CA &o) const 
	{
		return std::hash<std::string>()(o.m_name) ^ std::hash<int>()(o.m_value);
	}
	
};
template<>
struct std::equal_to<CA>{//等比的模板定制
public:
	bool operator()(const CA &o1, const CA &o2) const
	{
		return o1.m_value == o2.m_value && o1.m_name == o2.m_name;
	}
	
};
```