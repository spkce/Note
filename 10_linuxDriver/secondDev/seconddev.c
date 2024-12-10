#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/device.h>//建立自动设备节点要包含的头文件
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
//#include <asm/system.h>
#include <linux/slab.h>
 
#include <asm/uaccess.h>
 
#define SECOND_MAJOR 248
#define SECOND_MINOR 100
 
static struct class *firstdrv_class;//自动建立设备节点时要用到一个类
static int second_major = SECOND_MAJOR;//定义主设备号为248
static int second_minor = SECOND_MINOR;//定义次设备号为100
 
//设备结构体
struct second_dev 
{
 struct cdev cdev;
 atomic_t counter;//定义一个原子变量
 struct timer_list s_timer;//定义定时器结构体里面包含定时处理函数，定时到期时间等
};
 
struct second_dev *second_devp;//设备结构体指针
//定时器处理函数
static void second_timer_handle(unsigned long arg)
{
	mod_timer(&second_devp->s_timer,jiffies + HZ);//重新加载定时器的到期时间在原来基础上加多1s
	atomic_inc(&second_devp->counter);//原子变量值加1
	printk(KERN_NOTICE "current jiffies is %ld\n",jiffies);//打印，我们定时处理函数实现功能就是打印
}
 
int second_open(struct inode *inode,struct file *filp)//打开函数
{
	init_timer(&second_devp->s_timer);//初始化定时器
	second_devp->s_timer.function = &second_timer_handle;//指定定时处理函数
	second_devp->s_timer.expires = jiffies + HZ;//指定定时到期时间，在原来上1s后调用定时处理函数
	add_timer(&second_devp->s_timer);//将定时器加到内核动态定时器链表中，注册定时器
	atomic_set(&second_devp->counter,0);//初始化原子变量的值为0
	return 0;
}
 
//关闭文件
int second_release(struct inode *inode,struct file *filp)
{
	del_timer(&second_devp->s_timer);//删除定时器
	return 0;
}
 
static ssize_t second_read(struct file *filp,char __user *buf,size_t count,loff_t *ppos)
{
	int counter;
	counter = atomic_read(&second_devp->counter);//获得原子变量的值
 
	if(put_user(counter,(int *)buf))//put_user()实现内核->用户
		return -EFAULT;
	else
 		return sizeof(unsigned int);//返回字节
}
 
static const struct file_operations second_fops = {
 .owner = THIS_MODULE,
 .open  = second_open,
 .release = second_release,
 .read  = second_read,
};
 
//用于cdev初始化函数
static void second_setup_cdev(struct second_dev *dev,int index)
{
	int err,devno = MKDEV(second_major,index);//合并主次设备号
	cdev_init(&dev->cdev,&second_fops);//初始化设备（cdev），建立cdev和file_operations 连接
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev,devno,1);//注册设备到内核
 
	if(err)
		printk(KERN_NOTICE "Error %d adding LED%d",err,index);
}
 
int second_init(void)
{
	int ret;
	dev_t devno = MKDEV(second_major,second_minor);//合并主次设备号为dev_t格式
 
	if(second_major)
	 ret = register_chrdev_region(devno,1,"second");//申请设备号
	else
	{
	 ret = alloc_chrdev_region(&devno, 0, 1, "second");//动态申请设备号
	 second_major = MAJOR(devno);//获得主设备号
	 second_minor = MINOR(devno);
	}
 
	if(ret < 0)
		return ret;
 
	 firstdrv_class = class_create(THIS_MODULE, "second_major");//建立firstdrv这个类
	 device_create(firstdrv_class,NULL,devno,NULL,"second");//自动创建设备节点/dev/second
 
	second_devp = kmalloc(sizeof(struct second_dev),GFP_KERNEL);//动态申请设备结构体内存
	if(!second_devp)
	{
		 ret = -ENOMEM;
		 goto fail_malloc;
	}
	memset(second_devp,0,sizeof(struct second_dev));//清除设备结构体内存
	second_setup_cdev(second_devp,second_minor);//初始化cdev
	return 0;
 
	fail_malloc:
	 unregister_chrdev_region(devno, 1);
	 return ret;
}
 
void second_exit(void)
{
	cdev_del(&second_devp->cdev);//注销cdev
	kfree(second_devp);//释放设备结构体内存
	unregister_chrdev_region(MKDEV(second_major,second_minor),1);//释放设备号
	device_destroy(firstdrv_class,MKDEV(second_major,second_minor));
	class_destroy(firstdrv_class);//释放这个类，干掉它
}
 
MODULE_LICENSE("Dual BSD/GPL");
module_init(second_init);
module_exit(second_exit);