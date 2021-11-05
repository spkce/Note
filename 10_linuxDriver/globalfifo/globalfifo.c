#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define GLOBALFIFO_SIZE 0x1000
#define MEM_CLEAR 0x01
#define GLOBALFIFO_MAJOR 231
#define DEVICE_NUM 1

static int globalfifo_major = GLOBALFIFO_MAJOR;
module_param(globalfifo_major, int, S_IRUGO);

struct globalfifo_dev
{
	struct cdev cdev;
	unsigned char mem[GLOBALFIFO_SIZE];
	struct mutex mutex;
	unsigned int current_len;
	wait_queue_head_t r_wait;
	wait_queue_head_t w_wait;
};

struct globalfifo_dev *globalfifo_devp;

static int globalfifo_open(struct inode *inode, struct file *filp)
{
	struct globalfifo_dev* dev = container_of(inode->i_cdev, struct globalfifo_dev, cdev);
	filp->private_data = dev;
	return 0;
}

static int globalfifo_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t globalfifo_read(struct file *filp, char __user * buf, size_t count, loff_t *ppos)
{
	int ret = 0;

	struct globalfifo_dev *dev = filp->private_data;
	DECLARE_WAITQUEUE(wait, current);
	mutex_lock(&dev->mutex);
	add_wait_queue(&dev->r_wait, &wait);

	while (dev->current_len == 0)
	{
		if (filp->f_flags & O_NONBLOCK)
		{
			printk(KERN_INFO"!!!!!!!!!!!!!!!!\n");
			ret = -EAGAIN;
			goto out;
		}

		__set_current_state(TASK_INTERRUPTIBLE);
		mutex_unlock(&dev->mutex);

		schedule();
		if (signal_pending(current))
		{
			ret = -ERESTARTSYS;
			goto out2;
		}
		mutex_lock(&dev->mutex);
	}


	if (count > dev->current_len)
	{
		count = dev->current_len;
	}

	if (copy_to_user(buf, dev->mem, count))
	{
		ret = -EFAULT;
		goto out;
	}
	else
	{
		memcpy(dev->mem, dev->mem + count, dev->current_len - count);
		dev->current_len -= count;
		printk(KERN_INFO"read %ld bytes, current len:%d\n", count, dev->current_len);

		wake_up_interruptible(&dev->w_wait);

		ret = count;
	}
out:
	mutex_unlock(&dev->mutex);
out2:
	remove_wait_queue(&dev->r_wait, &wait);
	__set_current_state(TASK_RUNNING);
	return ret;
}

static ssize_t globalfifo_write(struct file *filp, const char __user * buf, size_t count, loff_t *ppos)
{
	int ret = 0;

	struct globalfifo_dev *dev = filp->private_data;
	
	DECLARE_WAITQUEUE(wait, current);
	mutex_lock(&dev->mutex);
	add_wait_queue(&dev->w_wait, &wait);

	while (dev->current_len == GLOBALFIFO_SIZE)
	{
		if (filp->f_flags & O_NONBLOCK)
		{
			ret = -EAGAIN;
			goto out;
		}

		__set_current_state(TASK_INTERRUPTIBLE);
		mutex_unlock(&dev->mutex);

		schedule();
		if (signal_pending(current))
		{
			ret = -ERESTARTSYS;
			goto out2;
		}
		mutex_lock(&dev->mutex);
	}
	
	if (count > GLOBALFIFO_SIZE - dev->current_len)
	{
		count = GLOBALFIFO_SIZE - dev->current_len;
	}

	if (copy_from_user(dev->mem + dev->current_len, buf, count))
	{
		ret = -EFAULT;
		goto out;
	}
	else
	{
		dev->current_len += count;
		printk(KERN_INFO"written %ld bytes, current len:%d\n", count, dev->current_len);
		wake_up_interruptible(&dev->r_wait);
		ret = count;
	}
out:
	mutex_unlock(&dev->mutex);
out2:
	remove_wait_queue(&dev->w_wait, &wait);
	__set_current_state(TASK_RUNNING);
	return ret;
}

static const struct file_operations globalfifo_fops = {
	.owner = THIS_MODULE,
	.read = globalfifo_read,
	.write = globalfifo_write,
	.open = globalfifo_open,
	.release = globalfifo_release,
};

static int __init globalfifo_init(void)
{
	int ret, i, err;
	struct globalfifo_dev * dev;
	dev_t devno = MKDEV(globalfifo_major, 0);

	if (globalfifo_major)
	{
		ret = register_chrdev_region(devno, DEVICE_NUM, "globalfifo");
	}
	else
	{
		ret = alloc_chrdev_region(&devno, 0, DEVICE_NUM, "globalfifo");
	}

	if (ret)
	{
		return ret;
	}

	globalfifo_devp = kzalloc(sizeof(struct globalfifo_dev) * DEVICE_NUM, GFP_KERNEL);
	if (!globalfifo_devp)
	{
		ret = -ENOMEM;
		goto fail_malloc;
	}
	
	for (i = 0; i < DEVICE_NUM; i++)
	{
		dev = globalfifo_devp + i;
		devno = MKDEV(globalfifo_major, i);
		
		mutex_init(&dev->mutex);
		init_waitqueue_head(&dev->r_wait);
		init_waitqueue_head(&dev->w_wait);
		cdev_init(&dev->cdev, &globalfifo_fops);
		dev->cdev.owner = THIS_MODULE;
		err = cdev_add(&dev->cdev, devno, 1);
		if (err)
		{
			printk(KERN_NOTICE "Error:%d adding globalmen:%d", err, i);
		}
	}
	
	return 0;

fail_malloc:
	unregister_chrdev_region(devno, DEVICE_NUM);
	return ret;
}

static void __exit globalfifo_exit(void)
{
	int i;
	for ( i = 0; i < DEVICE_NUM; i++)
	{
		cdev_del(&(globalfifo_devp + i)->cdev);
	}
	
	kfree(globalfifo_devp);
	unregister_chrdev_region(MKDEV(globalfifo_major, 0), DEVICE_NUM);
}

module_init(globalfifo_init);
module_exit(globalfifo_exit);

MODULE_AUTHOR("spkce <spkce_crt@163.com>");
MODULE_LICENSE("GPL v2");
