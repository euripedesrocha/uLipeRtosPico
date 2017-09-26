/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_mem_dyn.c
 *
 *  @brief dynamic memory block allocator based on TLSF strategy
 *
 *
 *  Copyright (c) 2016 National Cheng Kung University, Taiwan.
 *  Copyright (c) 2006-2008, 2011, 2014 Matthew Conte.
 *  All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license that can be
 *  found in the LICENSE file.
 *
 *
 *
 *  TLSF-BSD is freely redistributable under the two-clause BSD License:
 *
 *	Copyright (c) 2016 National Cheng Kung University, Taiwan.
 *	Copyright (c) 2006-2008, 2011, 2014 Matthew Conte.
 *	All rights reserved.
 *
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions
//	are met:
//	1. Redistributions of source code must retain the above copyright
//	   notice, this list of conditions and the following disclaimer.
//	2. Redistributions in binary form must reproduce the above copyright
//	   notice, this list of conditions and the following disclaimer in the
//	   documentation and/or other materials provided with the distribution.
//	 *
//	THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
//	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//	ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
//	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
//	OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//	HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//	LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
//	OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//	SUCH DAMAGE.
 *
 *
 *
 */




#include "ulipe_rtos_pico.h"

#if (K_ENABLE_DYNAMIC_ALLOCATOR > 0)




#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#define tlsf_assert(expr)

typedef void *tlsf_t;
typedef void *pool_t;

/* Public constants: may be modified. */
enum tlsf_public {
    /* log2 of number of linear subdivisions of block sizes. Larger
     * values require more memory in the control structure. Values of
     * 4 or 5 are typical.
    */
    SL_INDEX_COUNT_LOG2 = 3,
};

/* Private constants: do not modify. */
enum tlsf_private {
#if defined(TLSF_64BIT)
    /* All allocation sizes and addresses are aligned to 8 bytes. */
    ALIGN_SIZE_LOG2 = 3,
#else
    /* All allocation sizes and addresses are aligned to 4 bytes. */
    ALIGN_SIZE_LOG2 = 2,
#endif
    ALIGN_SIZE = (1 << ALIGN_SIZE_LOG2),

/*
 * We support allocations of sizes up to (1 << FL_INDEX_MAX) bits.
 * However, because we linearly subdivide the second-level lists, and
 * our minimum size granularity is 4 bytes, it doesn't make sense to
 * create first-level lists for sizes smaller than SL_INDEX_COUNT * 4,
 * or (1 << (SL_INDEX_COUNT_LOG2 + 2)) bytes, as there we will be
 * trying to split size ranges into more slots than we have available.
 * Instead, we calculate the minimum threshold size, and place all
 * blocks below that size into the 0th first-level list.
 */

#if defined(TLSF_64BIT)
    /*
     * TODO: We can increase this to support larger sizes, at the expense
     * of more overhead in the TLSF structure.
     */
    FL_INDEX_MAX = 32,
#else
    FL_INDEX_MAX = 17,
#endif
    SL_INDEX_COUNT = (1 << SL_INDEX_COUNT_LOG2),
    FL_INDEX_SHIFT = (SL_INDEX_COUNT_LOG2 + ALIGN_SIZE_LOG2),
    FL_INDEX_COUNT = (FL_INDEX_MAX - FL_INDEX_SHIFT + 1),

    SMALL_BLOCK_SIZE = (1 << FL_INDEX_SHIFT),
};

/*
 * Cast and min/max macros.
 */
#define tlsf_cast(t, exp) ((t)(exp))
#define tlsf_min(a, b) ((a) < (b) ? (a) : (b))
#define tlsf_max(a, b) ((a) > (b) ? (a) : (b))

/*
 * Static assertion mechanism.
 */



/*
 * Data structures and associated constants.
 */

/*
 * Block header structure.
 *
 * There are several implementation subtleties involved:
 * - The prev_phys_block field is only valid if the previous block is free.
 * - The prev_phys_block field is actually stored at the end of the
 *   previous block. It appears at the beginning of this structure only to
 *   simplify the implementation.
 * - The next_free / prev_free fields are only valid if the block is free.
 */
