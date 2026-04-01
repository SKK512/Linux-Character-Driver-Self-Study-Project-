#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DEVICE_NAME   "pchar_dev"
#define DEVICE_CLASS  "pchar_class"
#define MEM_SIZE      1024



static dev_t dev_num;
static struct cdev pchar_cdev;
static struct class *pchar_class;
static char device_buffer[MEM_SIZE];

static int pchar_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "pchar: open\n");
	file->private_data = device_buffer;
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
	char *dev_buf = (char *)file->private_data;

	if (*ppos >= MEM_SIZE)
		return 0;

	if (count > MEM_SIZE - *ppos)
		count = MEM_SIZE - *ppos;

	if (copy_to_user(buf, dev_buf + *ppos, count))
		return -EFAULT;

	*ppos += count;

	printk(KERN_INFO "pchar: read %zu bytes, f_pos = %lld\n",
	       count, *ppos);

	return count;
}

static ssize_t pchar_write(struct file *file, const char __user *buf,
			   size_t count, loff_t *ppos)
{
	char *dev_buf = (char *)file->private_data;

	if (*ppos >= MEM_SIZE)
		return -ENOSPC;

	if (count > MEM_SIZE - *ppos)
		count = MEM_SIZE - *ppos;

	if (copy_from_user(dev_buf + *ppos, buf, count))
		return -EFAULT;

	*ppos += count;

	printk(KERN_INFO "pchar: wrote %zu bytes, f_pos = %lld\n",
	       count, *ppos);

	return count;
}

static loff_t pchar_llseek(struct file *file, loff_t offset, int whence)
{
	loff_t new_pos = 0;

	switch (whence) {
	case SEEK_SET:
		new_pos = offset;
		break;
	case SEEK_CUR:
		new_pos = file->f_pos + offset;
		break;
	case SEEK_END:
		new_pos = MEM_SIZE + offset;
		break;
	default:
		return -EINVAL;
	}

	if (new_pos < 0 || new_pos > MEM_SIZE)
		return -EINVAL;

	file->f_pos = new_pos;

	printk(KERN_INFO "pchar: llseek -> new f_pos = %lld\n", new_pos);

	return new_pos;
}

static const struct file_operations pchar_fops = {
	.owner   = THIS_MODULE,
	.open    = pchar_open,
	.release = pchar_release,
	.read    = pchar_read,
	.write   = pchar_write,
	.llseek  = pchar_llseek,
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

	/* NEW STYLE: only name argument */
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

	memset(device_buffer, 0, MEM_SIZE);

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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shubham");
MODULE_DESCRIPTION("Pseudo Character Device Driver - Practice");
