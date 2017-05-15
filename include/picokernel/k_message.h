/**
 * 							ULIPE RTOS PICO
 *  @file k_message.h
 *
 *  @brief basic messaging usage header file
 *
 */
#ifndef __K_MESSAGE_H
#define __K_MESSAGE_H

#if(K_ENABLE_MESSAGING > 0)

/* message control block structure */
typedef struct kmsg{
	uint8_t *data;
	archtype_t items;
	archtype_t slots_number;
	archtype_t wr_ptr;
	archtype_t rd_ptr;
	archtype_t slot_size;
	bool created;
	k_work_list_t rd_threads_pending;
	k_work_list_t wr_threads_pending;
}kmsg_t;


/**
 *  @fn MESSAGE_BLOCK_DECLARE()
 *  @brief allocates memory and a fully initialized messaging block
 *  @param
 *  @return
 */
#define MESSAGE_BLOCK_DECLARE(name, noof_slots, slot_size_val)							\
	static uint8_t data_##name[noof_slots * (slot_size_val + sizeof(archtype_t))] = {0};\
	static kmsg_t name = {																\
	  .data = &data_##name[0],  							                    		\
	  .items = 0,                                         								\
	  .slots_number = noof_slots,                         								\
	  .wr_ptr = 0,                                        								\
	  .rd_ptr = 0,                                        								\
	  .slot_size = slot_size_val,					                 					\
	  .wr_threads_pending.bitmap=0,														\
	  .rd_threads_pending.bitmap=0,														\
	  .created=false,																	\
	}



/* options for queue usage */
typedef enum {
	k_msg_block = 0,
	k_msg_accept,
}msg_opt_t;


/**
 *  @fn message_insert()
 *  @brief insert data on message queue, if no space enough blocks until a slot become free
 *  @param
 *  @return
 */
k_status_t message_insert(kmsg_t *m, void *data, uint32_t size, msg_opt_t opt);

/**
 *  @fn message_remove()
 *  @brief remove a message from head of queue, if no message, blocks until a slot fills
 *  @param
 *  @return
 */
k_status_t message_remove(kmsg_t *msg, void *data, uint32_t *size,bool peek, msg_opt_t opt);




#endif
#endif
