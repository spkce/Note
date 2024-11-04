# 平台总线模型

平台总线模型就是把原来的驱动 C 文件给分成了两个， 一个是 device.c(设备)， 一个是 driver.c(驱动)。把稳定不变的放在 driver.c 里面， 需要变得就放在了 device.c 里面。平台总线模型将设备代码和驱动代码分离， 将和硬件设备相关的都放到 device.c 文件里面，驱动部分代码都放到 driver.c 文件里面

在linux2.6以后的设备驱动模型中，需要关心总线、设备和驱动3个实体。总线将设备和驱动绑定。系统在注册一个设备时会去寻找与之匹配的驱动。相反系统每注册一个驱动都会去寻找相应的设备。匹配由总线完成


# platform_device结构体 （设备）
```c++
struct platform_device {
	const char *name; 	//platform 设备的名字， 用来和 platform 驱动相匹配。
	int id;	//ID 是用来区分如果设备名字相同的时候(通过在后面添加一个数字来代表不同的设备)
	bool id_auto;
	struct device dev;	//内置的 device 结构体
	u32 num_resources;	//资源结构体数量
	struct resource *resource;	//指向一个资源结构体数组
	const struct platform_device_id *id_entry;
	char *driver_override; /* Driver name to force a match */
	/* MFD cell pointer */
	 struct mfd_cell *mfd_cell;
	/* arch specific additions */
	struct pdev_archdata archdata;
};
```
其中，最重要的成员为 struct resouce *resource，下文platform设备资源和数据介绍

# platform_driver结构体 （驱动）
头文件`#include <linux/platform_device.h>`
```c++
struct platform_driver {
    int (*probe)(struct platform_device *);/*probe()函数，当“驱动”与“设备”匹配成功以后，probe()函数就会执行*/
    int (*remove)(struct platform_device *);
    void (*shutdown)(struct platform_device *);
    int (*suspend)(struct platform_device *, pm_message_t state);
    int (*resume)(struct platform_device *);
    struct device_driver  driver;
    const struct platform_device_id  *id_table; /*id_table是个表(也就是数组)，每个元素的类型为platfomm_device_id*/
    bool prevent_deferred_probe;
};
```

# bus_type 结构体 （总线）
```c++
struct bus_type {
    const char        *name;       /*总线名称*/
    const char        *dev_name;
    struct device        *dev_root;
    struct device_attribute    *dev_attrs;    /* use dev_groups instead */
    const struct attribute_group **bus_groups;   /*总线属性*/
    const struct attribute_group **dev_groups;   /*设备属性*/
    const struct attribute_group **drv_groups;   /*驱动属性*/
 
    int (*match)(struct device *dev, struct device_driver *drv);
    int (*uevent)(struct device *dev, struct kobj_uevent_env *env);
    int (*probe)(struct device *dev);
    int (*remove)(struct device *dev);
    void (*shutdown)(struct device *dev);
 
    int (*online)(struct device *dev);
    int (*offline)(struct device *dev);
 
    int (*suspend)(struct device *dev, pm_message_t state);
    int (*resume)(struct device *dev);
 
    const struct dev_pm_ops *pm;
 
    const struct iommu_ops *iommu_ops;
 
    struct subsys_private *p;
    struct lock_class_key lock_key;
};
```

重点在match函数，该函数确定了platform_device和platform_driver之间是如何匹配的

