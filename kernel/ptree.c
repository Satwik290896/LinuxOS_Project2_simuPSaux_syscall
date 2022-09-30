#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/prinfo.h>
#include <linux/syscalls.h>

static struct task_struct *get_root(int root_pid)
{
        if (root_pid == 0)
                return &init_task;

        return find_task_by_vpid(root_pid);
}

SYSCALL_DEFINE3(ptree, struct prinfo __user *, buf, int __user *, nr, int, root_pid)
{
	struct task_struct *root_task = get_root(root_pid);
	(void) root_task;
	
	return 0;
}

