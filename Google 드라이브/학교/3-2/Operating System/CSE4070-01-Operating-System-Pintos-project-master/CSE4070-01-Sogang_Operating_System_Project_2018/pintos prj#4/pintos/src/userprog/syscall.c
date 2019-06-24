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
#include "threads/palloc.h"
#include "threads/pte.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "vm/page.h"
#include "vm/swap.h"

static void syscall_handler (struct intr_frame *);
static struct lock open_lock;
static struct lock read_lock;
static struct lock write_lock;
static struct lock close_lock;
static struct semaphore file_sema;
bool is_wait_called_twice[2000];

void
syscall_init (void) 
{
	intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
	lock_init(&open_lock);
	lock_init(&read_lock);
	lock_init(&write_lock);
    lock_init(&close_lock);
    lock_init (&file_lock);
    sema_init (&file_sema, 0);
	int i;
	for(i = 0; i < 2000; i++) {
		is_wait_called_twice[i] = false;
	}
}

bool is_valid_vaddr (void *addr) {
//    printf ("addr : %p\n\n", addr);
	if(!is_user_vaddr(addr))
		return false;
    if (page_get_entry (((uint32_t)addr >> 12) << 12, thread_tid()) == NULL)
        return false;
    return true;
//    void* page = pagedir_get_page(thread_current()->pagedir, addr);
//    printf ("page : %p\n\n", page);
//    return page != NULL;
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
void munmap(int mapping);

void exit(int status)
{
	int i;
	/* close all opened fild */
	for(i = 3; i < thread_current()->cur_fd; i++) {
		struct file *f = thread_current()->f[i];
		if(f != NULL) {
            if (thread_current()->mbuffer[i] != NULL)
                munmap(i);
            lock_acquire (&file_lock);
            file_close(f);
            lock_release (&file_lock);
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
//			lock_acquire(&write_lock);
			lock_acquire (&file_lock);
            page_pinning_buffers (buffer, size);
			res = file_write(thread_current()->f[fd], buffer, size);
            lock_release (&file_lock);
//            page_unpinning_buffers (buffer, size);
//			lock_release(&write_lock);
			return res;
		}
	}
    return 0;
}

int read (int fd, void *buffer, unsigned size){
	int i;
//    if (page_get_entry ((buffer >> 12) << 12, thread_tid()) == NULL)
//        exit (-1);
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
//		lock_acquire(&read_lock);
//        printf ("%p, %d\n\n", buffer, size);
        lock_acquire (&file_lock);
        page_pinning_buffers (buffer, size);
		res = file_read(thread_current()->f[fd], buffer, size);
        lock_release (&file_lock);
//        page_unpinning_buffers (buffer, size);
//		lock_release(&read_lock);
		return res;
	}
}

unsigned tell (int fd) {
	if(thread_current()->f[fd])
		return file_tell(thread_current()->f[fd]); 
}

int open(const char *file){
//    printf ("(open) file : %s\n", file);
	int i;
	struct file *f;
	if(file == NULL) {
		exit(-1);
	}
	/* critical section */
//	lock_acquire(&open_lock);
	lock_acquire (&file_lock);
	f = filesys_open(file);
    lock_release (&file_lock);
//	lock_release(&open_lock);
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
//    printf ("(create) file : %s\n", file);
	if(file == NULL) {
		exit(-1);
	}
    lock_acquire (&file_lock);
    bool res = filesys_create (file, initial_size);
    lock_release (&file_lock);
	return res;
}

int filesize(int fd) {
    lock_acquire (&file_lock);
    int res = file_length (thread_current()->f[fd]);
    lock_release (&file_lock);
	return res;
}

bool remove(const char *file) {
	return filesys_remove(file);
}

void seek(int fd, unsigned position) {
    lock_acquire (&file_lock);
	file_seek(thread_current()->f[fd], position);
    lock_release (&file_lock);
}

