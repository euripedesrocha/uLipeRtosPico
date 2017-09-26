/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2017
 * Generated linker script file for LPC54102J256
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.23
 * MCUXpresso IDE Debug Build on Sep 26, 2017 3:12:19 PM
 */

MEMORY
{
  /* Define each memory region */
  MFlash256 (rx) : ORIGIN = 0x0, LENGTH = 0x40000 /* 256K bytes (alias Flash) */  
  Ram0_64 (rwx) : ORIGIN = 0x2000000, LENGTH = 0x10000 /* 64K bytes (alias RAM) */  
  Ram1_32 (rwx) : ORIGIN = 0x2010000, LENGTH = 0x8000 /* 32K bytes (alias RAM2) */  
  Ram2_8 (rwx) : ORIGIN = 0x3400000, LENGTH = 0x2000 /* 8K bytes (alias RAM3) */  
}

  /* Define a symbol for the top of each memory region */
  __base_MFlash256 = 0x0  ; /* MFlash256 */  
  __base_Flash = 0x0 ; /* Flash */  
  __top_MFlash256 = 0x0 + 0x40000 ; /* 256K bytes */  
  __top_Flash = 0x0 + 0x40000 ; /* 256K bytes */  
  __base_Ram0_64 = 0x2000000  ; /* Ram0_64 */  
  __base_RAM = 0x2000000 ; /* RAM */  
  __top_Ram0_64 = 0x2000000 + 0x10000 ; /* 64K bytes */  
  __top_RAM = 0x2000000 + 0x10000 ; /* 64K bytes */  
  __base_Ram1_32 = 0x2010000  ; /* Ram1_32 */  
  __base_RAM2 = 0x2010000 ; /* RAM2 */  
  __top_Ram1_32 = 0x2010000 + 0x8000 ; /* 32K bytes */  
  __top_RAM2 = 0x2010000 + 0x8000 ; /* 32K bytes */  
  __base_Ram2_8 = 0x3400000  ; /* Ram2_8 */  
  __base_RAM3 = 0x3400000 ; /* RAM3 */  
  __top_Ram2_8 = 0x3400000 + 0x2000 ; /* 8K bytes */  
  __top_RAM3 = 0x3400000 + 0x2000 ; /* 8K bytes */  
