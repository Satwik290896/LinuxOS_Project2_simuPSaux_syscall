#f22-hmwk2-team27
## "master" branch
Master branch contains Linux kernal of version "5.10.138" along with an additional system call by name "ptree" with syscall number "441". It is implemented in both x86 and ARM architectures.
##Systemcall "ptree"
A general description of syscall "ptree" is as below - 
```
int ptree(struct prinfo *buf, int *nr, int root_pid);
```
##Arguments of ptree
1. *buf*:  It points to a buffer to store the process tree's data. The data stored inside the buffer will be in BFS order: processes at a higher level (level 0 is considered to be higher than level 10) should appear before processes at a lower level.
2. *nr*:  It represents the allowed size of the buffer 'buf'. The system call copies at most that many entries of the process tree data to the buffer and stores the number of entries actually copied in nr.
3. *root_pid*: It represents the pid of the root of the subtree the syscall is required to traverse. 
##Return Value
System call "ptree" -
1. returns 0 on success.
2. returns EINVAL: if buf or nr are null, or if the number of entries is less than 1
3. returns EFAULT: if buf or nr are outside the accessible address space.



#Linux kernel
============

There are several guides for kernel developers and users. These guides can
be rendered in a number of formats, like HTML and PDF. Please read
Documentation/admin-guide/README.rst first.

In order to build the documentation, use ``make htmldocs`` or
``make pdfdocs``.  The formatted documentation can also be read online at:

    https://www.kernel.org/doc/html/latest/

There are various text files in the Documentation/ subdirectory,
several of them using the Restructured Text markup notation.

Please read the Documentation/process/changes.rst file, as it contains the
requirements for building and running the kernel, and information about
the problems which may result by upgrading your kernel.