typedef struct block_header_t {
    /* Points to the previous physical block. */
    struct block_header_t *prev_phys_block;

    /* The size of this block, excluding the block header. */
    size_t size;

    /* Next and previous free blocks. */
    struct block_header_t *next_free;
    struct block_header_t *prev_free;
} block_header_t;

/*
 * Since block sizes are always at least a multiple of 4, the two least
 * significant bits of the size field are used to store the block status:
 * - bit 0: whether block is busy or free
 * - bit 1: whether previous block is busy or free
 */
static const size_t block_header_free_bit = 1 << 0;
static const size_t block_header_prev_free_bit = 1 << 1;

/*
 * The size of the block header exposed to used blocks is the size field.
 * The prev_phys_block field is stored *inside* the previous free block.
 */
static const size_t block_header_overhead = sizeof(size_t);

/* User data starts directly after the size field in a used block. */
static const size_t block_start_offset =
    offsetof(block_header_t, size) + sizeof(size_t);

/*
 * A free block must be large enough to store its header minus the size of
 * the prev_phys_block field, and no larger than the number of addressable
 * bits for FL_INDEX.
 */
static const size_t block_size_min =
    sizeof(block_header_t) - sizeof(block_header_t *);
static const size_t block_size_max = tlsf_cast(size_t, 1) << FL_INDEX_MAX;

/* The TLSF control structure. */
typedef struct control_t {
    /* Empty lists point at this block to indicate they are free. */
    block_header_t block_null;

    /* Bitmaps for free lists. */
    unsigned int fl_bitmap;
    unsigned int sl_bitmap[FL_INDEX_COUNT];

    /* Head of free lists. */
    block_header_t *blocks[FL_INDEX_COUNT][SL_INDEX_COUNT];
} control_t;

/* A type used for casting when doing pointer arithmetic. */
typedef ptrdiff_t tlsfptr_t;


static uint8_t k_heap[K_HEAP_SIZE + sizeof(control_t)];
static tlsf_t k_mem;


/* find first set:
 *   __ffs(1) == 0, __ffs(0) == -1, __ffs(1<<31) == 31
 */
static inline int tlsf_ffs(unsigned int word)
{
    return((word == 0)? -1: port_bit_ls_scan(word));
}

static inline int tlsf_fls(unsigned int word)
{
    return((word == 0)? 0: 31  - port_bit_fs_scan(word));
}

/*
 * block_header_t member functions.
 */

static size_t block_size(const block_header_t *block)
{
    return block->size & ~(block_header_free_bit | block_header_prev_free_bit);
}

static void block_set_size(block_header_t *block, size_t size)
{
    const size_t oldsize = block->size;
    block->size =
        size | (oldsize & (block_header_free_bit | block_header_prev_free_bit));
}


static int block_is_free(const block_header_t *block)
{
    return tlsf_cast(int, block->size &block_header_free_bit);
}

static void block_set_free(block_header_t *block)
{
    block->size |= block_header_free_bit;
}

static void block_set_used(block_header_t *block)
{
    block->size &= ~block_header_free_bit;
}

static int block_is_prev_free(const block_header_t *block)
{
    return tlsf_cast(int, block->size &block_header_prev_free_bit);
}

static void block_set_prev_free(block_header_t *block)
{
    block->size |= block_header_prev_free_bit;
}

static void block_set_prev_used(block_header_t *block)
{
    block->size &= ~block_header_prev_free_bit;
}

static block_header_t *block_from_ptr(const void *ptr)
{
    return tlsf_cast(block_header_t *,
                     tlsf_cast(unsigned char *, ptr) - block_start_offset);
}

static void *block_to_ptr(const block_header_t *block)
{
    return tlsf_cast(void *,
                     tlsf_cast(unsigned char *, block) + block_start_offset);
}

/* Return location of next block after block of given size. */
static block_header_t *offset_to_block(const void *ptr, tlsfptr_t size)
{
    return tlsf_cast(block_header_t *, tlsf_cast(tlsfptr_t, ptr) + size);
}

/* Return location of previous block. */
static block_header_t *block_prev(const block_header_t *block)
{
    return block->prev_phys_block;
}

