# <center>面向对象设计原则</center>

## 三大基本特性：
### 封装
封装，就是把客观事物封装成抽象的类，并且类可以把自己的数据和方法只让可信的类或者对象操作，对不可信的进行信息隐藏。一个类就是一个封装了数据以及操作这些数据的代码的逻辑实体。在一个对象内部，某些代码或某些数据可以是私有的，不能被外界访问。通过这种方式，对象对内部数据提供了不同级别的保护，以防止程序中无关的部分意外的改变或错误的使用了对象的私有部分。

### 继承
继承，指可以让某个类型的对象获得另一个类型的对象的属性的方法。

### 多态
多态，是指一个类实例的相同方法在不同情形有不同表现形式。多态机制使具有不同内部结构的对象可以共享相同的外部接口。这意味着，虽然针对不同对象的具体操作不同，但通过一个公共的类，它们（那些操作）可以通过相同的方式予以调用.

多态的实现：方法的覆盖，是指子类重新定义父类的方法的做法

## 五大基本原则：
### 单一职责(Single Responsibility Principle, SRP)
定义：一个对象只专注于做一件事情

单一职责原则可以降低类的复杂度，一个类只负责一项职责，其逻辑肯定要比负责多项职责简单的多；提高类的可读性，提高系统的可维护性；变更引起的风险降低，变更是必然的，如果单一职责原则遵守的好，当修改一个功能时，可以显著降低对其他功能的影响。需要说明的一点是单一职责原则不只是面向对象编程思想所特有的，只要是模块化的程序设计，都适用单一职责原则。

### 开放封闭(Open－Close Principle, OCP)
定义：对修改关闭，对扩展开放

开放封闭原则主要体现在对扩展开放、对修改封闭，意味着有新的需求或变化时，可以对现有代码进行扩展，以适应新的情况。软件需求总是变化的，世界上没有一个软件的是不变的，因此对软件设计人员来说，必须在不需要对原有系统进行修改的情况下，实现灵活的系统扩展

例： 代码中有已完成的函数Draw，画一个边长为l的正方形

```c++
//画一个边长为l的正方形
bool Draw(int l);
```

后续需求变更：要求除了正方形外也能画矩形。 此时若对原有函数进行修改修改，对调用的代码也需要同步修改。因此直接增加一个画矩形的函数。
```c++
//画一个边长为l的正方形
bool Draw(int l);
//画一个宽为w的，高为h矩形
bool Draw(int w, int h);
```
开放封闭可以说有两个要点：1. 需求变更时不应该修改而是扩展；2. 代码的结构应该支持且方便扩展，以应对可能的需求变更。

### 里式替换(Liskov Substitution Principle, LSP)
定义：所有应用其基类的地方，必须都能透明的使用其子类的对象（基类可以完全替换子类）

在软件中将一个基类对象替换成它的子类对象，程序将不会产生任何错误和异常，反过来则不成立，如果一个软件实体使用的是一个子类对象的话，那么它不一定能够使用基类对象。里氏替换原则是实现开闭原则的重要方式之一，由于使用基类对象的地方都可以使用子类对象，因此在程序中尽量使用基类类型来对对象进行定义，而在运行时再确定其子类类型，用子类对象来替换父类对象。

例： 基类`CShape`，派生出子类`CSquare`和`CRect`分别代表着正方形和矩形。

```c++
class CShape
{
	int Draw();
};

class CSquare : public CShape
{
	int Draw();
};

class CRect : public CShape
{
	int Draw();
};


int main()
{
	CShape * a = new CSquare();
	a.Draw();

	CShape * b = new CRect();
	b.Draw();
}
```

### 依赖倒置(Dependency Inversion Principle, DIP)
定义：模块间的依赖通过抽象发生，实现类之间不发生直接的依赖关系，其依赖关系是通过接口或者抽象类产生的；(模块间要通过抽象接口隔离开，而不是通过具体的类强耦合起来)

