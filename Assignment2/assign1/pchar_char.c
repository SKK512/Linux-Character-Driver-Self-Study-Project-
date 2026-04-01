#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/kfifo.h>
#include <linux/wait.h>
#include <linux/sched.h>

#define DEVICE_NAME   "pchar_dev"
#define DEVICE_CLASS  "pchar_class"
#define FIFO_SIZE     1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shubham");
MODULE_DESCRIPTION("Pseudo Character Device Driver with blocking read");
MODULE_VERSION("1.0");

static dev_t dev_num;
static struct cdev pchar_cdev;
static struct class *pchar_class;

static DECLARE_KFIFO(pchar_fifo, char, FIFO_SIZE);
static wait_queue_head_t rd_wq;

static int pchar_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "pchar: open\n");
	return 0;
}

static int pchar_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "pchar: release\n");
	return 0;
}

static ssize_t pchar_read(struct file *file, char __user *buf,
			  size_t count, loff_t *ppos)
{
	unsigned int copied;
	int ret;

	if (kfifo_is_empty(&pchar_fifo)) {
		printk(KERN_INFO "pchar: buffer empty, reader will sleep\n");

		if (file->f_flags & O_NONBLOCK)
			return -EAGAIN;

		ret = wait_event_interruptible(rd_wq,
					       !kfifo_is_empty(&pchar_fifo));
		if (ret)
			return ret;
	}

	ret = kfifo_to_user(&pchar_fifo, buf, count, &copied);
	if (ret)
		return ret;

	printk(KERN_INFO "pchar: read %u bytes\n", copied);

	return copied;
}

static ssize_t pchar_write(struct file *file, const char __user *buf,
			   size_t count, loff_t *ppos)
{
	unsigned int copied;
	int ret;

	ret = kfifo_from_user(&pchar_fifo, buf, count, &copied);
	if (ret)
		return ret;

	printk(KERN_INFO "pchar: wrote %u bytes\n", copied);

	if (copied > 0)
		wake_up_interruptible(&rd_wq);

	return copied;
}

static const struct file_operations pchar_fops = {
	.owner   = THIS_MODULE,
	.open    = pchar_open,
	.release = pchar_release,
	.read    = pchar_read,
	.write   = pchar_write,
};

static int __init pchar_init(void)
{
	int ret;

	ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (ret) {
		printk(KERN_ERR "pchar: alloc_chrdev_region failed\n");
		return ret;
	}

	printk(KERN_INFO "pchar: registered with major=%d, minor=%d\n",
	       MAJOR(dev_num), MINOR(dev_num));

	cdev_init(&pchar_cdev, &pchar_fops);
	pchar_cdev.owner = THIS_MODULE;

	ret = cdev_add(&pchar_cdev, dev_num, 1);
	if (ret) {
		printk(KERN_ERR "pchar: cdev_add failed\n");
		unregister_chrdev_region(dev_num, 1);
		return ret;
	}

	pchar_class = class_create(DEVICE_CLASS);
	if (IS_ERR(pchar_class)) {
		printk(KERN_ERR "pchar: class_create failed\n");
		cdev_del(&pchar_cdev);
		unregister_chrdev_region(dev_num, 1);
		return PTR_ERR(pchar_class);
	}

	if (IS_ERR(device_create(pchar_class, NULL, dev_num, NULL, DEVICE_NAME))) {
		printk(KERN_ERR "pchar: device_create failed\n");
		class_destroy(pchar_class);
		cdev_del(&pchar_cdev);
		unregister_chrdev_region(dev_num, 1);
		return -1;
	}

	INIT_KFIFO(pchar_fifo);
	init_waitqueue_head(&rd_wq);

	printk(KERN_INFO "pchar: device initialized successfully\n");

	return 0;
}

static void __exit pchar_exit(void)
{
	device_destroy(pchar_class, dev_num);
	class_destroy(pchar_class);
	cdev_del(&pchar_cdev);
	unregister_chrdev_region(dev_num, 1);

	printk(KERN_INFO "pchar: device removed\n");
}

module_init(pchar_init);
module_exit(pchar_exit);
