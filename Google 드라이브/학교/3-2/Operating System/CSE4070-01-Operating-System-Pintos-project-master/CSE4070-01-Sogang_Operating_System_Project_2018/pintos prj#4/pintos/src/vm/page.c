#include "page.h"
//#include "frame.h"
#include "swap.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "userprog/pagedir.h"
#include "threads/vaddr.h"
#include "threads/interrupt.h"

struct hash pt;
bool pt_less_func (const struct hash_elem *a, 
        const struct hash_elem *b, void *aux) {
    uint64_t aidx = page_get_idx(hash_entry(a, struct page_entry, elem));
    uint64_t bidx = page_get_idx(hash_entry(b, struct page_entry, elem));
    if (aidx < bidx)    return true;
    else                return false;
}

unsigned pt_hash_func (const struct hash_elem *e, void *aux) {
    uint32_t idx = page_get_idx(hash_entry(e, struct page_entry, elem));
    return hash_bytes (&idx, sizeof idx);
}

void page_table_init () {
    hash_init (&pt, &pt_hash_func, &pt_less_func, NULL);
    frame_for_swap = palloc_get_page (PAL_USER);
    lock_init (&page_lock);
}

uint32_t get_page_number (void* addr) {
    return ((uint32_t)addr & 0xfffff000) >> 12;
}

struct page_entry *page_get_entry (void *vaddr, uint32_t pid) {
    lock_acquire (&page_lock);
    struct hash_elem *he = get_hash_elem (vaddr, pid);
    lock_release (&page_lock);
    if (he != NULL)
        return hash_entry (he, struct page_entry, elem);
    else
        return NULL;
}

struct hash_elem *get_hash_elem (void *vaddr, uint32_t pid) {
    struct page_entry pte;
    pte.vpn = get_page_number (vaddr);
    pte.pid = pid;
    return hash_find(&pt, &pte.elem);
}

void page_insert (void *upage, void *knpage, bool writable) {
    ASSERT (upage != NULL);
    struct page_entry *pte = malloc (sizeof (struct page_entry));
    ASSERT (pte != NULL);

    if (knpage != frame_for_swap) {         // knpage : page.h에 전역변수로 선언
        pte->ppn = get_page_number (knpage);
        pte->swap_idx = -1;
        pte->is_on_disk = false;
    }
    else {
        pte->ppn = 0;
        pte->swap_idx = swap_out (knpage);  // knpage는 load_segment에서 채워짐
        pte->is_on_disk = true;
    }
    pte->pinned = false;
    pte->vpn = get_page_number (upage);
    pte->pid = thread_tid ();
    pte->writable = writable;
    pte->elem.list_elem.next = NULL;
    pte->elem.list_elem.prev = NULL;
    pte->t = thread_current();
    lock_acquire (&page_lock);
    if (hash_insert(&pt, &(pte->elem)) == NULL)
        ASSERT (pte);
    lock_release (&page_lock);
    return;
}

bool page_delete (void *upage, uint32_t pid) {
    lock_acquire (&page_lock);
    struct hash_elem *target = get_hash_elem (upage, pid);
    lock_release (&page_lock);
    if (target != NULL) {
        lock_acquire (&page_lock);
        hash_delete (&pt, target);
        lock_release (&page_lock);
        free (hash_entry (target, struct page_entry, elem));
        return true;
    }
    return false;
}

bool page_destroy_by_pid (uint32_t pid) {
    lock_acquire (&page_lock);
    bool deleted = true;
    while (deleted == true) {
        deleted = false;
        struct hash_iterator i;

        hash_first (&i, &pt);
        while (hash_next (&i)) {
            struct page_entry *pte = hash_entry (hash_cur (&i), struct page_entry, elem);
            if (pte->pid == pid) {
                if (pte->swap_idx != -1) {
                    swap_free (pte->swap_idx);
                }
                struct hash_elem *h = hash_cur (&i);
                hash_delete (&pt, &pte->elem);
                free (hash_entry (h, struct page_entry, elem));
                deleted = true;
                break;
            }
        }
    }
    lock_release (&page_lock);
    return true;
}

bool page_evict_frame () {  // randomly evict
    struct hash_iterator i;
    struct thread *cur = thread_current();
    struct page_entry *pte = NULL;

    while (1) {
        hash_first (&i, &pt);
        while (hash_next (&i)) {
            pte = hash_entry (hash_cur (&i), struct page_entry, elem);
            if (pte->pinned == false && pte->ppn != 0) {
                if (thread_get_ticks () % 3 == 0)
                      continue;
                pte->swap_idx = swap_out (pte->ppn << 12);
                pagedir_clear_page (pte->t->pagedir, (void *)(pte->vpn << 12));
                palloc_free_page ((void *)(pte->ppn << 12));
                pte->ppn = 0;
                return true;
            } 
        }
    }
    PANIC ("page_evict_frame fail!");
}

void page_set_swap (void *upage, void *kpage, uint32_t pid) {
    struct hash_elem *e = get_hash_elem (upage, pid);
    ASSERT (e != NULL);
    struct page_entry *pte = hash_entry (e, struct page_entry, elem);
    pte->ppn = get_page_number (kpage);
    pte->swap_idx = -1;
}
/*
void page_set_all_accessd_false () {
    lock_acquire (&page_lock);
    struct thread *cur = thread_current();
    struct hash_iterator i;
    hash_first (&i, &pt);
    while (hash_next (&i)) {
        struct page_entry *pte = hash_entry (hash_cur (&i), struct page_entry, elem);
        pagedir_set_accessd (cur->pagedir, (void *)(pte->vpn << 12), false);
    }
    lock_release (&page_lock);
}*/
    
uint32_t page_get_idx (const struct page_entry *pte) {
    uint32_t idx = (uint32_t)(pte->vpn);
    idx = idx << 12;
    idx += pte->pid;
    ASSERT (pte->pid < 0x1000);
    return idx;
}

bool page_swap_in_and_pinning (void *upage, uint32_t pid) {
    struct page_entry *pte = page_get_entry (upage, pid);
    lock_acquire (&swap_lock);
    lock_release (&swap_lock);
    if (pte == NULL)
        return false;

    pte->pinned = true;

    if (pte->ppn != 0)
        return true;

    swap_in (upage, pte->swap_idx);
    return true;
}   

void page_pinning_buffers (void *buffer_, unsigned size_) {
    uint32_t buffer = ((uint32_t) buffer_ >> 12) << 12;
    int size = size_ + (uint32_t) buffer_ - buffer;
    uint32_t pid = thread_tid();
    while (size > 0) {
        page_swap_in_and_pinning ((void*) buffer, pid);
        buffer += PGSIZE;
        size -= PGSIZE;
    }
}

void page_unpinning_buffers (void *buffer_, unsigned size_) {
    uint32_t buffer = ((uint32_t) buffer_ >> 12) << 12;
    int size = size_ + (uint32_t) buffer_ - buffer;
    uint32_t pid = thread_tid();
    while (size > 0) {
        struct page_entry *pte = page_get_entry (buffer, pid);
        ASSERT (pte != NULL);
        pte->pinned = false;
        buffer += PGSIZE;
        size -= PGSIZE;
    }
}

int page_swap_in_all () {
    struct thread *cur = thread_current();
    struct hash_iterator i;
    void *p;
    int cnt = 0;
    int res  = 0;
    hash_first (&i, &pt);
    while (hash_next (&i)) {
        struct page_entry *pte = hash_entry (hash_cur (&i), struct page_entry, elem);
        if (pte->ppn == 0) {
            is_swapped[pte->swap_idx] = true;
        }
        
    }
    return res;
}
