/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_port_x86_iamcu_defs.h
 *
 *  @brief specific x86 iamcu port file
 *
 *
 */

#ifndef __INTEL_QUARK_SE_C1000_K_PORT_X86_IAMCU_DEFS_H_
#define __INTEL_QUARK_SE_C1000_K_PORT_X86_IAMCU_DEFS_H_

#if (ARCH_TYPE_QUARK_IAMCU > 0)

/* resolve the IDT gate size */
#define PORT_IDT_NUM_GATES (68)
#define PORT_IDT_SIZE (sizeof(intr_gate_desc_t) * PORT_IDT_NUM_GATES)

typedef struct idtr {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) port_idtr_t;



typedef struct intr_gate_desc {
	uint16_t isr_low;
	uint16_t selector; /* Segment selector */
	uint16_t conf;
	uint16_t isr_high;
} __attribute__((packed)) port_intr_gate_desc_t;


/**
 * @name APIC
 * @{
 */

typedef struct {
	volatile uint32_t reg;
	volatile uint32_t pad[3];
} apic_reg_pad_t;

/** APIC register block type. */
typedef struct {
	volatile apic_reg_pad_t reserved0[2];
	volatile apic_reg_pad_t id;      /**< LAPIC ID */
	volatile apic_reg_pad_t version; /**< LAPIC version*/
	volatile apic_reg_pad_t reserved1[4];
	volatile apic_reg_pad_t tpr;    /**< Task priority*/
	volatile apic_reg_pad_t apr;    /**< Arbitration priority */
	volatile apic_reg_pad_t ppr;    /**< Processor priority */
	volatile apic_reg_pad_t eoi;    /**< End of interrupt */
	volatile apic_reg_pad_t rrd;    /**< Remote read */
	volatile apic_reg_pad_t ldr;    /**< Logical destination */
	volatile apic_reg_pad_t dfr;    /**< Destination format */
	volatile apic_reg_pad_t svr;    /**< Spurious vector */
	volatile apic_reg_pad_t isr[8]; /**< In-service */
	volatile apic_reg_pad_t tmr[8]; /**< Trigger mode */
	volatile apic_reg_pad_t irr[8]; /**< Interrupt request */
	volatile apic_reg_pad_t esr;    /**< Error status */
	volatile apic_reg_pad_t reserved2[6];
	volatile apic_reg_pad_t lvtcmci;   /**< Corrected Machine Check vector */
	volatile apic_reg_pad_t icr[2];    /**< Interrupt command */
	volatile apic_reg_pad_t lvttimer;  /**< Timer vector */
	volatile apic_reg_pad_t lvtts;     /**< Thermal sensor vector */
	volatile apic_reg_pad_t lvtpmcr;   /**< Perfmon counter vector */
	volatile apic_reg_pad_t lvtlint0;  /**< Local interrupt 0 vector */
	volatile apic_reg_pad_t lvtlint1;  /**< Local interrupt 1 vector */
	volatile apic_reg_pad_t lvterr;    /**< Error vector */
	volatile apic_reg_pad_t timer_icr; /**< Timer initial count */
	volatile apic_reg_pad_t timer_ccr; /**< Timer current count */
	volatile apic_reg_pad_t reserved3[4];
	volatile apic_reg_pad_t timer_dcr; /**< Timer divide configuration */
} qm_lapic_reg_t;


/* Local APIC. */
#define QM_LAPIC_BASE (0xFEE00000)
#define QM_LAPIC ((qm_lapic_reg_t *)QM_LAPIC_BASE)
#define QM_INT_CONTROLLER QM_LAPIC

#define QM_ISR_EOI(vector)                             \
do {                                                  \
		QM_INT_CONTROLLER->eoi.reg = 0;                                \
		QM_IOAPIC->eoi.reg = vector;                                   \
	} while (0)

typedef struct {
	volatile apic_reg_pad_t ioregsel; /**< Register selector. */
	volatile apic_reg_pad_t iowin;    /**< Register window. */
	volatile apic_reg_pad_t reserved[2];
	volatile apic_reg_pad_t eoi; /**< EOI register. */
} qm_ioapic_reg_t;


#define QM_IOAPIC_REG_VER (0x01)    /* IOAPIC version. */
#define QM_IOAPIC_REG_REDTBL (0x10) /* Redirection table base. */


/* IO / APIC base address. */
#define QM_IOAPIC_BASE (0xFEC00000)
#define QM_IOAPIC ((qm_ioapic_reg_t *)QM_IOAPIC_BASE)


#define PORT_SWAP_VECTOR	0x20
#define PORT_INITIAL_EFLAGS 0x200

/*
 * Stack frame data structure:
 */
typedef struct
{
	uint32_t eflags;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint32_t pc;
}x86_iamcu_xcpt_contents_t;


#endif
#endif
