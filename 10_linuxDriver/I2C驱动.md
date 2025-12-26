# I2C驱动框架
1. I2C 核心： I2C 核心提供了 I2C 总线驱动和设备驱动的注册、 注销方法
2. I2C 总线驱动： I2C 总线驱动是对 I2C 硬件体系结构中适配器端的实现， 适配器可由CPU 控制， 甚至可以直接集成在 CPU 内部。I2C 总线驱动就是 SOC 的 I2C 控制器驱动，也叫做 I2C 适配器驱动
3. I2C 设备驱动： I2C 设备驱动是对 I2C 硬件体系结构中设备端的实现， 设备一般挂接在受 CPU 控制的 I2C 适配器上， 通过 I2C 适配器与 CPU 交换数据。

## api
i2c_adapter 注册销毁函数
```c++
int i2c_add_adapter(struct i2c_adapter *adapter)
int i2c_add_numbered_adapter(struct i2c_adapter *adap)
void i2c_del_adapter(struct i2c_adapter * adap)
```
i2c_driver 注册/注销函数
```c++
int i2c_register_driver(struct module *owner, struct i2c_driver *driver)
int i2c_add_driver (struct i2c_driver *driver)
void i2c_del_driver(struct i2c_driver *driver)
```

# I2C 总线驱动
I2C 总线和 platform 总线类似， 区别在于platform 总线是虚拟的一条总线， 而 I2C 总线是实际
存在的。 对于使用 I2C 通信的设备， 在驱动中直接使用 I2C 总线即可。 I2C 总线驱动的重点是 I2C 适配器驱动  
I2C 总线驱动（I2C 适配器驱动）的主要工作就是初始化 i2c_adapter 结构体变量，然后设置 i2c_algorithm 中的 master_xfer 函数。最后通过 i2c_add_numbered_adapter或 i2c_add_adapter 这两个函数向系统注册i2c_adapter。
一般 SOC 的 I2C 总线驱动都是由半导体厂商编写的，所以大多数只要专注于 I2C 设备驱动即可。


## 关键结构体
### i2c_adapter
```c++
//include/linux/i2c.h
struct i2c_adapter {
    struct module *owner;
    unsigned int class; /* classes to allow probing for */
    const struct i2c_algorithm *algo; /* 总线访问算法 */
    void *algo_data;
 
    /* data fields that are valid for all devices */
    struct rt_mutex bus_lock;
 
    int timeout; /* in jiffies */
    int retries;
    struct device dev; /* the adapter device */
 
    int nr;
    char name[48];
    struct completion dev_released;
 
    struct mutex userspace_clients_lock;
    struct list_head userspace_clients;
 
    struct i2c_bus_recovery_info *bus_recovery_info;
    const struct i2c_adapter_quirks *quirks;
};
```
i2c_algorithm 类型的指针变量 algo， 对于一个 I2C 适配器， 要对外提供读写 API 函数， 设备驱动程序可以使用这些 API 函数来完成读写操作。 i2c_algorithm 是 I2C 适配器与 IIC 设备进行通信的方法。

### i2c_algorithm
```c++
//include/linux/i2c.h 
 
struct i2c_algorithm 
{
    ......
    int (*master_xfer)(struct i2c_adapter *adap,struct i2c_msg *msgs,int num);
    int (*smbus_xfer) (struct i2c_adapter *adap, u16 addr,
    unsigned short flags, char read_write,
    u8 command, int size, union i2c_smbus_data *data);
 
    /* To determine what the adapter supports */
    u32 (*functionality) (struct i2c_adapter *);
    ......
};
```
master_xfer 是 I2C 适配器的传输函数， 可以通过此函数来完成与 IIC 设备之间的通信。smbus_xfer 是 SMBUS 总线的传输函数

# I2C 设备驱动
在 I2C 设备驱动中主要有两个重要的结构体： i2c_client 和 i2c_driver。 i2c_client 是描述设备信息的，i2c_driver 描述驱动内容。 

## i2c_client
当驱动和设备匹配成功后，每检测到一个 I2C 设备就会给这个 I2C 设备分配一个i2c_client. 这个ic_client存储着这个设备的所有信息，如芯片地址
```c++
//include/linux/i2c.h
struct i2c_client {
    unsigned short flags; /* 标志 */
    unsigned short addr; /* 芯片地址， 7 位，存在低 7 位*/
    ......
    char name[I2C_NAME_SIZE]; /* 名字 */
    struct i2c_adapter *adapter; /* 对应的 I2C 适配器 */
    struct device dev; /* 设备结构体 */
    int irq; /* 中断 */
    struct list_head detected;
    ......
};
```

