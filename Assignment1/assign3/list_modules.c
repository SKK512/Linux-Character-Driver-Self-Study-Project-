#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shubham");
MODULE_DESCRIPTION("List all loaded kernel modules");
MODULE_VERSION("1.0");

/*
 * Trick:
 *  - All modules are in a circular doubly-linked list.
 *  - Each struct module has a 'struct list_head list' member.
 *  - THIS_MODULE->list is one node in that list.
 *  - We treat &THIS_MODULE->list as the 'head' and iterate using list_for_each_entry.
 *  - That gives us all other modules; we print self separately.
 */

static int __init listmods_init(void)
{
	struct module *mod;
	int count = 0;

	printk(KERN_INFO "listmods: loaded modules (including this one):\n");

	/* Print this module first */
	printk(KERN_INFO "listmods: %s (this module)\n", THIS_MODULE->name);
	count++;

	/*
	 * Iterate over the global module list using THIS_MODULE->list
	 * as the head of a circular list.
	 * This will visit all other modules one by one.
	 */
	list_for_each_entry(mod, &THIS_MODULE->list, list) {
		printk(KERN_INFO "listmods: %s\n", mod->name);
		count++;
	}

	printk(KERN_INFO "listmods: total modules = %d\n", count);

	return 0;
}

static void __exit listmods_exit(void)
{
	printk(KERN_INFO "listmods: module unloaded\n");
}

module_init(listmods_init);
module_exit(listmods_exit);

