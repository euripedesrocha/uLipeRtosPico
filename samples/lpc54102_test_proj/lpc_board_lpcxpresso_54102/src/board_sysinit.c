/*
 * @brief NXP LPCXpresso 54102 Sysinit file
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2014
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

 #include "board.h"

/* The System initialization code is called prior to the application and
   initializes the board for run-time operation. Board initialization
   for the NXP LPC54102 board includes default pin muxing and clock setup
   configuration. */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Pin muxing table, only items that need changing from their default pin
   state are in this table. Not every pin is mapped. */

STATIC const PINMUX_GRP_T pinmuxing[] = {
    /* UART0 */
    {0, 0,  (IOCON_FUNC1 | IOCON_MODE_PULLUP | IOCON_DIGITAL_EN)},         /* UART0 RX */
    {0, 1,  (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGITAL_EN)},             /* UART0 TX */
    {0, 2,      (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */
    {0, 3,      (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */
    {0, 4,      (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */


    /* UART1 */
    {0, 5,  (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGITAL_EN)},             /* BLE RX */
    {0, 6,      (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN)},         /* BLE TX */

    {0, 7,      (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN )}, /* BLE STATE 1 */
    {0, 8,      (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN )}, /* BLE IRQ */
    {0, 9,      (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */
    {0, 10, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN )}, /* BLE Wake up, Output */
    {0, 11, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */
    {0, 12, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */
    {0, 13, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */
    {0, 14, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */

#if 1
    /* 15.16.17 FOR SWD */
	 {0, 15, (IOCON_FUNC2 | IOCON_MODE_INACT | IOCON_ANALOG_EN)}, /* SWO */
	 //{0, 16, (IOCON_FUNC5 | IOCON_MODE_INACT | IOCON_DIGITAL_EN)}, /* SWCLK_TCK */
	 //{0, 17, (IOCON_FUNC5 | IOCON_MODE_INACT | IOCON_DIGITAL_EN)}, /* SWDIO */
#else
	 {0, 15, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN)}, /* SWO */
	 {0, 16, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN)}, /* SWCLK_TCK */
	 {0, 17, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN)}, /* SWDIO */

#endif
    {0, 18, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN)},                /* NC */
    {0, 19, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN | IOCON_DIGITAL_EN)},           /* NFC IRQ */
    {0, 20, (IOCON_FUNC0 | IOCON_MODE_PULLDOWN | IOCON_DIGITAL_EN)},           /* NFC VEN */
    {0, 21, (IOCON_FUNC0 | IOCON_MODE_INACT)},          /* P0_21 */
    {0, 22, (IOCON_FUNC0 | IOCON_MODE_INACT)},          /* P0_22 */

	
    /* PN66T I2C */
    {0, 23, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_STDI2C_EN)}, /* I2C0_SCL (SCL) */
    {0, 24, (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_STDI2C_EN)}, /* I2C0_SDA-WAKEUP (SDA) */
    {0, 25, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN)},           /* NFC Download REQ */
    {0, 26, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN)},           /* QN9022 Reset */


    /* GPIOs */
    {0, 27, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN)},           /* P0_27 */
    {0, 28, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN)},           /* P0_28 */

    {1, 0,      (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */
    {1, 1,      (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */
    {1, 2,      (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */
    {1, 3,      (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */
    {1, 4,      (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */
    {1, 5,      (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */
    {1, 6,      (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */
    {1, 7,      (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */
    {1, 8,      (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_ANALOG_EN )}, /* NC */

};

#ifndef BOARD_USECLKINSRC
#define BOARD_USECLKINSRC   (0)
#endif

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/
#define BRIDGE_SSEL_PORT 0
#define BRIDGE_SSEL_PIN 14
static void ConfigureBridgeSSEL(void)
{
	PINMUX_GRP_T pinMuxBridgeSSEL[] = {
		{BRIDGE_SSEL_PORT, BRIDGE_SSEL_PIN, (IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN)}
	};
	/* Default bits to Link processor powered down. */
	uint32_t functionBits = (IOCON_FUNC1 | IOCON_MODE_PULLDOWN | IOCON_DIGITAL_EN);

	/* Set the bridge SSEL pin to GPIO pull down so we can read the state */
	Chip_IOCON_SetPinMuxing(LPC_IOCON, pinMuxBridgeSSEL, sizeof(pinMuxBridgeSSEL) / sizeof(PINMUX_GRP_T));

	/* Drive the bridge SSEL pin low and then read it back */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, BRIDGE_SSEL_PORT, BRIDGE_SSEL_PIN);
	Chip_GPIO_SetPinState(LPC_GPIO, BRIDGE_SSEL_PORT, BRIDGE_SSEL_PIN, false);

	/* Set direction back to input and if the pin reads high, we know the link processor is powered */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, BRIDGE_SSEL_PORT, BRIDGE_SSEL_PIN);
	if (Chip_GPIO_GetPinState(LPC_GPIO, BRIDGE_SSEL_PORT, BRIDGE_SSEL_PIN)) {

		/* Set function bits when Link processor present */
		functionBits = (IOCON_FUNC1 | IOCON_MODE_INACT | IOCON_DIGITAL_EN);
	}

#if 0
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 15, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 16, false);
	Chip_GPIO_SetPinState(LPC_GPIO, 0, 17, false);

	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 15);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 16);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO, 0, 17);
#endif

	pinMuxBridgeSSEL[0].modefunc = functionBits;
	Chip_IOCON_SetPinMuxing(LPC_IOCON, pinMuxBridgeSSEL, sizeof(pinMuxBridgeSSEL) / sizeof(PINMUX_GRP_T));
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Sets up system pin muxing */
void Board_SetupMuxing(void)
{
	/* Enable IOCON clock */
	Chip_Clock_EnablePeriphClock(SYSCON_CLOCK_IOCON);

	Chip_IOCON_SetPinMuxing(LPC_IOCON, pinmuxing, sizeof(pinmuxing) / sizeof(PINMUX_GRP_T));

	/* Bridge SSEL requires detection to set state correctly */
	ConfigureBridgeSSEL();

	/* IOCON clock left on, this is needed if CLKIN is used. */
}

/* Set up and initialize clocking prior to call to main */
void Board_SetupClocking(void)
{
	uint32_t freq = BOARD_MAINCLOCKRATE;

	/* The IRC is always the first clock source even if CLK_IN is used later.
	   Once CLK_IN is selected as the clock source. We can turned off the IRC later.
	   Turn on the IRC by clearing the power down bit */
	Chip_SYSCON_PowerUp(SYSCON_PDRUNCFG_PD_IRC_OSC | SYSCON_PDRUNCFG_PD_IRC);

#if BOARD_USECLKINSRC == (0)

	/* Check if the Chip can support higher frequency */
#if (BOARD_MAINCLOCKRATE > 96000000)
	if (LPC_SYSCON->DEVICE_ID1 < V4_UID) {
		/* Older version of chips does not support > 96MHz (see errata) */
		freq = 96000000;
	}
#endif
	/* Setup PLL based on (internal) IRC clocking */
	Chip_SetupIrcClocking(freq);

#else
	/* Setup PLL based on (external) CLKIN clocking */
	Chip_SetupExtInClocking(freq);
#endif

	/* Select the CLKOUT clocking source */
	Chip_Clock_SetCLKOUTSource(SYSCON_CLKOUTSRC_MAINCLK, 1);
}

/* Set up and initialize hardware prior to call to main */
void Board_SystemInit(void)
{
	/* Setup system clocking and muxing */
	Board_SetupMuxing();
	Board_SetupClocking();
}
