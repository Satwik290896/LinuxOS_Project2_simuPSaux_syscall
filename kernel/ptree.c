#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/sched/task.h>
#include <linux/prinfo.h>
#include <linux/syscalls.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/unistd.h>

static struct task_struct *get_root(int root_pid)
{
	if (root_pid == 0)
		return &init_task;

	return find_task_by_vpid(root_pid);
}

/* populate an entry in buf with the info from task_struct p. */
void assignBufferValue(struct prinfo *buf, struct task_struct *p,
		       int *count, int level)
{
	buf[*count].pid = p->pid;
	buf[*count].parent_pid = p->real_parent->pid;
	buf[*count].uid = (int)p->real_cred->uid.val;
	buf[*count].level = level;
	strcpy(buf[*count].comm, p->comm);
	(*count)++;
}

int ptree_bfs_internal(struct prinfo *buffer,
		       struct task_struct *p,
		       int max_entries,
		       int *actual_entries,
		       int *buf_q)
{
	struct list_head *list;
	struct task_struct *task;

	p = get_root(buffer[*buf_q].pid);

	if (p == NULL)
		return -2;

	list_for_each(list, &p->children) {
		if (*actual_entries >= max_entries)
			return -1;
		task = list_entry(list, struct task_struct, sibling);
		assignBufferValue(buffer, task, actual_entries,
				  buffer[*buf_q].level + 1);
	}
	(*buf_q)++;

	return 0;
}

SYSCALL_DEFINE3(ptree, struct prinfo __user *, buf, int __user *, nr,
		int, root_pid)
{
	struct task_struct *root_task;
	int max_entries;
	int actual_entries = 0;
	struct prinfo *buffer;
	struct task_struct *p;
	int buf_q = 0;
	int i;

	if (buf == NULL || nr == NULL)
		return -EINVAL;

	/* copy *nr from user space into max_entries */
	if (copy_from_user(&max_entries, nr, sizeof(int)))
		return -EFAULT;

	if (max_entries < 1)
		return -EINVAL;

	if (root_pid < 0)
		return -EINVAL;

	root_task = get_root(root_pid);
	if (root_task == NULL)
		return -ESRCH;

	buffer = kmalloc_array(max_entries, sizeof(struct prinfo), GFP_KERNEL);
	if (!buffer)
		return -ENOMEM;
	for (i = 0; i < max_entries; i++)
		buffer[i].pid = -1;

	rcu_read_lock();

	p = root_task;
	assignBufferValue(buffer, p, &actual_entries, 0);

	/* do our process BFS:
	 * for each process in buffer (of up to *nr processes):
	 *   for each of its child processes:
	 *     add the child into buffer, for up to *nr total processes
	 */
	while (buffer[buf_q].pid >= 0 && actual_entries < max_entries) {
		int result = ptree_bfs_internal(buffer, p, max_entries,
						&actual_entries, &buf_q);

		if (result == -1)
			break;
		else if (result == -2)
			return -ESRCH;
	}

	rcu_read_unlock();

	/* copy stored prinfo entries into buf, and update *nr. */
	if (copy_to_user(buf, buffer, actual_entries * sizeof(struct prinfo)))
		return -EFAULT;

	if (copy_to_user(nr, &actual_entries, sizeof(int)))
		return -EFAULT;

	kfree(buffer);

	return 0;
}

