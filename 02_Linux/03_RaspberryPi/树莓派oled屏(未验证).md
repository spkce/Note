# oled 引脚

oled有个2个spi之外的引脚，
DC引脚用来控制输入命令还是数据，这个就选取一个GPIO，那么我们比较熟悉的就是GPIO17；
RST引脚，用来复位显示屏，选取GPIO18；

# 设备树
树莓派3b+的设备树文件
arch/arm/boot/dts/broadcom/bcm2710-rpi-3-b-plus.dts

```
&spi0 {
	pinctrl-names = "default";
	pinctrl-0 = <&spi0_pins &spi0_cs_pins>;
	cs-gpios = <&gpio 8 1>, <&gpio 7 1>;

	spidev0: spidev@0{
		compatible = "spidev";
		reg = <0>;	/* CE0 */
		#address-cells = <1>;
		#size-cells = <0>;
		spi-max-frequency = <125000000>;
	};

	spidev1: spidev@1{
		compatible = "spidev";
		reg = <1>;	/* CE1 */
		#address-cells = <1>;
		#size-cells = <0>;
		spi-max-frequency = <125000000>;
	};
};
```
将spi0修改为oled
```
&spi0 {
	pinctrl-names = "default";
	pinctrl-0 = <&spi0_pins &spi0_cs_pins>;
	cs-gpios = <&gpio 8 1>, <&gpio 7 1>;

	//spidev0: spidev@0{
	//	compatible = "spidev";
	//	reg = <0>;	/* CE0 */
	//	#address-cells = <1>;
	//	#size-cells = <0>;
	//	spi-max-frequency = <125000000>;
	//};
	myoled@0{
		compatible= "pgg,oled";
		reg=<0>; /*CE0 对应cs-gpios = <&gpio 8 1>*/ 
		#address-cells=<1>;
		#size-cells=<0>;
		spi-max-frequency = <125000000>;
		dc-gpios = <&gpio 17 GPIO ACTIVE HIGH>; /*命令/数据切换DC*/
		rst-gpios = <&gpio 18 GPIO_ACTIVE_HIGH>; /*RST*/
	}
	spidev1: spidev@1{
		compatible = "spidev";
		reg = <1>;	/* CE1 */
		#address-cells = <1>;
		#size-cells = <0>;
		spi-max-frequency = <125000000>;
	};
};
```