```c++
static int platform_match(struct device *dev, struct device_driver *drv)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct platform_driver *pdrv = to_platform_driver(drv);

	/*Attempt an OF style match first*/
	if (of_driver_match_device(dev, drv))
		return 1;
	
	/*Then try ACPI style match*/
	if (acpi_driver_match_device(dev, drv))
		return 1;
	
	/*Then try to match against the id table*/
	if (pdrv->id_table)
		return platform_match_id(pdrv->id_table, pdev) != NULL;
	
	/*fall-back to driver name match*/
	return (strcmp(pdec->name, drv->name) == 0);
}
```
匹配platform_device和platform_driver有4中可能：
1. 基于设备树风格的匹配
2. 基于ACPI风格的匹配
3. 匹配ID表(platform_device设备名是否出现在platform_driver的ID表内）
4. 匹配platform_device设备名和驱动名

linux 2.6 arm平台中：platform_device通常定义在BSP的板文件中实现，在板文件中platform_device通常归纳为一个数组，最终通过platform_add_device函数统一注册。
linux 3.x 之后根据设备树内容自动展开platform_device。

# 将 globalfifo移植为platform设备
globalfifo移植为platform设备，第一步需要加一个设备驱动壳：即实现platform_driver中remove和probe接口，并使用
`module_platform_driver`声明将驱动注册进platform平台中。
原来的模块的init函数，现在由globalfifo_probe完成
原来的模块的exit函数，现在由globalfifo_remove完成
```c++
static void globalfifo_setup_cdev(struct globalfifo_dev *dev,int index)
{
    int devno = MKDEV(globalfifo_major,index);
    printk(KERN_INFO "index is %d\n",index); //添加打印代码是不想编译时这个代码被优化掉，导致次设备号的设备打不开
    cdev_init(&dev->cdev ,&globalfifo_fops);
    printk(KERN_INFO "dev->cdev is %p\n",&dev->cdev);
    dev->cdev.owner = THIS_MODULE;
    int err = cdev_add(&dev->cdev ,devno, 1);
    if(err)
        printk(KERN_NOTICE "Error %d adding globalfifo%d",err,index);
}

static int globalfifo_probe(struct platform_device *pdev)
{
    int ret,i;
    dev_t devno = MKDEV(globalfifo_major,0);
    if (globalfifo_major)
        ret = register_chrdev_region(devno,1,"globalfifo");
    else{
        ret = alloc_chrdev_region(&devno,0,1,"globalfifo");
        globalfifo_major = MAJOR(devno);
    }

    if (ret < 0)
        return ret;
    
    globalfifo_devp = kzalloc(sizeof(struct globalfifo_dev) * DEVICE_NUM,GFP_KERNEL);
    if (!globalfifo_devp)
    {
        ret = -ENOMEM;
        goto fail_malloc;
    }
    

    mutex_init(&(globalfifo_devp+i)->globalfifo_mutex);
    init_waitqueue_head(&(globalfifo_devp+i)->r_wait);
    init_waitqueue_head(&(globalfifo_devp+i)->w_wait);
    globalfifo_setup_cdev(globalfifo_devp + i,i);
    
    return 0;
fail_malloc:
    unregister_chrdev_region(devno,DEVICE_NUM);
    return ret;
}


static int globalfifo_remove(struct platform_device *pdev)
{
    cdev_del(&(globalfifo_devp+i)->cdev);
    kfree(globalfifo_devp);
  
    unregister_chrdev_region(MKDEV(globalfifo_major,0),DEVICE_NUM);
    return 0;
}

static struct platform_driver globalfifo_driver = {
    .driver = {
        .name = "globalfifo",
        .owner = THIS_MODULE,
    },
    .probe = globalfifo_probe,
    .remove = globalfifo_remove,
};
module_platform_driver(globalfifo_driver);
MODULE_AUTHOR("spkce <spkce_crt@163.com>");
MODULE_LICENSE("GPL v2");
```
在dts中增加
```dts
globalfifo: globalfifo {
compatible = "globalfifo";
#address-cells = <1>
#size-cells = <1>
};
```
使用make dtbs 来单独编译设备树

再insmod 驱动就可以了
## 另外的方法（存疑）
或者直接在板文件arch/arm/mach-<soc名>/mach-<板名>.c中增加
```c++
static struct platform_device globalfifo_device = {
	.name = "globalfifo",
	.id = -1,
};
```
执行加载命令：
```shell
depmod //第一次加载驱动的时候需要运行此命令
modprobe device.ko //加载设备模块
modprobe driver.ko //加载驱动模块
```

若一切顺利， /sys/devices/platform目录下会开到一个globalfifo的子目录，/sys/devices/platform/globalfifo下会有一个driver文件指向 /sys/bus/platform/drivers/globalfifo的符号连接
# platform 设备资源和数据
platform_device中最重要的成员为 struct resouce *resource，一般我们只需要关系start、end和flag这三个字段就够了
```c++
struct resource {
    resource_size_t start;     //资源起始地址
    resource_size_t end;       //资源结束地址
    const char *name;          //资源名称
    unsigned long flags;       // 资源类型，有IO，MEM，IRQ，DMA类型
    struct resource *parent, *sibling, *child;   //资源链表指针
};
```
start、end和flag分别表示资源的开始值、结束值和类型。

flag的取值：
```c++
/*
* IO resources have these defined flags.
*/
#define IORESOURCE_BITS 0x000000ff /* Bus-specific bits */

#define IORESOURCE_TYPE_BITS 0x00001f00 /* Resource type */
#define IORESOURCE_IO 0x00000100 /* PCI/ISA I/O ports */
#define IORESOURCE_MEM 0x00000200
#define IORESOURCE_REG 0x00000300 /* Register offsets */
#define IORESOURCE_IRQ 0x00000400
#define IORESOURCE_DMA 0x00000800
#define IORESOURCE_BUS 0x00001000

#define IORESOURCE_PREFETCH 0x00002000 /* No side effects */
#define IORESOURCE_READONLY 0x00004000
#define IORESOURCE_CACHEABLE 0x00008000
#define IORESOURCE_RANGELENGTH 0x00010000
#define IORESOURCE_SHADOWABLE 0x00020000

#define IORESOURCE_SIZEALIGN 0x00040000 /* size indicates alignment */
#define IORESOURCE_STARTALIGN 0x00080000 /* start field is alignment */

#define IORESOURCE_MEM_64 0x00100000
#define IORESOURCE_WINDOW 0x00200000 /* forwarded by bridge */
#define IORESOURCE_MUXED 0x00400000 /* Resource is software muxed */

#define IORESOURCE_EXCLUSIVE 0x08000000 /* Userland may not map this resource */
#define IORESOURCE_DISABLED 0x10000000
#define IORESOURCE_UNSET 0x20000000 /* No address assigned yet */
#define IORESOURCE_AUTO 0x40000000
#define IORESOURCE_BUSY 0x80000000 /* Driver has marked this resource busy */

/* PCI control bits. Shares IORESOURCE_BITS with above PCI ROM. */
#define IORESOURCE_PCI_FIXED (1<<4) /* Do not move resource */
```
若flag 为IORESOURCE_MEM时，start、end表示platform_device占据内存的起始地址和结束地址。若若flag为IORESOURCE_IRQ时,start、end表示platform_device中断号的开始值和结束值