## i2c_driver
i2c_driver 类似 platform_driver，是编写 I2C 设备驱动重点要处理的内容
```c++
//include/linux/i2c.h
struct i2c_driver {
    unsigned int class;
 
    /* Notifies the driver that a new bus has appeared. You should
    * avoid using this, it will be removed in a near future.
    */
    int (*attach_adapter)(struct i2c_adapter *) __deprecated;
 
    /* Standard driver model interfaces */
    int (*probe)(struct i2c_client *, const struct i2c_device_id *);
    int (*remove)(struct i2c_client *);
 
    /* driver model interfaces that don't relate to enumeration */
    void (*shutdown)(struct i2c_client *);
 
    /* Alert callback, for example for the SMBus alert protocol.
    * The format and meaning of the data value depends on the
    * protocol.For the SMBus alert protocol, there is a single bit
    * of data passed as the alert response's low bit ("eventflag"). */
    void (*alert)(struct i2c_client *, unsigned int data);
    /* a ioctl like command that can be used to perform specific
    * functions with the device.
    */
    int (*command)(struct i2c_client *client, unsigned int cmd,void *arg);
 
    struct device_driver driver;
    const struct i2c_device_id *id_table;
 
    /* Device detection callback for automatic device creation */
    int (*detect)(struct i2c_client *, struct i2c_board_info *);
    const unsigned short *address_list;
    struct list_head clients;
};
```
当 I2C 设备和驱动匹配成功以后 probe 函数就会执行。device_driver 驱动结构体，如果使用设备树，需要设置 device_driver 的of_match_table 成员变量，即驱动的兼容(compatible)性。 未使用设备树的设备需要设置id_table 设备匹配 ID 表。

在编写 I2C 设备的驱动时， 主要是创建 i2c_driver 结构体， 并实现里面的内容。 i2c_driver 结构体创建完成后， 使用i2c_register_driver 函数向 Linux 内核中注册 i2c 设备。


# api
## i2c_transfer
```c++
struct i2c_msg {
	__u16 addr;	/* slave address			*/
	__u16 flags;
#define I2C_M_RD		0x0001	/* read data, from slave to master */
					/* I2C_M_RD is guaranteed to be 0x0001! */
#define I2C_M_TEN		0x0010	/* this is a ten bit chip address */
#define I2C_M_DMA_SAFE		0x0200	/* the buffer of this message is DMA safe */
					/* makes only sense in kernelspace */
					/* userspace buffers are copied anyway */
#define I2C_M_RECV_LEN		0x0400	/* length will be first received byte */
#define I2C_M_NO_RD_ACK		0x0800	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_IGNORE_NAK	0x1000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_REV_DIR_ADDR	0x2000	/* if I2C_FUNC_PROTOCOL_MANGLING */
#define I2C_M_NOSTART		0x4000	/* if I2C_FUNC_NOSTART */
#define I2C_M_STOP		0x8000	/* if I2C_FUNC_PROTOCOL_MANGLING */
	__u16 len;		/* msg length				*/
	__u8 *buf;		/* pointer to msg data			*/
};
int i2c_transfer(struct i2c_adapter *adap, struct i2c_msg *msgs, int num)
```
‌adap‌：指向 I2C 适配器（i2c_adapter）的指针，代表与物理总线连接的硬件抽象层。‌‌
‌msgs‌：指向 i2c_msg 结构体数组的指针，每个结构体描述一条独立消息（如写寄存器地址或读数据）。‌‌
‌num‌：消息数组中的消息数量，支持单次传输多条消息（如先写后读操作）

例子：
```c++
static const struct regmap_config lt9611_regmap_config = {
    .reg_bits = 8,
    .val_bits = 8,
	.reg_read = i2c_read,
	.reg_write = i2c_write,
    .max_register = 0x100,
};
static int i2c_read(void *context, unsigned int reg, unsigned int *val)
{
	struct i2c_client *i2c = context;
    struct i2c_msg xfer[2] = {0};
    uint8_t regAddr = (uint8_t)reg;
    uint8_t buf = 0;
    int ret;

	xfer[0].addr = i2c->addr;
	xfer[0].flags = 0;
	xfer[0].len = 1;
	xfer[0].buf = (uint8_t*)&regAddr;

	/* Read data */
	xfer[1].addr = i2c->addr;
	xfer[1].flags = I2C_M_RD;
	xfer[1].len = 1;
	xfer[1].buf = (uint8_t*)&buf;

	ret = i2c_transfer(i2c->adapter, xfer, 2);
	if (ret < 0)
		return ret;
	else if (ret != 2)
		return -EIO;
    
    *val = buf;
    return 0;
}

static int i2c_write(void *context, unsigned int reg, unsigned int val)
{
    struct i2c_client *i2c = context;
	struct i2c_msg xfer[1];
    int ret;
    uint8_t buf[2] = {0};
    buf[0] = (uint8_t)reg;
    buf[1] = (uint8_t)val;

    xfer[0].addr = i2c->addr;
	xfer[0].flags = 0;
	xfer[0].len = 2;
	xfer[0].buf = (uint8_t *)&buf;

    ret = i2c_transfer(i2c->adapter, xfer, 1);
	if (ret < 0)
		return ret;
	else if (ret != 2)
		return -EIO;

    return 0;
}
```
## i2c_master_send
i2c_master_send 内部会创建一个 i2c_msg 结构体，然后使用i2c_transfer发送数据
```c++
static int write_register(struct i2c_client *client, u8 reg, u8 value) {
    u8 buffer[2];
    buffer[0] = reg;   // 寄存器地址
    buffer[1] = value; // 数据
    int ret = i2c_master_send(client, buffer, 2);
    if (ret != 2) {
        dev_err(&client->dev, "Write failed: %d\n", ret);
        return ret;
    }
    return 0;
}

```