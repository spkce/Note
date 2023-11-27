# initiator_socket和target_socket
```
tlm_utils::simple_initiator_socket<(module_name)> 
tlm_utils::simple_target_socket<(module_name)>
```
信息有blocking 和 non blocking transport的区别。

## blocking
在blocking方式时，target模块需要注册回调函数。initiator模块调用b_transport后，target模块注册的回调函数同时也被调用
```c++
#include "systemc.h"
#include "tlm.h"
#include "simple_initiator_socket.h"
#include "simple_target_socket.h"

class Initiator : public sc_module
{
public:
	SC_HAS_PROCESS(Initiator);
	Initiator(sc_module_name name) : sc_module(name)
	{
		SC_THREAD(hello_ic);
	}

public:
	tlm_utils::simple_initiator_socket<Initiator> init_socket;

private:
	void hello_ic()
	{
		auto *payload = new tlm::tlm_generic_payload();
		assert(payload != nullptr);
		const char *data{"Hello world"};
		payload->set_data_ptr((unsigned char *)data);
		sc_time delay = SC_ZERO_TIME;
		init_socket->b_transport(*payload, delay);
		delete payload;
	}
};

class Target : public sc_module
{
public:
	SC_HAS_PROCESS(Target);
	Target(sc_module_name name) : sc_module(name)
	{
		targ_socket.register_b_transport(this, &Target::hello_ic);
	}

public:
	tlm_utils::simple_target_socket<Target> targ_socket;

private:
	void hello_ic(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay_time)
	{
		const char *data = reinterpret_cast<const char *>(payload.get_data_ptr());
		std::cout << data << std::endl;
		return ;
	}
};

int sc_main(int argc, char *argv[])
{
	Initiator init("Initiator");
	Target targ("Target");
	init.init_socket(targ.targ_socket);
	sc_start(10, SC_NS);
	return 0;
}
```

## non blocking
实际上调用时候是阻塞的.
```c++
#include "systemc.h"
#include "tlm.h"
#include "simple_initiator_socket.h"
#include "simple_target_socket.h"

class Initiator : public sc_module
{
public:
	SC_HAS_PROCESS(Initiator);
	Initiator(sc_module_name name) : sc_module(name)
	{
		SC_THREAD(thread1);
		//sensitive
	}

public:
	tlm_utils::simple_initiator_socket<Initiator> init_socket;

private:
	void thread1()
	{
		auto *payload = new tlm::tlm_generic_payload();
		assert(payload != nullptr);
		const char *data{"Hello world"};
		payload->set_data_ptr((unsigned char *)data);
		
		tlm::tlm_phase phase = tlm::BEGIN_REQ;
		sc_time delay = SC_ZERO_TIME;

		init_socket->nb_transport_fw(*payload, phase, delay);
		delete payload;
	}
};

class Target : public sc_module
{
public:
	SC_HAS_PROCESS(Target);
	Target(sc_module_name name) : sc_module(name)
	{
		targ_socket.register_nb_transport_fw(this, &Target::callback);
	}

public:
	tlm_utils::simple_target_socket<Target> targ_socket;

private:
	tlm::tlm_sync_enum callback(tlm::tlm_generic_payload & payload, tlm::tlm_phase& phase, sc_core::sc_time &delay)
	{
		const char *data = reinterpret_cast<const char *>(payload.get_data_ptr());
		return tlm::TLM_ACCEPTED;
	}
};

int sc_main(int argc, char *argv[])
{
	Initiator init("Initiator");
	Target targ("Target");
	init.init_socket(targ.targ_socket);
	sc_start(10, SC_NS);
	return 0;
}

```

一般用 sc_core::sc_event等， 配合实现非阻塞调用callback中仅仅用于发送事件，接收方再接收

## tlm_generic_payload
用于数据传输
可以理解tlm_generic_payload仅仅是封装了一些基本的数据传输情况

支持几个类型
```c++
//传输一块内存（data）到addr上
tlm_generic_payload* trans = new tlm_generic_payload;
trans->set_command(TLM_WRITE_COMMAND);
trans->set_address(addr);
trans->set_data_ptr(data);
trans->set_data_length(size);
```


