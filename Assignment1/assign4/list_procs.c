#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>   /* for for_each_process */
#include <linux/sched.h>          /* for current, task_ppid_nr */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shubham");
MODULE_DESCRIPTION("List all processes starting from current process");
MODULE_VERSION("1.0");

static int __init listprocs_init(void)
{
	struct task_struct *task;
	bool start = false;
	int count = 0;

	printk(KERN_INFO "listprocs: module loaded\n");
	printk(KERN_INFO "listprocs: starting from current: %s (pid=%d)\n",
	       current->comm, current->pid);

	/*
	 * for_each_process iterates over all processes.
	 * We skip entries until we reach 'current', then
	 * start printing from there onwards.
	 */
	for_each_process(task) {
		if (task == current)
			start = true;

		if (!start)
			continue;

		printk(KERN_INFO "listprocs: pid=%d, ppid=%d, comm=%s\n",
		       task->pid, task_ppid_nr(task), task->comm);
		count++;
	}

	printk(KERN_INFO "listprocs: total processes printed = %d\n", count);

	return 0;
}

static void __exit listprocs_exit(void)
{
	printk(KERN_INFO "listprocs: module unloaded\n");
}

module_init(listprocs_init);
module_exit(listprocs_exit);

