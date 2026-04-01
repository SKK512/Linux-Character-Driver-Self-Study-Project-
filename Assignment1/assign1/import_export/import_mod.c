#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shubham");
MODULE_DESCRIPTION("Import Module");
MODULE_VERSION("1.0");

extern int add_two_numbers(int a, int b);

static int __init import_init(void)
{
    int result;

    printk(KERN_INFO "import_mod: init\n");

    result = add_two_numbers(7, 5);
    printk(KERN_INFO "import_mod: result from exported function = %d\n", result);

    return 0;
}

static void __exit import_exit(void)
{
    printk(KERN_INFO "import_mod: exit\n");
}

module_init(import_init);
module_exit(import_exit);