/* Return location of next existing block. */
static block_header_t *block_next(const block_header_t *block)
{
    block_header_t *next = offset_to_block(
        block_to_ptr(block), block_size(block) - block_header_overhead);
    tlsf_assert(!block_is_last(block));
    return next;
}

/* Link a new block with its physical neighbor, return the neighbor. */
static block_header_t *block_link_next(block_header_t *block)
{
    block_header_t *next = block_next(block);
    next->prev_phys_block = block;
    return next;
}

static void block_mark_as_free(block_header_t *block)
{
    /* Link the block to the next block, first. */
    block_header_t *next = block_link_next(block);
    block_set_prev_free(next);
    block_set_free(block);
}

static void block_mark_as_used(block_header_t *block)
{
    block_header_t *next = block_next(block);
    block_set_prev_used(next);
    block_set_used(block);
}

static size_t align_up(size_t x, size_t align)
{
    return (x + (align - 1)) & ~(align - 1);
}

static size_t align_down(size_t x, size_t align)
{
    return x - (x & (align - 1));
}


/*
 * Adjust an allocation size to be aligned to word size, and no smaller
 * than internal minimum.
*/
static size_t adjust_request_size(size_t size, size_t align)
{
    size_t adjust = 0;
    if (size) {
        const size_t aligned = align_up(size, align);

        /* aligned sized must not exceed block_size_max */
        if (aligned < block_size_max) {
            adjust = tlsf_max(aligned, block_size_min);
        }
    }
    return adjust;
}

/*
 * TLSF utility functions. In most cases, these are direct translations of
 * the documentation found in the white paper.
*/

static void mapping_insert(size_t size, int *fli, int *sli)
{
    int fl, sl;
    if (size < SMALL_BLOCK_SIZE) {
        /* Store small blocks in first list. */
        fl = 0;
        sl = tlsf_cast(int, size) / (SMALL_BLOCK_SIZE / SL_INDEX_COUNT);
    } else {
        fl = tlsf_fls(size);
        sl = tlsf_cast(int, size >> (fl - SL_INDEX_COUNT_LOG2)) ^
             (1 << SL_INDEX_COUNT_LOG2);
        fl -= (FL_INDEX_SHIFT - 1);
    }
    *fli = fl;
    *sli = sl;
}

/* This version rounds up to the next block size (for allocations) */
static void mapping_search(size_t size, int *fli, int *sli)
{
    if (size >= SMALL_BLOCK_SIZE) {
        const size_t round =
            (1 << (tlsf_fls(size) - SL_INDEX_COUNT_LOG2)) - 1;
        size += round;
    }
    mapping_insert(size, fli, sli);
}

static block_header_t *search_suitable_block(control_t *control,
                                             int *fli, int *sli)
{
    int fl = *fli;
    int sl = *sli;

    /*
     * First, search for a block in the list associated with the given
     * fl/sl index.
     */
    unsigned int sl_map = control->sl_bitmap[fl] & (((unsigned int)~0) << sl);
    if (!sl_map) {
        /* No block exists. Search in the next largest first-level list. */
        const unsigned int fl_map = control->fl_bitmap & (((unsigned int)~0) << (fl + 1));
        if (!fl_map) {
            /* No free blocks available, memory has been exhausted. */
            return NULL;
        }

        fl = tlsf_ffs(fl_map);
        *fli = fl;
        sl_map = control->sl_bitmap[fl];
    }
    sl = tlsf_ffs(sl_map);
    *sli = sl;

    /* Return the first block in the free list. */
    return control->blocks[fl][sl];
}

/* Remove a free block from the free list.*/
static void remove_free_block(control_t *control,
                              block_header_t *block,
                              int fl, int sl)
{
    block_header_t *prev = block->prev_free;
    block_header_t *next = block->next_free;
    next->prev_free = prev;
    prev->next_free = next;

    /* If this block is the head of the free list, set new head. */
    if (control->blocks[fl][sl] == block) {
        control->blocks[fl][sl] = next;

        /* If the new head is null, clear the bitmap. */
        if (next == &control->block_null) {
            control->sl_bitmap[fl] &= ~(1 << sl);

            /* If the second bitmap is now empty, clear the fl bitmap. */
            if (!control->sl_bitmap[fl]) {
                control->fl_bitmap &= ~(1 << fl);
            }
        }
    }
}

