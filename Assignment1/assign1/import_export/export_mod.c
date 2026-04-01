#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shubham");
MODULE_DESCRIPTION("Export Module");
MODULE_VERSION("1.0");

int add_two_numbers(int a, int b)
{
    printk(KERN_INFO "export_mod: adding %d and %d\n", a, b);
    return a + b;
}
EXPORT_SYMBOL(add_two_numbers);   

static int __init export_init(void)
{
    printk(KERN_INFO "export_mod: loaded, function exported\n");
    return 0;
}

static void __exit export_exit(void)
{
    printk(KERN_INFO "export_mod: unloaded\n");
}

module_init(export_init);
module_exit(export_exit);

