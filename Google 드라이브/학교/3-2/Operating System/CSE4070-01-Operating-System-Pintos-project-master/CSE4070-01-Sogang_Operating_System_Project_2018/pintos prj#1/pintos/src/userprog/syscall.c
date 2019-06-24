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

static void syscall_handler (struct intr_frame *);

	void
syscall_init (void) 
{
	intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

bool is_valid_vaddr (void *addr) {
    void* page = pagedir_get_page(thread_current()->pagedir, addr);
    DEBUG("addr : %p, page : %p\n", addr, page);
    return page != NULL && is_user_vaddr(addr);
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
    printf("%s: exit(%d)\n", thread_name(), status); 
    is_child_alive(thread_tid(), 2, &status);
    is_child_alive(thread_tid(), -1, NULL);
    thread_exit();
}
int wait(tid_t tid)
{
    return process_wait(tid);
}
int write(int fd, const void *buffer, unsigned size) {
    if(fd == 1) {
        putbuf(buffer, size);
        return size;
    }
    return -1;
}
int read (int fd, void *buffer,unsigned size){
	int i;
	if(fd == 0){
		for(i = 0; i < size ; i++)
			((char *)buffer)[i] = input_getc();
		return size;
	}
	return -1;
}

	static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	void *addr = (f->esp);
	int tmp;
	if(!is_valid_vaddr(f->esp) || (f->esp>=0xbffffffc) && *(int *)(f->esp)!=0)
		exit(-1);
	int syscall_num = (*(uint64_t *)addr);
    struct thread *t = thread_current();
	

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
			f->eax = wait(*(tid_t *)(f->esp + 4));
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
			break;
		case SYS_REMOVE:
			break;
		case SYS_OPEN:
			break;
		case SYS_FILESIZE:
			break;
		case SYS_READ:
			if(!is_valid_vaddr(*(char**)(f->esp + 28)) || !is_valid_vaddr(f->esp + 32))
                exit(-1);
            f->eax = read((int)*(uint32_t *)(f->esp+24), (void*)*(uint32_t*)(f->esp + 28), (unsigned)*((uint32_t*)(f->esp + 32)));
			break;
		case SYS_WRITE:
            if(!is_valid_vaddr(*(char**)(f->esp + 28)) || !is_valid_vaddr(f->esp + 32))
                exit(-1);
            f->eax = write((int)*(uint32_t *)(f->esp+24), (void*)*(uint32_t*)(f->esp + 28), (unsigned)*((uint32_t*)(f->esp + 32)));
			break;
		case SYS_SEEK:
			break;
		case SYS_TELL:
			break;
		case SYS_CLOSE:
			break;
		default:
			break;
	}
    END("syscall_handler\n");
}