```c++
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/list.h>
#include <linux/spi/spi.h>
#include <linux/err.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/gpio/consumer.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#define OLED_SET_XY 99
#define OLED_SET_XY_WRITE_DATA 100
#define OLED_SET_XY_WRITE_DATAS 101
#define OLED_SET_DATAS 102 /*102为低8位，高16位用来表示长度*/
//为0表示命令,为1表示数据
#define OLED_CMD 0
#define OLED_DATA 1

struct spi_device *oled dev;
static struct gpio_desc *oled_dc;
static struct gpio_desc *oled_rst;
static int major;
static struct class *oled class;
static char data_buf[1024];
static void oled_write_cmd_data(unsigned char uc_dataa,unsigned char uc_cmd)
{
    if(uc_cmd==0)
    {
        gpiod_set_value(oled_dc,0);//拉低,表示写入指令
    }
    else
    {
        gpiod_set_value(oled_dc,1);//拉高,表示写入数据
    }
    spi_write(oled_dev,&uc_data,1);//写入
}
static void oled_write_datas(unsigned char *buf, int len)
{
    gpiod_set_value(oled_dc,1);/拉高,表示写入数据
    spi_write(oled dev,buf,len);//写入
}
static int oled_hardware_init(void)
{
    gpiod_set_value(oled_rst, 1);
    mdelay(100);
    gpiod_set_value(oled_rst, 0);
    mdelay(100);
    gpiod_set_value(oled_rst, 1);
    oled_write_cmd_data(0xae,OLED_CMD);//关闭显示
    oled_write_cmd_data(0x00,OLED_CMD);//设置 lowercolumn address
    oled_write_cmd_data(0x10,OLED_CMD);//设置 higher column address
    oled_write_cmd_data(0x40,OLED_CMD);//设置 display staart line
    oled_write_cmd_data(0xB0,OLED_CMD);//设置page addres:S
    oled_write_cmd_data(0x81,OLED_CMD);// contract contrrol
    oled_write_cmd_data(0x66,OLED_CMD);//128
    oled_write_cmd_data(0xa1,OLED_CMD);//设置 segment remap
    oled_write_cmd_data(0xa6,OLED_CMD);//normal /reverse
    oled_write_cmd_data(0xa8,OLED_CMD);//multiple ratio
    oled_write_cmd_data(0xFF,OLED_CMD);//duty = 1/64
    oled_write_cmd_data(0xc8,OLED_CMD);//com scan direction
    oled_write_cmd_data(0xd3,OLED_CMD);//set displat offset
    oled_write_cmd_data(0x00,OLED_CMD);//
    oled_write_cmd_data(0xd5,OLED_CMD);//set osc divisidon
    oled_write_cmd_data(0x80,OLED_CMD);//
    oled_write_cmd_data(0xd9,OLED_CMD);//ser pre-chargeperiod
    oled_write_cmd_data(0xF1,OLED_CMD);//
    oled_write_cmd_data(0xda,OLED_CMD);//set com pins
    oled_write_cmd_data(0x12,OLED_CMD);//
    oled_write_cmd_data(0xdb,OLED_CMD);//set vcomh
    oled_write_cmd_data(0x40,OLED_CMD);//
    oled_write_cmd_data(0x8d,OLED_CMD);//set charge pumpdisable
    oled_write_cmd_data(0x14,OLED_CMD);//
    oled_write_cmd_data(0xaf,OLED_CMD);//set dispkayon
    return 0;
}

void OLED_DIsp_Set_Pos(int x, int x, int y)
{
    oled_write_cmd_data(0xb0+y,OLED_CMD);
    oled_write_cmd_data((x&0x0f),OLED_CMD);
    oled_write_cmd_data(((x&0xf0)>>4)|0x10,OLED_CMD);
}
static void OLED DIsp_Clear(void)
{
    unsigned char x, y;
    for (y = 0; y < 8; y++)
    {
        OLED_DIsp_Set_Pos(0, y);
        for (x = 0; x < 128; x++)
            oled_write_cmd_data(0, OLED_DATA);/*清零*/
    }
}

static long oled_ioctl(struct file *file, unsigned iint cmd, unsigned long arg)
{
    const void __user *from = (const void __user *)arg;
    char param_buf[3];
    int size;
    int err;
    
    switch (cmd & 0xff)
    {
        case OLED_SET_XY:
        {
            err = copy_from_user(param_buf, from, 2);
            OLED_DIsp_Set_Pos(param_buf[0], param_buf[1]);
            break;
        }
        case OLED_SET_XY_WRITE_DATA:
        {
            err = copy_from_user(param_buf, from, 3);
            OLED_DIsp_Set_Pos(param_buf[0], param_buf[1]);
            oled_write_cmd_data(param_buf[2], OLED_DATA);
            break;
        }
        case OLED_SET_XY_WRITE_DATAS:
        {
            err = copy_from_user(param_buf, from, 3);
            size = param_buf[2];
            err = copy_from_user(data_buf, from+3, size);
            OLED_DIsp_Set_Pos(param_buf[0], param_buf[1]);
            oled_write_datas(data_buf, size);
            break;
        }
        case OLED_SET_DATAS:
        {
            size = cmd >> 8;
            err = copy_from_user(data_buf, from, size);
            oled_write_datas(data_buf, size);
            break;
        }
    }
    return 0;
}

/*定义自己的file_operations结构体*/
static struct file_operations oled_fops = {
.owner = THIS_MODULE,
.unlocked_ioctl = oled_ioctl,
};
static int oled_probe(struct spi_device *spi)
{
    printk("%s %s %d\n", _FILE_, _FUNCTION_, __LINE__);
    oled_dev = spi;
    /* register_chrdev */
    major = register_chrdev(0, "oled", &oled_fops);
    /* class_create */
    oled_class = class_create(THIS_MODULE, "oled_class")
    /* device_create */
    device_create(oled_class, NULL, MKDEV(major, 0), NULL, "myoled");
    /* spi oled init */
    oled_dc = gpiod_get(&spi->dev, "dc", GPIOD_OUT_HIGH);
    oled_rst = gpiod_get(&spi->dev, "rst", GPIOD_OUT_HIGH);
    oled_hardware_init();
    OLED_DIsp_Clear();
    return 0;
}
static int oled_remove(struct spi_device *spi)
{
    printk("%s %s %d\n", _FILE_, _ FUNCTION_,__LINE_);
    device_destroy(oled_class, MKDEV(major, 0));
    class_destroy(oled_class);
    unregister_chrdev(major, "oled");
    gpiod_put(oled_dc);
    gpiod_put(oled_rst);
    return 0;
}
static const struct of_device_id oled_of_match[] = {
{
    .compatible = "pgg,oled",
    {}
};
static struct spi_driver oled_driver = {
    .driver = {
        .name = "myoled_drv",
        .of_match_table = oled_of_match,
    },
    .probe = oled_probe,
    .remove = oled_remove,
};
int oled_init(void)
{
    return spi_register_driver(&oled_driver);
}

static void oled_exit(void)
{
    spi_unregister_driver(&oled_driver);
}
module_init(oled_init);
module_exit(oled_exit);
MODULE_LICENSE("GPL");
```

