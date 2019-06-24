#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <stdint.h>
#include <debug.h>
#include <hash.h>
#include "threads/synch.h"

struct page_entry {
    uint32_t vpn;   // virtual page number
    uint32_t ppn;   // physical page number
    uint32_t pid;
    struct hash_elem elem;
    int32_t swap_idx;   // if page -> frame, idx = -1
                        // otherwise, idx >= 0
    struct thread *t;
    bool pinned;
    bool is_on_disk;
    bool writable;
};

struct lock page_lock;
void *frame_for_swap;


void page_table_init ();
void page_insert (void *upage, void *knpage, bool writable);
bool page_delete (void *upage, uint32_t pid);
bool page_destroy_by_pid (uint32_t pid);
bool page_evict_frame ();
void page_set_all_accessd_false ();
void page_set_swap (void *upage, void *kpage, uint32_t pid);
bool page_swap_in_and_pinning (void *upage, uint32_t pid);
void page_pinning_buffers (void *buffer_, unsigned size_);
void page_unpinning_buffers (void *buffer_, unsigned size_);
int page_swap_in_all ();
uint32_t get_page_number (void *);
uint32_t page_get_idx(const struct page_entry *pte);
struct page_entry *page_get_entry (void *vaddr, uint32_t pid);
struct hash_elem *get_hash_elem (void *vaddr, uint32_t pid);

#endif
