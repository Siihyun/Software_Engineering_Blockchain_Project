#include "swap.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#include "userprog/pagedir.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "threads/interrupt.h"

#define SPP (PGSIZE / BLOCK_SECTOR_SIZE)
#define SWAP_IDX_SIZE (block_size (swap_block) / SPP)
int swap_cnt = 0;
void swap_init () {
    swap_block = block_get_role (BLOCK_SWAP);
    if (swap_block == NULL)
        PANIC ("swap_block failed!");

    is_swapped = malloc (SWAP_IDX_SIZE);

    int i;
    for (i = 0; i < SWAP_IDX_SIZE; i++) {
        is_swapped[i] = false;
    }
    lock_init (&swap_lock);
    return;
}

uint32_t swap_out (void *frame) {
    ASSERT (is_kernel_vaddr (frame));
    uint32_t swap_idx;
    lock_acquire (&swap_lock);
    /* swap disk의 빈 공간을 찾는다. */
    for (swap_idx = 0; swap_idx < SWAP_IDX_SIZE; swap_idx++) {
        if (is_swapped[swap_idx] == false) {
            is_swapped[swap_idx] = true;
            break;
        }
    }
    /* 위에서 찾은 빈 공간에 frame의 내용을 write한다. */
    int i;
    for (i = 0; i < SPP; i++) {
        block_write (swap_block, swap_idx * SPP + i, frame + i * BLOCK_SECTOR_SIZE);
    }
    lock_release (&swap_lock);
    return swap_idx;
}

void swap_in (void *upage, uint32_t swap_idx) {
    ASSERT (is_user_vaddr (upage));
    void *kpage = palloc_get_page (PAL_USER);
    while (kpage == NULL) {
        page_evict_frame ();
        kpage = palloc_get_page (PAL_USER);
    }
    struct page_entry *pte = page_get_entry (upage, thread_tid());
    ASSERT (pte != NULL);
    ASSERT (kpage != NULL);
    int i;
    lock_acquire (&swap_lock);
    for (i = 0; i < SPP; i++) {
        block_read (swap_block, swap_idx * SPP + i, kpage + i * BLOCK_SECTOR_SIZE);
    }
    pagedir_set_page (thread_current()->pagedir, upage, kpage, pte->writable);
    page_set_swap (upage, kpage, thread_tid());
    is_swapped[swap_idx] = false;
    lock_release (&swap_lock);
    return;
}

void swap_free (uint32_t swap_idx) {
    lock_acquire (&swap_lock);
    is_swapped[swap_idx] = false;
    lock_release (&swap_lock);
    return;
}
