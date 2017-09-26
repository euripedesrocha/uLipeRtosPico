/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_mem_dyn.h
 *
 *  @brief dynamic memory block allocator based on TLSF strategy
 *
 *
 */
#ifndef __K_MEM_DYN_H
#define __K_MEM_DYN_H

#if (K_ENABLE_DYNAMIC_ALLOCATOR > 0)

/**
 *  @fn k_malloc()
 *  @brief allocates a block of specified size
 *
 *  @param size - size in bytes of memory request
 *
 *  @return a valid pointer to memory block with specified size or NULL in case of no memory left
 */
void *k_malloc(size_t size);


/**
 *  @fn k_free
 *  @brief after use, the block can be released using this function
 *
 *  @param pointer to block who wants to release
 *
 *  @return none
 */
void k_free(void *p);


#endif
#endif /* OSMEM_H_ */
