# 指定目标架构
ARCH := arm64

# 指定交叉编译器前缀
CROSS_COMPILE := aarch64-none-linux-gnu-


# 指定内核源码路径
KDIR := /home/y0371/source/linux-5.10.116/build

# 指定当前模块源码路径
PWD := $(shell pwd)

# 指定要编译的模块
obj-m += iic.o 

# 默认编译目标
default:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules

# 清理目标
clean:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) clean

# 安装模块
install:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(PWD) modules_install

.PHONY: default clean install