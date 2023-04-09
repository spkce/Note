# sc_fifo
队列,默认深度为16

```c++
sc_core::sc_fifo<tlm::tlm_generic_payload *> m_fifo;

m_fifo.write(&payload);

tlm::tlm_generic_payload *t_payload = m_fifo.read();
```