/* Insert a free block into the free block list. */
static void insert_free_block(control_t *control,
                              block_header_t *block,
                              int fl, int sl)
{
    block_header_t *current = control->blocks[fl][sl];
    block->next_free = current;
    block->prev_free = &control->block_null;
    current->prev_free = block;

    /*
     * Insert the new block at the head of the list, and mark the first-
     * and second-level bitmaps appropriately.
     */
    control->blocks[fl][sl] = block;
    control->fl_bitmap |= (1 << fl);
    control->sl_bitmap[fl] |= (1 << sl);
}

/* Remove a given block from the free list. */
static void block_remove(control_t *control, block_header_t *block)
{
    int fl, sl;
    mapping_insert(block_size(block), &fl, &sl);
    remove_free_block(control, block, fl, sl);
}

/* Insert a given block into the free list. */
static void block_insert(control_t *control, block_header_t *block)
{
    int fl, sl;
    mapping_insert(block_size(block), &fl, &sl);
    insert_free_block(control, block, fl, sl);
}

static int block_can_split(block_header_t *block, size_t size)
{
    return block_size(block) >= sizeof(block_header_t) + size;
}

/* Split a block into two, the second of which is free. */
static block_header_t *block_split(block_header_t *block, size_t size)
{
    /* Calculate the amount of space left in the remaining block. */
    block_header_t *remaining = offset_to_block(block_to_ptr(block),
                                                size - block_header_overhead);

    const size_t remain_size = block_size(block) -
                               (size + block_header_overhead);

    block_set_size(remaining, remain_size);

    block_set_size(block, size);
    block_mark_as_free(remaining);

    return remaining;
}

/* Absorb a free block's storage into an adjacent previous free block. */
static block_header_t *block_absorb(block_header_t *prev, block_header_t *block)
{
    /* Note: Leaves flags untouched. */
    prev->size += block_size(block) + block_header_overhead;
    block_link_next(prev);
    return prev;
}

/* Merge a just-freed block with an adjacent previous free block. */
static block_header_t *block_merge_prev(control_t *control,
                                        block_header_t *block)
{
    if (block_is_prev_free(block)) {
        block_header_t *prev = block_prev(block);
        block_remove(control, prev);
        block = block_absorb(prev, block);
    }

    return block;
}

/* Merge a just-freed block with an adjacent free block. */
static block_header_t *block_merge_next(control_t *control,
                                        block_header_t *block)
{
    block_header_t *next = block_next(block);

    if (block_is_free(next)) {
        block_remove(control, next);
        block = block_absorb(block, next);
    }

    return block;
}

/* Trim any trailing block space off the end of a block, return to pool. */
static void block_trim_free(control_t *control,
                            block_header_t *block,
                            size_t size)
{
    if (block_can_split(block, size)) {
        block_header_t *remaining_block = block_split(block, size);
        block_link_next(block);
        block_set_prev_free(remaining_block);
        block_insert(control, remaining_block);
    }
}




static block_header_t *block_locate_free(control_t *control, size_t size)
{
    int fl = 0, sl = 0;
    block_header_t *block = NULL;

    if (size) {
        mapping_search(size, &fl, &sl);
        block = search_suitable_block(control, &fl, &sl);
    }

    if (block) {
        tlsf_assert(block_size(block) >= size);
        remove_free_block(control, block, fl, sl);
    }

    if (unlikely(block && !block->size))
        block = NULL;

    return block;
}

static void *block_prepare_used(control_t *control,
                                block_header_t *block,
                                size_t size)
{
    void *p = NULL;
    if (block) {
        tlsf_assert(size && "size must be non-zero");
        block_trim_free(control, block, size);
        block_mark_as_used(block);
        p = block_to_ptr(block);
    }
    return p;
}

/* Clear structure and point all empty lists at the null block. */
static void control_construct(control_t *control)
{
    int i, j;

    control->block_null.next_free = &control->block_null;
    control->block_null.prev_free = &control->block_null;

    control->fl_bitmap = 0;
    for (i = 0; i < FL_INDEX_COUNT; ++i) {
        control->sl_bitmap[i] = 0;
        for (j = 0; j < SL_INDEX_COUNT; ++j) {
            control->blocks[i][j] = &control->block_null;
        }
    }
}



