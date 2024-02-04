make VERBOSE=1 打印编译过程详细信息

gcc -c *.c 编译当前目录下的 .c 不链接  生成 *.c

ar rcs lib.a *.o  将当前目录下的 .o文件链接成 lib.a


-rpath = dir
      将目录添加到运行时库搜索路径。
      在将ELF可执行文件与共享库链接时使用。所有-rpath
      参数都被串联并传递到运行时链接程序，该链接程序
      使用它们在运行时定位共享对象。

-rpath和-rpath-link都可以在链接时指定库的路径；但是运行时rpath-link指定的路径就不再有效（链接器没有将库的路径包含进可执行文件中），而-rpath指定的路径还有效（因为链接器已经将库的路径包含在可执行文件中了)

获取gcc 的版本
```shell
GCC_VER ?=$(shell gcc -dumpversion | cut -d. -f1,2)
```


编译子目录：
```Makefile
SRC_DIR := $(shell ls -l | grep ^d | awk '{if($$9 != "obj") print $$9}')
$(SRC_DIR) : ECHO #must have ECHO
	@echo "-> $@"
	@$(MAKE) <...> -C $@

ECHO:
	@echo "library : "$(SRC_DIR)
```


```Makefile

SRC_DIR := $(shell ls -l | grep ^d | awk '{if($$9 != "obj") print $$9}')
SRC_DIR += ./
CPP_SRCS := $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.cpp))
CPP_OBJS :=  $(patsubst %.cpp, %.o, $(CPP_SRCS))
CPPOBJS := $(addprefix $(OBJS_PATH)/, $(CPP_OBJS))

$(CPPOBJS) : $(DIR_OBJ)/%.o: %.cpp
	@echo "CXX $(notdir $@)"
	@mkdir -p $(dir $(@))
	@$(CXX) -o $(DIR_OBJ)/$(notdir $@) -c $< $(CFLAGS) $(INC)
```