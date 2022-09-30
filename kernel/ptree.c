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

	/* copy *nr from user space into max_entries */
	if (copy_from_user(&max_entries, nr, sizeof(int)))
		return -EFAULT;

	if (max_entries < 1)
		return -EINVAL;

	/* TODO: allocate a buffer for up to 'max_entries' prinfo entries.
	 * the total space needed will be (max_entries * sizeof(struct prinfo)).
	 */

	rcu_read_lock();

	/* TODO: from root_task, do an iterative BFS.
	 * for each task_struct that we iterate on:
	 *   populate a prinfo entry, store it in our allocated buffer at index [actual_entries]
	 *   increment actual_entries
	 *   if actual_entries == max_entries, stop iterating.
	 * be sure not to do anything blocking (like allocation) in this section :)
	 */
	
	rcu_read_unlock();

        /* TODO: copy our stored prinfo entries into *buf using copy_to_user.
	 *  if this fails, return -EFAULT
	 * then, delete our allocated buffer
	 */

	/* update *nr with how many entries were copied */
	if (copy_to_user(nr, &actual_entries, sizeof(int)))
		return -EFAULT;
	
	/* delete this later, this is just to silence unused variable warnings for now */
	(void) root_task;
	(void) actual_entries;
	
	return 0;
}

