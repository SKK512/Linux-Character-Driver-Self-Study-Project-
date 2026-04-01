#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

static int value = 0;
static char *name = (char *)"default";
static int arr[4] = {0, 0, 0, 0};
static int arr_len = 0;

module_param(value, int, 0644);
MODULE_PARM_DESC(value, "An integer value");

module_param(name, charp, 0644);
MODULE_PARM_DESC(name, "A string parameter");

module_param_array(arr, int, &arr_len, 0644);
MODULE_PARM_DESC(arr, "An integer array");

static int __init param_init(void)
{
	int i;

	printk(KERN_INFO "param_demo: init\n");
	printk(KERN_INFO "param_demo: value = %d\n", value);
	printk(KERN_INFO "param_demo: name  = %s\n", name);

	printk(KERN_INFO "param_demo: array length = %d\n", arr_len);
	for (i = 0; i < arr_len; i++)
		printk(KERN_INFO "param_demo: arr[%d] = %d\n", i, arr[i]);

	return 0;
}

static void __exit param_exit(void)
{
	printk(KERN_INFO "param_demo: exit\n");
}

module_init(param_init);
module_exit(param_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("skk");
MODULE_DESCRIPTION("Module Parameter");
