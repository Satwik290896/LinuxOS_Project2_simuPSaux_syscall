#ifndef __PRINTFO_H
#define __PRINTFO_H

#include <linux/sched.h>
#include <unistd.h>

struct prinfo {
        pid_t parent_pid;       /* process id of parent */
        pid_t pid;              /* process id */
        uid_t uid;              /* user id of process owner */
        char comm[16];          /* name of program executed */
        int level;              /* level of this process in the subtree */
};
#endif