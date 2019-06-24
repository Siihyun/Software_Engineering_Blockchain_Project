#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <user/syscall.h>
#include "devices/input.h"
#include "devices/shutdown.h"
#include "lib/kernel/list.h"
#include "threads/interrupt.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "filesys/file.h"
#include "filesys/filesys.h"

static void syscall_handler (struct intr_frame *);
static struct lock open_lock;
static struct lock read_lock;
static struct lock write_lock;
bool is_wait_called_twice[2000];

void
syscall_init (void) 
{
	intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
	lock_init(&open_lock);
	lock_init(&read_lock);
	lock_init(&write_lock);
	int i;
	for(i = 0; i < 2000; i++) {
		is_wait_called_twice[i] = false;
	}
}

bool is_valid_vaddr (void *addr) {
	if(!is_user_vaddr(addr))
		return false;
    void* page = pagedir_get_page(thread_current()->pagedir, addr);

    return page != NULL;
}

int pibonacci(int n){
	int n1 = 1, n2 = 1, n3 = 0;
	if(n == 1 || n == 2) return 1;
	n-=2;
	while(n--){
		n3 = n1 + n2;
		n1 = n2;
		n2 = n3;
	}
	return n3;
}
int sum_of_four_integers(int a,int b,int c,int d){
	return a+b+c+d;
}
void exit(int status)
{
	int i;
	/* close all opened fild */
	for(i = 3; i < thread_current()->cur_fd; i++) {
		struct file *f = thread_current()->f[i];
		if(f != NULL) {
			file_close(f);
		}
	}


    printf("%s: exit(%d)\n", thread_name(), status);

	/* pass exit status to wait() */
	pass_status(SAVE, thread_tid(), status);

	/* wait_sema_up()
	   1. pass information that exit() executed.
	   2. check if sema_down executed for corresponding sema variable.
	   3. if sema_down executed, return.
	      else, sema_up execute and return.
	   1~3 executed atomically. */
	wait_sema_up(sema_user + thread_tid(), thread_tid());

    thread_exit();
}

int wait(tid_t tid)
{
    return process_wait(tid);
}

int write(int fd, const void *buffer, unsigned size) {
    if(fd == 1) { // fd 3~128 
        putbuf(buffer, size);
        return size;
    }
	if(thread_current()->f[fd] == NULL)
		exit(-1);
	else {
		if (file_writable(thread_current()->f[fd]) == false) {
			int res;
			/* critical section */
			lock_acquire(&write_lock);
			res = file_write(thread_current()->f[fd], buffer, size);
			lock_release(&write_lock);
			return res;
		}
	}
    return 0;
}

int read (int fd, void *buffer, unsigned size){
	int i;
	if(fd == 0){
		for(i = 0; i < size ; i++)
			((char *)buffer)[i] = input_getc();
		return size;
	}
	else if(fd == 1) {
		exit(-1);
	}
	else {
		int res;
		/* critical section */
		lock_acquire(&read_lock);
		res = file_read(thread_current()->f[fd], buffer, size);
		lock_release(&read_lock);
		return res;
	}
}

unsigned tell (int fd) {
	if(thread_current()->f[fd])
		return file_tell(thread_current()->f[fd]); 
}

int open(const char *file){
	int i;
	struct file *f;
	if(file == NULL) {
		exit(-1);
	}
	/* critical section */
	lock_acquire(&open_lock);
	f = filesys_open(file);
	lock_release(&open_lock);
	thread_current()->f[thread_current()->cur_fd] = f;
	
	if(thread_current()->f[thread_current()->cur_fd] == NULL) {
		return -1;
	}
	if(thread_get_tid_from_name(file) != -1) {
		file_deny_write(thread_current()->f[thread_current()->cur_fd]);
	}
	else {
		file_allow_write(thread_current()->f[thread_current()->cur_fd]);
	}
	return thread_current()->cur_fd++;
}

bool create(const char* file, unsigned initial_size) {
	if(file == NULL) {
		exit(-1);
	}
	return filesys_create(file, initial_size);
}

int filesize(int fd) {
	return file_length(thread_current()->f[fd]);
}

bool remove(const char *file) {
	return filesys_remove(file);
}

