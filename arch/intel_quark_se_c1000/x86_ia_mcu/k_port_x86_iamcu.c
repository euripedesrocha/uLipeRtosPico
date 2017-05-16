/**
 * 							ULIPE RTOS PICO
 *
 *  @file k_port_x86_iamcu.c
 *
 *  @brief specific x86 iamcu port file
 *
 *
 */
#include "ulipe_rtos_pico.h"
#include "include/arch/intel_quark_se_c1000/k_port_x86_iamcu_defs.h"

#if(ARCH_TYPE_QUARK_IAMCU > 0)

/* provide idt descriptor symbol */
static port_intr_gate_desc_t port_idt_table[IDTPORT_IDT_SIZE] = {0};
extern void port_first_swap(void);
extern void port_swap(void);


/** privanta functions */



/*
 * Setup IDT gate as an interrupt descriptor and assing the ISR entry point
 */
static inline void idt_set_intr_gate_desc(uint32_t vector, uint32_t isr)
{
	port_intr_gate_desc_t *desc;

	desc = __idt_start + vector;

	desc->isr_low = isr & 0xFFFF;
	desc->selector = 0x08; /* Code segment offset in GDT */

	desc->conf = 0x8E00; /* type: 0b11 (Interrupt)
				d: 1 (32 bits)
				ss: 0
				dpl: 0
				p: 1
			     */
	desc->isr_high = (isr >> 16) & 0xFFFF;
}

/*
 * Initialize Interrupt Descriptor Table.
 * The IDT is initialized with null descriptors: any interrupt at this stage
 * will cause a triple fault.
 */

static inline void idt_init(void)
{
	port_idtr_t idtr;

	/* Initialize idtr structure */
	idtr.limit = IDT_SIZE - 1;
	idtr.base = (uint32_t)port_idt_table;

	/* Load IDTR register */
	__asm__ __volatile__("lidt %0\n\t" ::"m"(idtr));
}

#define LAPIC_VECTOR_MASK (0xFF)

static void _ioapic_set_redtbl_entry(unsigned int irq, uint64_t value)
{
	unsigned int offset = QM_IOAPIC_REG_REDTBL + (irq * 2);

	QM_IOAPIC->ioregsel.reg = offset;
	QM_IOAPIC->iowin.reg = value & 0x00000000FFFFFFFF;
	QM_IOAPIC->ioregsel.reg = offset + 1;
	QM_IOAPIC->iowin.reg = (value & 0xFFFFFFFF00000000) >> 32;
}

/* Get redirection table size */
static __inline__ int _ioapic_get_redtbl_size(void)
{
	int max_entry_number;

	QM_IOAPIC->ioregsel.reg = QM_IOAPIC_REG_VER;
	max_entry_number = (QM_IOAPIC->iowin.reg & 0x00FF0000) >> 16;

	return max_entry_number + 1;
}

static uint32_t _ioapic_get_redtbl_entry_lo(unsigned int irq)
{
	QM_IOAPIC->ioregsel.reg = QM_IOAPIC_REG_REDTBL + (irq * 2);
	return QM_IOAPIC->iowin.reg;
}

static void _ioapic_set_redtbl_entry_lo(unsigned int irq, uint32_t value)
{
	QM_IOAPIC->ioregsel.reg = QM_IOAPIC_REG_REDTBL + (irq * 2);
	QM_IOAPIC->iowin.reg = value;
}

/*
 * Initialize Local and IOAPIC
 */
static inline void apic_init(void)
{
	int i;
	int size;

	/* Enable LAPIC */
	QM_LAPIC->svr.reg |= BIT(8);

	/* Set up LVT LINT0 to ExtINT and unmask it */
	QM_LAPIC->lvtlint0.reg |= (1 << 8 | 1 << 9 | 1 << 10);
	QM_LAPIC->lvtlint0.reg &= ~(1 << 16);

	/* Clear up any spurious LAPIC interrupts */
	QM_LAPIC->eoi.reg = 0;

	/* Setup IOAPIC Redirection Table */
	size = _ioapic_get_redtbl_size();
	for (i = 0; i < size; i++) {
		_ioapic_set_redtbl_entry(i, (1 << 16));
	}
}


static inline void ioapic_register_irq(unsigned int irq, unsigned int vector)
{
	uint32_t value;

	value = _ioapic_get_redtbl_entry_lo(irq);

	/* Assign vector and set polarity (positive). */
	value &= ~LAPIC_VECTOR_MASK;
	value |= (vector & LAPIC_VECTOR_MASK);
	value &= ~(1 << 13);

	/* Set trigger mode. */
	switch (irq) {
	case QM_IRQ_RTC_0:
	case QM_IRQ_AONPT_0:
	case QM_IRQ_WDT_0:
		/* Edge sensitive. */
		value &= ~(1 << 15);
		break;
	default:
		/* Level sensitive. */
		value |= (1 << 15);
		break;
	}

	_ioapic_set_redtbl_entry_lo(irq, value);
}

static inline void ioapic_mask_irq(unsigned int irq)
{
	uint32_t value = _ioapic_get_redtbl_entry_lo(irq);

	value |= (1 << 16);

	_ioapic_set_redtbl_entry_lo(irq, value);
}

static inline void ioapic_unmask_irq(unsigned int irq)
{
	uint32_t value = _ioapic_get_redtbl_entry_lo(irq);

	value &= ~(1 << 16);

	_ioapic_set_redtbl_entry_lo(irq, value);
}


/** public functions */

archtype_t *port_create_stack_frame(archtype_t *stack, thread_t thr_func, void *cookie)
{
	ULIPE_ASSERT(stack != NULL);
	ULIPE_ASSERT(thr_func != NULL);

	x86_iamcu_xcpt_contents_t *ptr = (x86_iamcu_xcpt_contents_t *)stack;
	--ptr;


	/* populate initial stack frame */
	ptr->pc  = (archtype_t)thr_func;
	ptr->eax = (archtype_t)cookie;
	ptr->ecx = 0xec;
	ptr->ebx = 0xeb;
	ptr->edx = 0xed;
	ptr->esp = 0;
	ptr->ebp = 0;
	ptr->esi = 0xe51;
	ptr->edi = 0xed1;
	ptr->eflags = PORT_INITIAL_EFLAGS;

	return((archtype_t *)ptr);
}




extern void port_init_machine(void)
{

	/* setup vector table */
	idt_init();

	/* setup x86 int controller (APIC) */
	apic_init();


	/* installs exceptions handlers */
	idt_set_intr_gate_desc(PORT_START_VECTOR, (uint32_t)port_first_swap);
	idt_set_intr_gate_desc(PORT_SWAP_VECTOR, (uint32_t)port_swap);

}

#if(K_ENABLE_TIMERS > 0)
extern void port_start_timer(archtype_t reload_val)
{

}


extern void port_timer_load_append(archtype_t append_val)
{

}


extern uint32_t port_timer_halt(void)
{

}

extern void port_timer_resume(void)
{

}

static void port_timer_handler (void)
{

}
#endif


#endif