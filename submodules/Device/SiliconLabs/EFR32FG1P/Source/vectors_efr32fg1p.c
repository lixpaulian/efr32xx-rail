//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu
//

// ----------------------------------------------------------------------------

#include "cmsis-plus/cortexm/exception-handlers.h"

// ----------------------------------------------------------------------------

void __attribute__((weak))
Default_Handler(void);

// Forward declaration of the specific IRQ handlers. These are aliased
// to the Default_Handler, which is a 'forever' loop. When the application
// defines a handler (with the same name), this will automatically take
// precedence over these weak definitions
//

void __attribute__ ((weak, alias("Default_Handler")))
EMU_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
FRC_PRI_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
WDOG0_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
FRC_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
MODEM_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
RAC_SEQ_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
RAC_RSM_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
BUFC_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
LDMA_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
GPIO_EVEN_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
TIMER0_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
USART0_RX_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
USART0_TX_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
ACMP0_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
ADC0_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
IDAC0_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
I2C0_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
GPIO_ODD_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
TIMER1_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
USART1_RX_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
USART1_TX_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
LEUART0_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
PCNT0_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
CMU_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
MSC_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
CRYPTO_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
LETIMER0_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
AGC_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
PROTIMER_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
RTCC_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
SYNTH_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
CRYOTIMER_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
RFSENSE_IRQHandler(void);
void __attribute__ ((weak, alias("Default_Handler")))
FPUEH_IRQHandler(void);


// ----------------------------------------------------------------------------

extern unsigned int _estack;

typedef void
(* const pHandler)(void);

// ----------------------------------------------------------------------------

// The vector table.
// This relies on the linker script to place at correct location in memory.

__attribute__ ((section(".isr_vector"),used))
pHandler __isr_vectors[] =
{ //
		(pHandler) &_estack,                        // The initial stack pointer
				Reset_Handler,                            // The reset handler

				NMI_Handler,                              // The NMI handler
				HardFault_Handler,                     // The hard fault handler

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
				MemManage_Handler,                      // The MPU fault handler
				BusFault_Handler,                       // The bus fault handler
				UsageFault_Handler,                   // The usage fault handler
#else
				0, 0, 0,				  // Reserved
#endif
				0,                                        // Reserved
				0,                                        // Reserved
				0,                                        // Reserved
				0,                                        // Reserved
				SVC_Handler,                              // SVCall handler
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
				DebugMon_Handler,                       // Debug monitor handler
#else
				0,					  // Reserved
#endif
				0,                                        // Reserved
				PendSV_Handler,                           // The PendSV handler
				SysTick_Handler,                          // The SysTick handler

				// ----------------------------------------------------------------------
				// efr32fg1p vectors
				EMU_IRQHandler, /*  0 - EMU       */
				FRC_PRI_IRQHandler, /*  1 - FRC_PRI       */
				WDOG0_IRQHandler, /*  2 - WDOG0       */
				FRC_IRQHandler, /*  3 - FRC       */
				MODEM_IRQHandler, /*  4 - MODEM       */
				RAC_SEQ_IRQHandler, /*  5 - RAC_SEQ       */
				RAC_RSM_IRQHandler, /*  6 - RAC_RSM       */
				BUFC_IRQHandler, /*  7 - BUFC       */
				LDMA_IRQHandler, /*  8 - LDMA       */
				GPIO_EVEN_IRQHandler, /*  9 - GPIO_EVEN       */
				TIMER0_IRQHandler, /*  10 - TIMER0       */
				USART0_RX_IRQHandler, /*  11 - USART0_RX       */
				USART0_TX_IRQHandler, /*  12 - USART0_TX       */
				ACMP0_IRQHandler, /*  13 - ACMP0       */
				ADC0_IRQHandler, /*  14 - ADC0       */
				IDAC0_IRQHandler, /*  15 - IDAC0       */
				I2C0_IRQHandler, /*  16 - I2C0       */
				GPIO_ODD_IRQHandler, /*  17 - GPIO_ODD       */
				TIMER1_IRQHandler, /*  18 - TIMER1       */
				USART1_RX_IRQHandler, /*  19 - USART1_RX       */
				USART1_TX_IRQHandler, /*  20 - USART1_TX       */
				LEUART0_IRQHandler, /*  21 - LEUART0       */
				PCNT0_IRQHandler, /*  22 - PCNT0       */
				CMU_IRQHandler, /*  23 - CMU       */
				MSC_IRQHandler, /*  24 - MSC       */
				CRYPTO_IRQHandler, /*  25 - CRYPTO       */
				LETIMER0_IRQHandler, /*  26 - LETIMER0       */
				AGC_IRQHandler, /*  27 - AGC       */
				PROTIMER_IRQHandler, /*  28 - PROTIMER       */
				RTCC_IRQHandler, /*  29 - RTCC       */
				SYNTH_IRQHandler, /*  30 - SYNTH       */
				CRYOTIMER_IRQHandler, /*  31 - CRYOTIMER       */
				RFSENSE_IRQHandler, /*  32 - RFSENSE       */
				FPUEH_IRQHandler, /*  33 - FPUEH       */
		};

// ----------------------------------------------------------------------------

// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.

void __attribute__ ((section(".after_vectors")))
Default_Handler(void)
{
	while (1)
	{
	}
}

// ----------------------------------------------------------------------------