void seek(int fd, unsigned position) {
	file_seek(thread_current()->f[fd], position);
}
bool wait_check = false;
	static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	void *addr = (f->esp);
	int tmp;
	if(!is_valid_vaddr(f->esp) || (f->esp>=0xbffffffc) && *(int *)(f->esp)!=0)
		exit(-1);
	int syscall_num = (*(uint64_t *)addr);
    struct thread *t = thread_current();

	int fnum = thread_current()->cur_fd;
	switch(syscall_num){
		case SYS_HALT:
            shutdown_power_off();
			break;
		case SYS_EXIT:
			if(!is_valid_vaddr(f->esp + 4))
				exit(-1);
            exit((int)*(uint64_t *)(f->esp + 4));
			break;
		case SYS_EXEC:
			if(!is_valid_vaddr(f->esp + 4) || !is_valid_vaddr(*(int *)(f->esp+4)))
				exit(-1);
            f->eax = process_execute((char*)*(uint64_t *)(f->esp+4));
			break;
		case SYS_WAIT:
			if(!is_valid_vaddr(f->esp + 4))
				exit(-1);
			if(is_wait_called_twice[*(tid_t*)(f->esp + 4)])
				f->eax = -1;
			else {
				is_wait_called_twice[*(tid_t*)(f->esp + 4)] = true;
				f->eax = wait(*(tid_t *)(f->esp + 4));
			}
            break;
		case SYS_PIBONACCI:
			if(!is_valid_vaddr(f->esp + 4))
				exit(-1);
			f->eax = pibonacci(*(int *)(f->esp + 4));
			break;
		case SYS_SUM_OF_FOUR_INTEGERS:
			if(!is_valid_vaddr(f->esp+4) || !is_valid_vaddr(f->esp+8) || !is_valid_vaddr(f->esp + 12) || !is_valid_vaddr(f->esp + 16))
				exit(-1);
			f->eax = sum_of_four_integers(*(int *)(f->esp + 4),*(int *)(f->esp+8),*(int *)(f->esp+12),*(int *)(f->esp+16));
			break;
		case SYS_CREATE:
			if(!is_valid_vaddr(*(char**)(f->esp + 4)))
				exit(-1);
			f->eax = create(*(char**)(f->esp + 4), *(unsigned*)(f->esp + 8));
			break;
		case SYS_REMOVE:
			f->eax = remove(*(char**)(f->esp + 4));
			break;
		case SYS_OPEN:
			if(!is_valid_vaddr(*(char**)(f->esp + 4)))
				exit(-1);
			f->eax = open(*(char**)(f->esp + 4));
			break;
		case SYS_FILESIZE:
			f->eax = filesize(*(int*)(f->esp + 4));
			break;
		case SYS_READ:
			if(!is_valid_vaddr(*(void**)(f->esp + 28)) || !is_valid_vaddr(f->esp + 32)) exit(-1);
            f->eax = read((int)*(uint32_t *)(f->esp+24), (void*)*(uint32_t*)(f->esp + 28), (unsigned)*((uint32_t*)(f->esp + 32)));
			break;
		case SYS_WRITE:
            if(!is_valid_vaddr((char **)(f->esp + 28)) || !is_valid_vaddr(*(char**)(f->esp + 28)) || !is_valid_vaddr(f->esp + 32))
                exit(-1);
            f->eax = write((int)*(uint32_t *)(f->esp+24), (void*)*(uint32_t*)(f->esp + 28), (unsigned)*((uint32_t*)(f->esp + 32)));
			break;
		case SYS_SEEK:
			if(thread_current()->f[*(int*)(f->esp + 4)])
				seek(*(int*)(f->esp + 4), *(unsigned*)(f->esp + 8));
			break;
		case SYS_TELL:
			f->eax = tell(*(int*)(f->esp + 4));
			break;
		case SYS_CLOSE:
			if(*(unsigned*)(f->esp + 4) < 140 &&
					thread_current()->f[*(unsigned*)(f->esp + 4)] != NULL) {
				file_close(thread_current()->f[*(unsigned*)(f->esp + 4)]);
				thread_current()->f[*(unsigned*)(f->esp + 4)] = NULL;
			}
			else
				exit(-1);
			break;
		default:
			break;
	}
}
