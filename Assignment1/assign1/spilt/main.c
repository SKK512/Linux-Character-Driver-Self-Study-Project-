#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include "helper.h"

static int __init split_init(void)
{
	printk(KERN_INFO "split_module: init called\n");
	print_from_helper();
	return 0;
}

static void __exit split_exit(void)
{
	printk(KERN_INFO "split_module: exit called\n");
}


module_init(split_init);
module_exit(split_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("skk");
MODULE_DESCRIPTION("Split Module");
