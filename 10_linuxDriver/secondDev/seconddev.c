#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/device.h>//�����Զ��豸�ڵ�Ҫ������ͷ�ļ�
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
 
static struct class *firstdrv_class;//�Զ������豸�ڵ�ʱҪ�õ�һ����
static int second_major = SECOND_MAJOR;//�������豸��Ϊ248
static int second_minor = SECOND_MINOR;//������豸��Ϊ100
 
//�豸�ṹ��
struct second_dev 
{
 struct cdev cdev;
 atomic_t counter;//����һ��ԭ�ӱ���
 struct timer_list s_timer;//���嶨ʱ���ṹ�����������ʱ����������ʱ����ʱ���
};
 
struct second_dev *second_devp;//�豸�ṹ��ָ��
//��ʱ��������
static void second_timer_handle(unsigned long arg)
{
	mod_timer(&second_devp->s_timer,jiffies + HZ);//���¼��ض�ʱ���ĵ���ʱ����ԭ�������ϼӶ�1s
	atomic_inc(&second_devp->counter);//ԭ�ӱ���ֵ��1
	printk(KERN_NOTICE "current jiffies is %ld\n",jiffies);//��ӡ�����Ƕ�ʱ������ʵ�ֹ��ܾ��Ǵ�ӡ
}
 
int second_open(struct inode *inode,struct file *filp)//�򿪺���
{
	init_timer(&second_devp->s_timer);//��ʼ����ʱ��
	second_devp->s_timer.function = &second_timer_handle;//ָ����ʱ������
	second_devp->s_timer.expires = jiffies + HZ;//ָ����ʱ����ʱ�䣬��ԭ����1s����ö�ʱ������
	add_timer(&second_devp->s_timer);//����ʱ���ӵ��ں˶�̬��ʱ�������У�ע�ᶨʱ��
	atomic_set(&second_devp->counter,0);//��ʼ��ԭ�ӱ�����ֵΪ0
	return 0;
}
 
//�ر��ļ�
int second_release(struct inode *inode,struct file *filp)
{
	del_timer(&second_devp->s_timer);//ɾ����ʱ��
	return 0;
}
 
static ssize_t second_read(struct file *filp,char __user *buf,size_t count,loff_t *ppos)
{
	int counter;
	counter = atomic_read(&second_devp->counter);//���ԭ�ӱ�����ֵ
 
	if(put_user(counter,(int *)buf))//put_user()ʵ���ں�->�û�
		return -EFAULT;
	else
 		return sizeof(unsigned int);//�����ֽ�
}
 
static const struct file_operations second_fops = {
 .owner = THIS_MODULE,
 .open  = second_open,
 .release = second_release,
 .read  = second_read,
};
 
//����cdev��ʼ������
static void second_setup_cdev(struct second_dev *dev,int index)
{
	int err,devno = MKDEV(second_major,index);//�ϲ������豸��
	cdev_init(&dev->cdev,&second_fops);//��ʼ���豸��cdev��������cdev��file_operations ����
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev,devno,1);//ע���豸���ں�
 
	if(err)
		printk(KERN_NOTICE "Error %d adding LED%d",err,index);
}
 
int second_init(void)
{
	int ret;
	dev_t devno = MKDEV(second_major,second_minor);//�ϲ������豸��Ϊdev_t��ʽ
 
	if(second_major)
	 ret = register_chrdev_region(devno,1,"second");//�����豸��
	else
	{
	 ret = alloc_chrdev_region(&devno, 0, 1, "second");//��̬�����豸��
	 second_major = MAJOR(devno);//������豸��
	 second_minor = MINOR(devno);
	}
 
	if(ret < 0)
		return ret;
 
	 firstdrv_class = class_create(THIS_MODULE, "second_major");//����firstdrv�����
	 device_create(firstdrv_class,NULL,devno,NULL,"second");//�Զ������豸�ڵ�/dev/second
 
	second_devp = kmalloc(sizeof(struct second_dev),GFP_KERNEL);//��̬�����豸�ṹ���ڴ�
	if(!second_devp)
	{
		 ret = -ENOMEM;
		 goto fail_malloc;
	}
	memset(second_devp,0,sizeof(struct second_dev));//����豸�ṹ���ڴ�
	second_setup_cdev(second_devp,second_minor);//��ʼ��cdev
	return 0;
 
	fail_malloc:
	 unregister_chrdev_region(devno, 1);
	 return ret;
}
 
void second_exit(void)
{
	cdev_del(&second_devp->cdev);//ע��cdev
	kfree(second_devp);//�ͷ��豸�ṹ���ڴ�
	unregister_chrdev_region(MKDEV(second_major,second_minor),1);//�ͷ��豸��
	device_destroy(firstdrv_class,MKDEV(second_major,second_minor));
	class_destroy(firstdrv_class);//�ͷ�����࣬�ɵ���
}
 
MODULE_LICENSE("Dual BSD/GPL");
module_init(second_init);
module_exit(second_exit);