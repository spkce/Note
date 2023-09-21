全加器
```c++
#include "systemc.h"

SC_MODULE(half_adder)
{
public:
	sc_in<bool> a;
	sc_in<bool> b;
	sc_out<bool> sum;
	sc_out<bool> carry;

	SC_CTOR(half_adder)
	{
		SC_METHOD(prc_half_adder);
		sensitive<<a <<b;
	}
	void prc_half_adder()
	{
		sum = a^b;
		carry = a & b;
	}
};

SC_MODULE(full_adder)
{
public:
	sc_in<bool> a, b, carry_in;
	sc_out<bool> sum, carry_out;
	sc_signal<bool> c1,s1,c2;

	half_adder *ha1_ptr, *ha2_ptr;

	SC_CTOR(full_adder)
	{
		ha1_ptr = new half_adder("ha1");
		ha1_ptr->a(a);
		ha1_ptr->b(b);
		ha1_ptr->sum(s1);
		ha1_ptr->carry(c1);

		ha2_ptr = new half_adder("ha2");
		(*ha2_ptr)(s1,carry_in,sum,c2);

		SC_METHOD(prc_or);
		sensitive<<c1 <<c2;
	}

	~full_adder()
	{
		delete ha1_ptr;
		delete ha2_ptr;
	}

	void prc_or()
	{
		carry_out = (bool)(c1 | c2);
	}
};

SC_MODULE(driver)
{
public:
	sc_out<bool> d_a,d_b,d_cin;
	SC_CTOR(driver)
	{
		SC_THREAD(prc_driver);
	}

	void prc_driver()
	{
		sc_uint<3> pattern;
		pattern = 0;

		while (1)
		{
			d_a = pattern[0];
			d_b = pattern[1];
			d_cin = pattern[2];

			wait(5, SC_NS);
			pattern++;
		}
		
	}
};

SC_MODULE(monitor)
{
public:
	sc_in<bool> m_a,m_b,m_cin,m_sum,m_cout;

	SC_CTOR(monitor)
	{
		SC_METHOD(prc_monitor);
		sensitive<<m_a <<m_b<<m_cin<<m_sum<<m_cout;
	}
	void prc_monitor()
	{
		std::cout<< "At time" << sc_time_stamp() << "::";
		std::cout<< "(a,b,carry_in):"<<m_a << m_b << m_cin;
		std::cout<< "(sum,carry_out):"<<m_sum << m_cout<<std::endl;
	}
};

int sc_main(int i, char* a[]) {
	sc_signal<bool> t_a,t_b,t_cin,t_sum,t_cout;

	full_adder f1("fullAdder");
	f1 << t_a << t_b << t_cin << t_sum << t_cout;

	driver d1("GenerateWaveforms");
	d1.d_a(t_a);
	d1.d_b(t_b);
	d1.d_cin(t_cin);

	monitor mo1("MonitorWaveForms");
	mo1 << t_a << t_b << t_cin <<t_sum<< t_cout;

	sc_start(100, SC_NS);

	sc_core::sc_report_handler::set_actions( "/IEEE_Std_1666/deprecated", sc_core::SC_DO_NOTHING );
	return 0;
}

```

运行结果：
```shell
At time0 s::(a,b,carry_in):000(sum,carry_out):00
At time5 ns::(a,b,carry_in):100(sum,carry_out):00
At time5 ns::(a,b,carry_in):100(sum,carry_out):10
At time10 ns::(a,b,carry_in):010(sum,carry_out):10
At time15 ns::(a,b,carry_in):110(sum,carry_out):10
At time15 ns::(a,b,carry_in):110(sum,carry_out):01
At time20 ns::(a,b,carry_in):001(sum,carry_out):01
At time20 ns::(a,b,carry_in):001(sum,carry_out):11
At time20 ns::(a,b,carry_in):001(sum,carry_out):10
At time25 ns::(a,b,carry_in):101(sum,carry_out):10
At time25 ns::(a,b,carry_in):101(sum,carry_out):00
At time25 ns::(a,b,carry_in):101(sum,carry_out):01
At time30 ns::(a,b,carry_in):011(sum,carry_out):01
At time35 ns::(a,b,carry_in):111(sum,carry_out):01
At time35 ns::(a,b,carry_in):111(sum,carry_out):11
At time40 ns::(a,b,carry_in):000(sum,carry_out):11
At time40 ns::(a,b,carry_in):000(sum,carry_out):01
At time40 ns::(a,b,carry_in):000(sum,carry_out):00
At time45 ns::(a,b,carry_in):100(sum,carry_out):00
At time45 ns::(a,b,carry_in):100(sum,carry_out):10
At time50 ns::(a,b,carry_in):010(sum,carry_out):10
At time55 ns::(a,b,carry_in):110(sum,carry_out):10
At time55 ns::(a,b,carry_in):110(sum,carry_out):01
At time60 ns::(a,b,carry_in):001(sum,carry_out):01
At time60 ns::(a,b,carry_in):001(sum,carry_out):11
At time60 ns::(a,b,carry_in):001(sum,carry_out):10
At time65 ns::(a,b,carry_in):101(sum,carry_out):10
At time65 ns::(a,b,carry_in):101(sum,carry_out):00
At time65 ns::(a,b,carry_in):101(sum,carry_out):01
At time70 ns::(a,b,carry_in):011(sum,carry_out):01
At time75 ns::(a,b,carry_in):111(sum,carry_out):01
At time75 ns::(a,b,carry_in):111(sum,carry_out):11
At time80 ns::(a,b,carry_in):000(sum,carry_out):11
At time80 ns::(a,b,carry_in):000(sum,carry_out):01
At time80 ns::(a,b,carry_in):000(sum,carry_out):00
At time85 ns::(a,b,carry_in):100(sum,carry_out):00
At time85 ns::(a,b,carry_in):100(sum,carry_out):10
At time90 ns::(a,b,carry_in):010(sum,carry_out):10
At time95 ns::(a,b,carry_in):110(sum,carry_out):10
At time95 ns::(a,b,carry_in):110(sum,carry_out):01
```