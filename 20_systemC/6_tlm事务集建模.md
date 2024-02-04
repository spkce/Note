# 什么叫“事务” ？
事务的最确切定义为设计中给定的两个时间点内发生的被认为不可分割的活动 。 这个定义非常精确 ，却非常难于理解 。 在一个总线系统中 ，总线的一次读或者一次写操作通常会被认为是事务 ；在一个包通信系统中 ，发送一个包通常被认为是一个事务 。 从硬件的角度 ，一个事务是两个实体（进
程 、模块 、通道等）间的一次控制或者数据传输 。 从软件的角度 ，一次事务通常被理解为一个函数调用 。 该调用的结果通常是参数被传到被调用函数 ，返回值被传回到调用函数 ，该调用过程可能立刻返回 ，也可能因为发生阻塞 ，在后来的某一个时间点返回 。

# 接口、端口、通道
SystemC 定义了接口（Interface） 、端口（Port）和通道（Channel）
* 模块用端口和通道连接
* 通道上挂载多个模块
* 通道实现多个接口


## 接口sc_interface
所有接口的基类
```c++
class SC_API sc_interface
{
public:

    // register a port with this interface (does nothing by default)
    virtual void register_port( sc_port_base& port_,
				const char*    if_typename_ );

    // get the default event
    virtual const sc_event& default_event() const;

    // destructor (does nothing)
    virtual ~sc_interface();

protected:

    // constructor (does nothing)
    sc_interface();

private:

    // disabled
    sc_interface( const sc_interface& );
    sc_interface& operator = ( const sc_interface& );
};


void
sc_interface::register_port( sc_port_base&, const char* )
{}


// get the default event

const sc_event&
sc_interface::default_event() const
{
    SC_REPORT_WARNING( SC_ID_NO_DEFAULT_EVENT_, 0 );
    return sc_event::none;
}


// destructor (does nothing)

sc_interface::~sc_interface()
{}


// constructor (does nothing)

sc_interface::sc_interface()
{}
```
方法 virtual void register_port( sc_port_base& port_, const char* if_typename_ );完成通道绑定（Bind）时的静态规则检查 。 port_是接口所连接的端口名 ，if_typename_是接口名 。 缺省的情况下 ，这个函数不做任何事情 。

通道可以使用 default_event() 来返回静态敏感表的缺省事件 。 default_event()中调用的 SC_REPORT_WARNING( SC_ID_NO_DEFAULT_EVENT_, 0 )用于产生警告信息 。

## 端口 sc_port

SystemC 规定了不同的模块之间进行通信必须通过端口 。

```c++
template <class IF, int N = 1, sc_port_policy P=SC_ONE_OR_MORE_BOUND>
class sc_port
: public sc_port_b<IF>
{
    // typdefs

    typedef sc_port_b<IF> base_type;
    typedef sc_port<IF,N,P> this_type;

public:

    // constructors

    sc_port()
	: base_type( N, P )
	{}

    explicit sc_port( const char* name_ )
	: base_type( name_, N, P )
	{}

    explicit sc_port( IF& interface_ )
	: base_type( N, P )
	{ sc_warn_port_constructor(); base_type::bind( interface_ ); }

    sc_port( const char* name_, IF& interface_ )
	: base_type( name_, N, P )
	{ sc_warn_port_constructor(); base_type::bind( interface_ ); }

    explicit sc_port( base_type& parent_ )
	: base_type( N, P )
	{ sc_warn_port_constructor(); base_type::bind( parent_ ); }

    sc_port( const char* name_, base_type& parent_ )
	: base_type( name_, N, P )
	{ sc_warn_port_constructor(); base_type::bind( parent_ ); }

    sc_port( this_type& parent_ )
	: base_type( N, P )
	{ sc_warn_port_constructor(); base_type::bind( parent_ ); }

    sc_port( const char* name_, this_type& parent_ )
	: base_type( name_, N, P )
	{ sc_warn_port_constructor(); base_type::bind( parent_ ); }


    // destructor (does nothing)

    virtual ~sc_port()
	{}

    virtual const char* kind() const
        { return "sc_port"; }

private:

    // disabled
    sc_port( const this_type& );
    this_type& operator = ( const this_type& );
};
```
IF是端口所要连接的通道的接口类型, N代表端口要连接的最大通道数，默认为1.当连接数目未定时可以使用 ０ ，它代表一个端口可以连接无穷多个通道


