```c++
#include <cxxabi.h>

std::string demangle(const std::string & name)
{
	char buffer[1024] = {0};  
	size_t size = sizeof(buffer);  
	int status;  
	char *ret;

	try {
		char* ret = abi::__cxa_demangle(name.c_str(), buffer, &size, &status);
		if(ret) 
		{
			return std::move(std::string(ret));
		}
		else
		{
			return name;
		}
	} catch(...) {
		return name;
	}
}
```