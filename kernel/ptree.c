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
	int max_entries;
	int actual_entries = 0;

	if (buf == NULL || nr == NULL)
		return -EINVAL;

	/* TODO: if buf or nr are outside accessible address space, return -EFAULT */

	max_entries = *nr; /* TODO: should this be done another way, e.g. with copy_from_user? */
	if (max_entries < 1)
		return -EINVAL;

	/* TODO: allocate a buffer for up to 'max_entries' prinfo entries. */

	rcu_read_lock();
	
	/* TODO: from root_task, do an iterative BFS.
	 * for each task_struct that we iterate on:
	 *   populate a prinfo entry, store it in our allocated buffer
	 *   increment actual_entries
	 *   if actual_entries == max_entries, stop iterating
	 */
	
	rcu_read_unlock();

        /* TODO: copy our stored prinfo entries into *buf using copy_to_user.
	 * then, delete our allocated buffer
	 * also, set *nr to actual_entries
	 */

	/* delete this later, this is just to silence unused variable warnings for now */
	(void) root_task;
	(void) actual_entries;
	
	return 0;
}

