/**
 *  @file hal/micro/flash.h
 * @brief Header for flash manipulating functions.
 *
 * Copyright 2013 Silicon Laboratories, Inc.                                *80*
 */

#ifndef FLASH_H_
#define FLASH_H_

/** Macro definitions       */

#if defined(EMBER_TEST)
  //#include ""
#else
  // platform that doesn't have flash support
#endif

/**
 * Function to write a byte to the given flash address.
 *
 * @param uint16_t  Flash address.
 * @param int8_t   Byte value to write.
 */
void halFlashWrite(uint16_t address, int8_t value);

/**
 * Function to erase a flash page.
 *
 * @param uint16_t  Flash page address.
 */
void halFlashErase(uint16_t address);

/**
 * Check if the given flash page is empty or not.
 *
 * @param uint16_t  Address to check.
 * @return        true - Flash page empty / false - Flash page not empty.
 */
uint8_t halFlashBlankCheck(uint16_t address);

/**
 * Read a value from the given flash location.
 *
 * @param uint16_t  Flash address to read.
 *
 * @return        Byte value at the given location.
 */
uint8_t halFlashRead(uint16_t address);


#endif /* FLASH_H_ */
