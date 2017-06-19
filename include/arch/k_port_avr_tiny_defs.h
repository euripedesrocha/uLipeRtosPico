/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_port_avr_tiny_defs.h
 *
 *  @brief specific avr tiny port file
 *
 *
 */



#ifndef __K_PORT_AVR_TINY_DEFS_H
#define __K_PORT_AVR_TINY_DEFS_H

#if (ARCH_TYPE_AVR_TINY > 0)


/* general machine specific registers */
#define MCUCR *((volatile uint8_t *)0x35)
#define GIMSK *((volatile uint8_t *)0x3B)
#define GIFR  *((volatile uint8_t *)0x3A)
#define SREG  *((volatile uint8_t *)0x3F)

/* timer specific register for use with timers */
#define TCCR0A  *((volatile uint8_t *)0x2A)
#define TCCR0B  *((volatile uint8_t *)0x33)
#define TCNT0	*((volatile uint8_t *)0x32)
#define OCR0A	*((volatile uint8_t *)0x29)
#define TIMSK 	*((volatile uint8_t *)0x39) 



/*
 * Stack frame data structure:
 */
typedef struct
{
	uint8_t r0;
	uint8_t r1;
	uint8_t r2;
	uint8_t r3;
	uint8_t r4;
	uint8_t r5;
	uint8_t r6;
	uint8_t r7;
	uint8_t r8;
	uint8_t r9;
	uint8_t r10;
	uint8_t r11;
	uint8_t r12;
	uint8_t r13;
	uint8_t r14;
	uint8_t r15;
	uint8_t r16;
	uint8_t r17;
	uint8_t r18;
	uint8_t r19;
	uint8_t r20;
	uint8_t r21;
	uint8_t r22;
	uint8_t r23;
	uint8_t r24;
	uint8_t r25;
	uint8_t r26;
	uint8_t r27;
	uint8_t r28;
	uint8_t r29;
	uint8_t r30;
	uint8_t sreg;
	uint8_t r31;
    uint16_t pc;
}avr_tiny_xcpt_contents_t;


#endif
#endif
