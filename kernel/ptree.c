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

void assignBufferValue(struct prinfo *buf, struct task_struct *p, int *count, int level){
	buf[*count].pid = p->pid;
	buf[*count].parent_pid = p->real_parent->pid;
	buf[*count].uid = (int)p->real_cred->uid.val;
	buf[*count].level = level;
	strcpy(buf[*count].comm, p->comm);
	/* printk("%d, %d, %d, %d, %s\n\n", buf[*count].pid, buf[*count].parent_pid, buf[*count].uid, level, buf[*count].comm);
	printk("%d\n", *count); */
	(*count)++;
}

SYSCALL_DEFINE3(ptree, struct prinfo __user *, buf, int __user *, nr, int, root_pid)
{
	struct task_struct *root_task = get_root(root_pid);
	int max_entries;
	int actual_entries = 0;
	struct prinfo *buffer;
	struct task_struct *p, *task;
	struct list_head *list;
	int buf_q = 0;
	int i;

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
	buffer = kmalloc(max_entries * sizeof(struct prinfo), GFP_KERNEL);
	if(!buffer)
		return -EFAULT;
	for(i = 0; i < max_entries; i++)
		buffer[i].pid = -1;

	rcu_read_lock();

	/* TODO: from root_task, do an iterative BFS.
	 * for each task_struct that we iterate on:
	 *   populate a prinfo entry, store it in our allocated buffer at index [actual_entries]
	 *   increment actual_entries
	 *   if actual_entries == max_entries, stop iterating.
	 * be sure not to do anything blocking (like allocation) in this section
	 * between rcu_read_lock and rcu_read_unlock :)
	 */
	p = root_task;
	assignBufferValue(buffer, p, &actual_entries, 0);

	while(buffer[buf_q].pid >= 0 && actual_entries < max_entries){
		// printk("parent pid: %d", buffer[buf_q].pid);
		p = get_root(buffer[buf_q].pid);
		list_for_each(list, &p->children){
			if(actual_entries >= max_entries)
				break;
			task = list_entry(list, struct task_struct, sibling);
			assignBufferValue(buffer, task, &actual_entries, buffer[buf_q].level + 1);
		}
		buf_q++;
	}
	
	rcu_read_unlock();

    /* TODO: copy our stored prinfo entries into *buf using copy_to_user.
	 *  if this fails, return -EFAULT
	 * then, delete our allocated buffer
	 */
	if(copy_to_user(buf, buffer, actual_entries * sizeof(struct prinfo)))
		return -EFAULT;

	/* update *nr with how many entries were copied */
	if (copy_to_user(nr, &actual_entries, sizeof(int)))
		return -EFAULT;
	kfree(buffer);
	/* delete this later, this is just to silence unused variable warnings for now */
	(void) root_task;
	(void) actual_entries;

	return 0;
}

