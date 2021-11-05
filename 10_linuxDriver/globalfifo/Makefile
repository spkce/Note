   
ifneq ($(KERNELRELEASE),)
#kernal module
obj-m += globalfifo.o
ccflags-y := -std=gnu99 -Wno-declaration-after-statement
#调试信息
#EXTRA_CFLAGS=-G -O0
else
KERNAL_VER = $(shell uname -r)

#内核头文件路径
KDIR := /lib/modules/$(KERNAL_VER)/build

all:
	make -C $(KDIR) M=$(shell pwd) modules
clean:
	rm -f *.ko *.o *.mod.o *mod.c *.symvers *.order *.cmd

endif