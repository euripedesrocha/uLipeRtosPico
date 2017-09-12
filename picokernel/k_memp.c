/**
 * @brief simple memory based on small block approach
 */


#include "ulipe_rtos_pico.h"


#if(K_ENABLE_MEMORY_POOLS > 0)
#define SMALL_BLOCK_BMP_LIMIT	31


void *k_block_alloc(pool_info_t *mem){
	void *ret = NULL;


	archtype_t key = port_irq_lock();

	/* check memory pool pointer */
	if(mem != NULL) {
		uint32_t x;
		uint32_t y;

		/* maps the block using bitmap current state */
		if(mem->bitmap_h) {
			x = SMALL_BLOCK_BMP_LIMIT - port_bit_fs_scan(mem->bitmap_h);
			y = SMALL_BLOCK_BMP_LIMIT - port_bit_fs_scan(mem->bitmap_l[x]);
		}

		uint32_t block_pos = (x << 5) | y;

		/* at least 1 block is free */
		if(mem->numblocks){

			/* obtains its address */
			int block_address = mem->block_size * block_pos;

			/* mark as used block */
			mem->bitmap_l[x] &= ~(1 << y);
			if(!mem->bitmap_l[x])
				mem->bitmap_h &= ~(1 << x);

			mem->numblocks--;
			ret = &mem->mem_pool[block_address];

		}
	}

	port_irq_unlock(key);

cleanup:
	/* return the block address */
	return(ret);
}



void k_block_free(pool_info_t *mem, void *p){

	/* check pointer */
	if((p != NULL) && (mem != NULL)) {
		uint8_t x = 0;
		uint8_t y = 0;

		uint32_t pbase = (uint32_t)mem->mem_pool;
		uint32_t palloc = (uint32_t)p;
		uint32_t block_position = ((palloc - pbase)/mem->block_size);


		/* bit position out of range, does not accept the block */
		if(block_position < 1024) {
			x = (uint8_t)((block_position >> 5 )& 31);
			y = (uint8_t)(block_position & 31);

			mem->bitmap_h |= (1 << x);
			mem->bitmap_l[x] |= 1 << y;
			mem->numblocks++;
		}
	}

	p = NULL;
}

#endif