若A模块依赖B模块实现，一旦B模块修改实现代码（因为需求的不断变更，这往往是经常发生的，而修改接口的情况往往较少）A模块很可能必须同时修改。而若是A模块依赖B模块的接口，B模块代码的修改不会影响A模块

例： 接口基类`CShape`，派生出子类`CSquare`和`CRect`分别代表着正方形和矩形。围栏类`CFence`应该使用接口而不是具体的子类

```c++
class IShape
{
protected:
	virtual int Draw() = 0;
};

class CSquare : public CShape
{
public:
	int Draw();
};

class CRect : public CShape
{
public:
	int Draw();
};

//围栏类应该依赖IShape接口类，而不应该依赖具体类CSquare、和CRect
class CFence
{
	IShape* m_area;
};

```

### 接口隔离(Interface Segregation Principle, ISP)
定义：客户端不应该依赖它不需要的接口，也可以说是类间的依赖要建立在最小的接口的基础上进行。

为各个类建立专用的接口，而不要试图去建立一个很庞大的接口供所有依赖它的类去调用。依赖几个专用的接口要比依赖一个综合的接口更灵活。可以理解接口隔离是为了减少接口的修改

例： 接口基类`CShape`，有Draw方法和set方法。`CScreen` 和 `CFence` 分别只调用Draw和set。不应该出现setandDraw的函数

```c++
class IShape
{
protected:
	virtual int Draw() = 0; // 画图形
	virtual int set() = 0; // 设置图形
};

class CScreen
{
	bool setFence()
	{
		p->Draw()
	}

	IShape* p;
};

class CFence
{
	bool setFence()
	{
		p->set()
	}

	IShape* p;
}

```

### 迪米特法则 (Law of Demeter, LOD)
定义：一个对象应该对其他对象有最少的了解, 也称最少知道原则。**（尽最大的努力隐藏自己!!!）**

一个类尽最大努力减少对其他类的耦合，同时被依赖的类也应该将一些方法最少的公开。知道的越少，相互之间的关心的就越少，耦合也就越小。如果有一天，被依赖类要修改，那么类间的牵连就越少，扩展越容易。

具体操作：对于C，函数能不写到头文件中就不写到头文件中，函数能加static就加static，依赖的头文件越少越好；对于C++，成员函数权限越小越好，避免使用友元，可用class 时不用 struct。

## 设计模式
设计模式只提一下比较常用的

### 单键模式（Singleton Pattern）

保证唯一的实例

单键的写法：
```c++
class CSingleton
{
public:
	static CSingleton* instance()
	{
		static CSingleton Inst;
		return &Inst;
	}
private:
	CSingleton();
	~CSingleton();
```

```c++
class CSingleton
{
public:
	static CSingleton* instance()
	{
		static CSingleton* pInstance = NULL;
		if (pInstance == NULL)
		{
			static Infra::CMutex sm_mutex;
			Infra::CGuard<Infra::CMutex> guard(sm_mutex);
			if (pInstance == NULL)
			{
				pInstance = new CSingleton;
			}
		}
		return pInstance;
	}
private:
	CSingleton();
	~CSingleton();
```

单键宏：
```c++
#undef PATTERN_SINGLETON_DECLARE
#define PATTERN_SINGLETON_DECLARE(classname)	\
public:											\
	static classname* instance();				\
private:										\
	classname();								\
	~classname();								\
	friend class std::auto_ptr<classname>;		\


#undef PATTERM_SINGLETON_IMPLEMENT
#define PATTERM_SINGLETON_IMPLEMENT(classname)	\
static std::auto_ptr<classname>	instance##classname(NULL);	\
void exit##classname(void)						\
{												\
	instance##classname = std::auto_ptr<classname>(NULL);	\
}												\
classname * classname::instance()						\
{												\
	if (NULL == instance##classname.get())		\
	{											\
		static Infra::CMutex sm_mutex;			\
		Infra::CGuard<Infra::CMutex> guard(sm_mutex);		\
		if (NULL == instance##classname.get())	\
		{										\
			instance##classname = std::auto_ptr<classname>(new classname);	\
			if(0 != atexit(exit##classname))	\
			{									\
				printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);	\
			}									\
		}										\
	}											\
	return instance##classname.get();			\
}												\
```