假设我们已经定义了接口 ram＿if
```c++
sc_port<ram_if> ram_port１;／／连接到一个 RAM 上
sc_port<ram_if,N> ram_portN;／／可以连接到 N 个 RAM 上
sc_port<ram_if,０> ram_port０;／／不限制所连接的 RAM 数量
```


端口实例：
ram_if
```c++
#ifndef _MEM_IF_H
#define _MEM_IF_H
#include "systemc.h"

enum transfer_status
{
	TRANSFER_OK = 0,
	TRANSFER_ERROR
};
template <class T>
class mem_read_if : public sc_interface
{
public:
	virtual transfer_status read(unsigned int address, T &data) = 0;
};
template <class T>
class mem_write_if : public sc_interface
{
public:
	virtual transfer_status write(unsigned int address, T &data) = 0;
};

class reset_if : public sc_interface
{
public:
	virtual bool reset() = 0;
};
template <class T>
class ram_if : public mem_write_if<T>, mem_read_if<T>, reset_if
{
public:
	virtual unsigned int start_address() const = 0;
	virtual unsigned int end_address() const = 0;
};
#endif
```
定义一个通道ram
```c++
#define _RAM_H
#include "systemc.h"

template <class T>
class ram : public sc_module, ram_if<T>
{
public:
	ram(sc_module_name name_, unsigned int start_address, unsigned int end_address)
		: sc_module(name_), m_start_address(start_address), m_end_address(end_address)
	{
		sc_assert(end_address > = start_address);
		mem = new T[end_address - start_address];
	}
	~ram()
	{
		if (mem)
		{
			delete mem;
			mem = 0;
		}
	}
	transfer_status read(unsigned address, T &data)
	{
		if (address < m_start_address || address > m_end_address)
		{
			data = 0;
			return TRANSFER_ERROR;
		}
		data = mem[address - m_start_address];
		return TRANSFER_OK;
	}
	transfer_status write(unsigned address, T &data)
	{
		if (address < m_start_address || address > m_end_address)
		{
			return TRANSFER_ERROR;
		}
		mem[address - m_start_address] = data;

		return TRANSFER_OK;
	}
	bool reset()
	{
		for (int i = 0; i < (m_end_address - m_start_address); i++)
			mem[i] = (T)0;
		return true;
	}
	inline unsigned int start_address() const
	{
		return m_start_address;
	}
	inline unsigned int end_address() const
	{
		return m_end_address;
	}

private:
	T *mem;
	unsigned int m_start_address, m_end_address;
};
#endif
```
Master通过 ram_if访问 Ram
```c++
SC_MODULE(Master)
{
	sc_in_clk clk;
	sc_port<ram_if<int>> ram_port; //端口实例 
	void main_action;
	int data;
	unsigned int address;
	SC_CTOR()
	{
		SC_CTHREAD(main_action, clk.pos());
	}
};
void Master::main_action()
{
	wait();

	int i = 0;
	while (i++ < 100)
	{
		address = 0; // any valid address
		if (transfer_status status = ram_port->write(address, data))
		{
			//端口操作
			cout << "Write RAM at address" << address << "to RAM,Data is " << data << endl;
		}
		else
			cout << "RAM write fail, possibly caused by address out of rang" << endl;
		if (transfer_status status = ram_port->read(address, data))
		{
			cout << "Read RAM at address " << address < < "from RAM, Data is " << data << endl;
		}
		else
			cout << "RAM read fail, possibly caused by address out of range" <<endl;

		wait();
		address++;
	}
	sc_stop();
}

```

# 基本通道
`sc_signal<T> `
`sc_signal_rv<N>`
`sc_mutex`
`sc_fifo<T>`
`sc_semaphore`
`sc_buffer<T>`

## `sc_signal<T>` 

`sc_signal<T>` 是最基本的通道 ，它用于连接模块的基本端口`sc_in<T>`、`sc_out <T>` 和`sc_inout<T>`。

 这里强调一下`sc_signal<T>`的设计规则 ：最多只有一个 `sc_out <T>` 或者`sc_inout<T>` 可以连接到`sc_signal<T>`，否则就会产生典型的多驱动情况 ，`sc_signal<T>` 可能会在同一时刻被赋予不同的值 ，导致系统行为的不确定性 。 可以有多个`sc_in<T>` 同时连接到 `sc_signal<T>`，这将导致一个信号的值被赋给多个信号 ，这是允许的 ，也是应该的 。
