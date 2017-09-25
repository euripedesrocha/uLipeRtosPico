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


/* options for queue usage */
typedef enum {
	k_msg_block = 0,
	k_msg_accept,
}msg_opt_t;


/**
 *  @fn MESSAGE_BLOCK_DECLARE()
 *  @brief allocates memory and a fully initialized messaging block
 *
 *  @param name - name of initialized message structure this is the parameter used on message API
 *  @param noof_slots - number of elements of this message
 *  @param slot_size_val - size in bytes of this message slot
 *
 *  @return a fully initialized kmsg_t block ready to use
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


/**
 *  @fn message_insert()
 *  @brief insert data on message queue and optionally block if there is no space free
 *
 *  @param m - message control block will hold the data inserted
 *  @param data - data to be inserted on message queue
 *  @param size - size of data amount to be inserted, it cannot be more than slot_size_val of control block
 *  @param opt - hints to kernel to block current task if no space left if k_msg_block is passed
 *
 *  @return k_status_ok or error for invalid values/queue full
 */
k_status_t message_insert(kmsg_t *m, void *data, uint32_t size, msg_opt_t opt);



/**
 *  @fn message_remove()
 *  @brief remove or peek a message from head of queue and optionally blocks if no space left
 *
 *  @param msg - message control block which holds the messages
 *  @param data - pointer to store data extracted from queue
 *  @param size - pointer to store the size of data extracted from queue
 *  @param peek - if true the data is copied from queue but is not removed from it
 *  @param opt  - hints to kernel to block current task if queue is empty  if k_msg_block is passed
 *
 *  @return k_status_ok or error for invalid values/queue empty
 */
k_status_t message_remove(kmsg_t *msg, void *data, uint32_t *size,bool peek, msg_opt_t opt);




#endif


#endif