int mmap (int fd, void *addr) {
    lock_acquire (&file_lock);
    struct file *file = file_reopen (thread_current()->f[fd]);
    ASSERT (file != NULL);

    if ((uint32_t) addr % PGSIZE != 0)
        return -1;
    
    size_t read_bytes = file_length (file);
    size_t zero_bytes = PGSIZE - (read_bytes % PGSIZE);
    uint8_t *upage = addr;

    thread_current()->msize[fd] = read_bytes;

    file_seek (file, 0);

    while (read_bytes > 0 || zero_bytes > 0) {

        size_t page_read_bytes = read_bytes < PGSIZE ? read_bytes : PGSIZE;
        size_t page_zero_bytes = PGSIZE - page_read_bytes;

        uint8_t *knpage = palloc_get_page (PAL_USER);
        while (knpage == NULL) {
            page_evict_frame ();
            knpage = palloc_get_page (PAL_USER);
        }
        
        int temp = file_read (file, knpage, page_read_bytes);
        if (temp != (int) page_read_bytes) {
            palloc_free_page (knpage);
            return -1;
        }

        memset (knpage + page_read_bytes, 0, page_zero_bytes);

        pagedir_set_page (thread_current()->pagedir, upage, knpage, true);
        page_insert (upage, knpage, true);

        read_bytes -= page_read_bytes;
        zero_bytes -= page_zero_bytes;
        upage += PGSIZE;
    }
    thread_current()->mbuffer[fd] = addr;
    lock_release (&file_lock);
    return fd;
}


void munmap (int mapping) {
    int i;
    int size = filesize(mapping);
    void *buffer = thread_current()->mbuffer[mapping];
    ASSERT (buffer != NULL);
    lock_acquire (&file_lock);
    page_pinning_buffers (buffer, size);
    file_write (thread_current()->f[mapping], buffer, size);
    lock_release (&file_lock);
    return;
}

bool wait_check = false;
	static void
syscall_handler (struct intr_frame *f UNUSED) 
{
//    printf ("(syscall) start\n");
	void *addr = (f->esp);
	int tmp;
    int handle;
    unsigned char *p;
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
//            printf ("create : %d\n", thread_tid());
			if(!is_valid_vaddr(*(char**)(f->esp + 4)))
				exit(-1);
			f->eax = create(*(char**)(f->esp + 4), *(unsigned*)(f->esp + 8));
			break;
		case SYS_REMOVE:
			f->eax = remove(*(char**)(f->esp + 4));
			break;
		case SYS_OPEN:
//            printf ("open : %d\n", thread_tid());
//            sema_down (&file_sema);
			if(!is_valid_vaddr(*(char**)(f->esp + 4)))
				exit(-1);
			f->eax = open(*(char**)(f->esp + 4));
			break;
		case SYS_FILESIZE:
			f->eax = filesize(*(int*)(f->esp + 4));
			break;
		case SYS_READ:
			if(!is_valid_vaddr(*(void**)(f->esp + 28)) || !is_valid_vaddr(f->esp + 32)) {
                exit(-1);
            }
            f->eax = read((int)*(uint32_t *)(f->esp+24), (void*)*(uint32_t*)(f->esp + 28), (unsigned)*((uint32_t*)(f->esp + 32)));
			break;
		case SYS_WRITE:
//            printf ("write : %d\n", thread_tid());
            if(!is_valid_vaddr((char **)(f->esp + 28)) || !is_valid_vaddr(*(char**)(f->esp + 28)) || !is_valid_vaddr(f->esp + 32))
                exit(-1);
            f->eax = write((int)*(uint32_t *)(f->esp+24), (void*)*(uint32_t*)(f->esp + 28), (unsigned)*((uint32_t*)(f->esp + 32)));
			break;
		case SYS_SEEK:
			if(thread_current()->f[*(int*)(f->esp + 4)])
				seek(*(int*)(f->esp + 4), *(unsigned*)(f->esp + 8));
			break;
		case SYS_TELL:
            lock_acquire (&file_lock);
			f->eax = tell(*(int*)(f->esp + 4));
            lock_release (&file_lock);
			break;
		case SYS_CLOSE:
//            printf ("close : %d\n", thread_tid());
			if(*(unsigned*)(f->esp + 4) < 140 &&
					thread_current()->f[*(unsigned*)(f->esp + 4)] != NULL) {
                lock_acquire (&file_lock);
				file_close(thread_current()->f[*(unsigned*)(f->esp + 4)]);
                lock_release (&file_lock);
//                sema_up (&file_sema);
				thread_current()->f[*(unsigned*)(f->esp + 4)] = NULL;
			}
			else
				exit(-1);
			break;
        case SYS_MMAP:
            handle = *(int*)(f->esp + 4);
            p = *(unsigned char **)(f->esp + 8);
            f->eax = mmap(handle, p);
            break;
        case SYS_MUNMAP:
            munmap(*(int*)(f->esp + 4));
//            printf ("sdfsdf\n");
            break;
		default:
			break;
	}
}
