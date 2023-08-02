### 1. cin

cin会被空格截断，若需要空格可以
```c++
char str[100];
cin.getline(str, sizeof(str));
```


一个进程的子线程创建一个子进程。若子线程退出，会发送信号给子进程（若子进程 prctl(PR_SET_PDEATHSIG, SIGINT)）



对于服务器而言，若创建会话套接字后，进程结束后，若该会话（server->client）为TCP状态为time_wait。则会让该端口再次绑定时失败.
而（client->server）会话为time_wait则不会


进程正常退出时，全局对象会被析构，静态局部对象也会析构。但是new出来的对象不会创建.
进程不正常退出时(被kill)，不会执行析构

实际测试atexit可以注册超过32个
```c++
void func0(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func1(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func2(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func3(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func4(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func5(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func6(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func7(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func8(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func9(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func10(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func11(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func12(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func13(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func14(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func15(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func16(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func17(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func18(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func19(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func20(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func21(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func22(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func23(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func24(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func25(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func26(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func27(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func28(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func29(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func30(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func31(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func32(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
void func33(){printf("\033[0;35m""%s:%d %s""\033[0m\n", __FILE__, __LINE__, __func__);}
			
int main()
{
	if(0 != atexit(func0)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func1)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func2)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func3)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func4)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func5)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func6)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func7)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func8)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func9)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func10)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func11)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func12)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func13)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func14)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func15)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func16)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func17)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func18)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func19)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func20)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func21)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func22)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func23)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func24)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func25)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func26)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func27)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func28)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func29)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func30)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func31)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func32)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	if(0 != atexit(func33)) {printf("%s:%s:%d atexit faild\n",__FILE__,__FUNCTION__,__LINE__);}
	return 0;
}
```


## atexit先于全局变量析构函数执行


## std::string::c_str() 和 const char* 的坑

std::string::c_str() 给出的指针会在对象析构后不可用。因此
substr().c_str()要小心。substr()会产生一样临时变量

## for (auto: ) 的坑

auto可能有坑，如下：
```
for(auto a : <int>[])
{
	......
}
```
auto 可能是int 而不是 int &， 在for中 a = 0；不会改变数组的值



2. std::swap
一下代码会出现double free
```c++

class CA 
{
public:
	CA()
	{
		printf("\033[0;35m""create :%p ""\033[0m\n", this);
		m_p = new char[32];
		memset(m_p, 0, 32);
	}

	~CA() 
	{
		if (m_p)
		{
			printf("\033[0;35m""this %p, delete:%p ""\033[0m\n", this, m_p);
			delete [] m_p;
			m_p = nullptr;
		}
	}

	char* m_p;
};

int main(int argc, char **argv)
{

	CA a1; 
	CA a2;

	memcpy(a1.m_p, "eeee", sizeof("eeee"));
	memcpy(a2.m_p, "ssss", sizeof("ssss"));
	
	printf("\033[0;35m""a1:%p,p:%p = %s ""\033[0m\n", &a1, a1.m_p, a1.m_p);
	printf("\033[0;35m""a2:%p,p:%p = %s""\033[0m\n", &a2, a2.m_p, a2.m_p);

	std::swap(a1, a2);

	printf("\033[0;35m""a1:%p,p:%p = %s""\033[0m\n", &a1, a1.m_p, a1.m_p);
	printf("\033[0;35m""a2:%p,p:%p = %s""\033[0m\n", &a2, a2.m_p, a2.m_p);

}
```
运行后：
```shell
create :0x7fffee4c4a98
create :0x7fffee4c4aa0
a1:0x7fffee4c4a98,p:0x7fffe5954ec0 = eeee
a2:0x7fffee4c4aa0,p:0x7fffe5954ef0 = ssss
this 0x7fffee4c4a60, delete:0x7fffe5954ec0
a1:0x7fffee4c4a98,p:0x7fffe5954ef0 = ssss
a2:0x7fffee4c4aa0,p:0x7fffe5954ec0 =
this 0x7fffee4c4a98, delete:0x7fffe5954ef0
this 0x7fffee4c4aa0, delete:0x7fffe5954ec0
free(): double free detected in tcache 2
Aborted (core dumped)
```

这是因为在std::swap中会产生一个零时变量,零时变量析构的时候删除了指针
```c++
template<typename T>
void swap(T& a, T& b){
  T temp(a);
  a = b;
  b = temp;
}
```

办法是特殊化swap
```shell
namespace std
{
//特殊化的std::swap，当T是Widget类型时使用如下实现
template<>
void swap<CA>(CA& a, CA& b)
{
    swap(a.m_p, b.m_p);
}
}
```

3. 库
首先是泛用库boost，界面库Qt，通信库ZeroMQ，视觉库OpenCV，并发库tbb，深度学习Caffe，序列化Protobuf，加上一些cppformat之类的小库。然后就是http客户端curl，http服务器 crow，gzip压缩zlib，json序列化nlohmann/json，二进制序列化protobuf，嵌入式数据库sqlite，日志库glog，参数解析库gflags，消息队列zmq，rpc库brpc，tcp网络库evpp，3d仿真osg，图形图像opencv，stl,boost,qt上面都讲过了。
