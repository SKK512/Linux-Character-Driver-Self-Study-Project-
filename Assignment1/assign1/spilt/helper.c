#include <linux/kernel.h>
#include "helper.h"
void print_from_helper(void)
{
	printk(KERN_INFO "split_module: message helper.c\n");
}