### 工厂模式（Factory Pattern）

![avatar][Image0]

```c++
class IShape
{

};

class CSquare : public CShape
{

};

class CRect : public CShape
{

};

class CShapeFactory()
{
public:
	static IShape* getShape(int type)
	{
		if (type == 0)
		{
			return new CSquare()
		}
		else
		{
			return new CRect()
		}
	}
}

int main()
{
	IShape* p = CShapeFactory::getShape(0);
}
```
### 观察者模式（Factory Pattern）
多个对象间存在一对多的依赖关系，当一个对象的状态发生改变时，所有依赖于它的对象都得到通知并被自动更新。又称作订阅-发布模式
```c++
//infra 库中观察者模板
template <typename P1, typename P2, typename P3, typename P4>
class CWatcher;

/**
* @brief 观察者模板类
*/
template <typename P1, typename P2, typename P3, typename P4>
class CObsrv
{
public:
	/**
	* @brief 观察者回调函数
	*/
	typedef TFuncation4<void, P1, P2, P3, P4> watch_t;

public:
	/**
	* @brief 构造函数
	* @param maxWather : 最大支持注册的观察者
	* @param maxParam : 最大参数队列
	*/
	CObsrv(int maxWather, int maxParam);
	virtual ~CObsrv();

	/**
	* @brief 观察者回调函数注册
	* @param fun : 回调函数
	* @return 成功/失败
	*/
	bool attach(const watch_t & fun);

	/**
	* @brief 观察者回调函数注销
	* @param fun : 回调函数
	* @return 成功/失败
	*/
	bool detach(const watch_t & fun);

	/**
	* @brief 通知观察者
	* @param  : 通知参数
	*/
	void distribute(P1 p1, P2 p2, P3 p3, P4 p4);

private:
	const int m_maxWather;
	const int m_maxParam;
	CMutex m_mutex;
	std::vector<CWatcher<P1, P2, P3, P4>*> m_vWatcher;
};

template <typename P1, typename P2, typename P3, typename P4>
class CWatcher
{
private:
	struct stParam
	{
		stParam(P1 _p1, P2 _p2, P3 _p3, P4 _p4)
		:p1(_p1),p2(_p2),p3(_p3),p4(_p4)
		{}
		stParam(const stParam & o)
		{
			p1 = o.p1;
			p2 = o.p2;
			p3 = o.p3;
			p4 = o.p4;
		}
		P1 p1;
		P2 p2; 
		P3 p3; 
		P4 p4;
	};
public:
	static CWatcher* create(const typename CObsrv<P1, P2, P3, P4>::watch_t & fun, int maxParam)
	{
		return new CWatcher(fun, maxParam);
	}

	void destory()
	{
		while (m_isProcessing)
		{
			
		}
		
		delete this;
	}

private:
	CWatcher(const typename CObsrv<P1, P2, P3, P4>::watch_t & fun, int maxParam)
	:m_maxParam(maxParam), m_mutex(), m_proc(fun),m_isProcessing(false)
	{}

	~CWatcher()
	{

	}

	static void* dispose(void* arg)
	{
		CWatcher* p = (CWatcher*)arg;
		p->m_isProcessing = true;

		p->m_mutex.lock();
		while (p->m_qParam.size() > 0)
		{
			struct stParam stp = p->m_qParam.front();
			p->m_qParam.pop();
			p->m_mutex.unlock();
			if (!p->m_proc.isEmpty())
			{
				p->m_proc(stp.p1, stp.p2, stp.p3, stp.p4);
			}
			p->m_mutex.lock();
		}
		p->m_mutex.unlock();
		p->m_isProcessing = false;
		return NULL;
	}

public:
	bool operator==(const typename CObsrv<P1, P2, P3, P4>::watch_t & fun)
	{
		return m_proc == fun;
	}
	
	bool isRun()
	{
		return m_isProcessing;
	}

	bool run(P1 p1, P2 p2, P3 p3, P4 p4)
	{
		if(m_proc.isEmpty())
		{
			return false;
		}

		m_mutex.lock();
		while (m_qParam.size() >= (size_t)m_maxParam)
		{
			m_qParam.pop();
		}

		m_qParam.push(stParam(p1, p2, p3, p4));
		m_mutex.unlock();

		if(!m_isProcessing)
		{
			int err = pthread_create(&m_handle, NULL, (void*(*)(void*))&CWatcher::dispose, (void*)this);
			if (err)
			{
				//线程创建失败
				InfraTrace("create pthread error: %d\n",err);
				return false;
			}

			//设置线程为可分离状态，线程运行结束后会自动释放资源。
			if (pthread_detach(m_handle))
			{
				InfraTrace("detach pthread error: %d\n",err);
				return false;
			}
		}
		return true;
	}

private:
	const int m_maxParam;
	Infra::CMutex m_mutex;
	std::queue<struct stParam> m_qParam;
	typename CObsrv<P1, P2, P3, P4>::watch_t m_proc;
	pthread_t m_handle;
	bool m_isProcessing;

};

template <typename P1, typename P2, typename P3, typename P4>
CObsrv<P1, P2, P3, P4>::CObsrv(int maxWather, int maxParam)
:m_maxWather(maxWather)
,m_maxParam(maxParam)
{
}

template <typename P1, typename P2, typename P3, typename P4>
CObsrv<P1, P2, P3, P4>::~CObsrv()
{
}

template <typename P1, typename P2, typename P3, typename P4>
bool CObsrv<P1, P2, P3, P4>::attach(const watch_t & fun)
{
	CGuard<CMutex> guard(m_mutex);
	if (m_vWatcher.size() >= (size_t)m_maxWather)
	{
		return false;
	}

	for (typename std::vector<CWatcher<P1, P2, P3, P4>*>::iterator it = m_vWatcher.begin(); it != m_vWatcher.end(); it++)
	{
		if (*(*it) == fun)
		{
			return false;
		}
	}

	CWatcher<P1, P2, P3, P4>* p = CWatcher<P1, P2, P3, P4>::create(fun, m_maxParam);
	m_vWatcher.push_back(p);
	return true;
}

template <typename P1, typename P2, typename P3, typename P4>
bool CObsrv<P1, P2, P3, P4>::detach(const watch_t & fun)
{
	CGuard<CMutex> guard(m_mutex);
	for (typename std::vector<CWatcher<P1, P2, P3, P4>*>::iterator it = m_vWatcher.begin(); it != m_vWatcher.end(); it++)
	{
		CWatcher<P1, P2, P3, P4> & watcher = (*(*it));
		if (watcher == fun && !watcher.isRun())
		{
			watcher.destory();
			m_vWatcher.erase(it);
			return true;
		}
	}
	return false;
}

template <typename P1, typename P2, typename P3, typename P4>
void CObsrv<P1, P2, P3, P4>::distribute(P1 p1, P2 p2, P3 p3, P4 p4)
{
	CGuard<CMutex> guard(m_mutex);
	for (typename std::vector<CWatcher<P1, P2, P3, P4>*>::iterator it = m_vWatcher.begin(); it != m_vWatcher.end(); it++)
	{
		CWatcher<P1, P2, P3, P4> & watcher = (*(*it));
		watcher.run(p1, p2, p3, p4);
	}
}

```

[Image0]:data:image/png;base64,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