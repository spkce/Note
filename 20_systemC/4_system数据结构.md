# 端口 sc_in, sc_out, sc_inout
定义方法： sc_in<数据类型> 端口名

端口的读写
```c++
sc_in<bool> clk；

clk.read() //读
clk.wirte() //写
```

端口用于指定模块间的连接方式：

```c++
template <class T>
class sc_in
: public sc_port<sc_signal_in_if<T>,1,SC_ONE_OR_MORE_BOUND>
```

# 信号 sc_signal
信号用于进程间通讯和连接模块实例

# sc_fifo
队列,默认深度为16

```c++
sc_core::sc_fifo<tlm::tlm_generic_payload *> m_fifo;

m_fifo.write(&payload);

tlm::tlm_generic_payload *t_payload = m_fifo.read();
```