/*
 * TLSF main interface.
 */
/*
 * Size of the TLSF structures in a given memory block passed to
 * tlsf_create, equal to the size of a control_t
 */
size_t tlsf_size(void)
{
    return sizeof(control_t);
}

size_t tlsf_align_size(void)
{
    return ALIGN_SIZE;
}

size_t tlsf_block_size_min(void)
{
    return block_size_min;
}

size_t tlsf_block_size_max(void)
{
    return block_size_max;
}

/*
 * Overhead of the TLSF structures in a given memory block passed to
 * tlsf_add_pool, equal to the overhead of a free block and the
 * sentinel block.
 */
size_t tlsf_pool_overhead(void)
{
    return 2 * block_header_overhead;
}

size_t tlsf_alloc_overhead(void)
{
    return block_header_overhead;
}


pool_t tlsf_add_pool(tlsf_t tlsf, void *mem, size_t bytes)
{
    block_header_t *block;
    block_header_t *next;

    const size_t pool_overhead = tlsf_pool_overhead();
    const size_t pool_bytes = align_down(bytes - pool_overhead, ALIGN_SIZE);

    if (((ptrdiff_t)mem % ALIGN_SIZE) != 0) {
        return 0;
    }

    if (pool_bytes < block_size_min || pool_bytes > block_size_max) {
        return 0;
    }

    /*
     * Create the main free block. Offset the start of the block slightly
     * so that the prev_phys_block field falls outside of the pool -
     * it will never be used.
    */
    block = offset_to_block(mem, -(tlsfptr_t)block_header_overhead);
    block_set_size(block, pool_bytes);
    block_set_free(block);
    block_set_prev_used(block);
    block_insert(tlsf_cast(control_t *, tlsf), block);

    /* Split the block to create a zero-size sentinel block. */
    next = block_link_next(block);
    block_set_size(next, 0);
    block_set_used(next);
    block_set_prev_free(next);

    return mem;
}

tlsf_t tlsf_create(void *mem)
{
    if (((tlsfptr_t)mem % ALIGN_SIZE) != 0) {
        return NULL;
    }

    control_construct(tlsf_cast(control_t *, mem));

    return tlsf_cast(tlsf_t, mem);
}

tlsf_t tlsf_create_with_pool(void *mem, size_t bytes)
{
    tlsf_t tlsf = tlsf_create(mem);
    tlsf_add_pool(tlsf, (char *)mem + tlsf_size(), bytes - tlsf_size());
    return tlsf;
}


pool_t tlsf_get_pool(tlsf_t tlsf)
{
    return tlsf_cast(pool_t, (char *)tlsf + tlsf_size());
}

void *tlsf_malloc(tlsf_t tlsf, size_t size)
{
    control_t *control = tlsf_cast(control_t *, tlsf);
    const size_t adjust = adjust_request_size(size, ALIGN_SIZE);
    block_header_t *block = block_locate_free(control, adjust);
    return block_prepare_used(control, block, adjust);
}

void tlsf_free(tlsf_t tlsf, void *ptr)
{
    if (unlikely(!ptr)) return;

    control_t *control = tlsf_cast(control_t *, tlsf);
    block_header_t *block = block_from_ptr(ptr);
    tlsf_assert(!block_is_free(block) && "block already marked as free");
    block_mark_as_free(block);
    block = block_merge_prev(control, block);
    block = block_merge_next(control, block);
    block_insert(control, block);
}



/** Public functions */
void k_heap_init(void)
{
	size_t s = K_HEAP_SIZE + tlsf_size();
	k_mem = tlsf_create_with_pool(k_heap,s);
	ulipe_assert(k_mem != NULL);
}


void *k_malloc(size_t size)
{
    void *ret = NULL;

    archtype_t key = port_irq_lock();

    /* request memory from the allocator block. */
    ret = tlsf_malloc(k_mem, size);

    port_irq_unlock(key);

    return ret;
}

void k_free(void *mem)
{

    archtype_t key = port_irq_lock();

    if (mem != NULL)
    {
    	tlsf_free(k_mem, mem);
    }

    port_irq_unlock(key);
}

#endif
