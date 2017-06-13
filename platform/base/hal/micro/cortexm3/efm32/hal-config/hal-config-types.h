// -----------------------------------------------------------------------------
// @file hal-config-types.h
// @brief HAL configuration types
//
// @section License
// <b>(C) Copyright 2016 Silicon Laboratories, http://www.silabs.com</b>
//
// This file is licensed under the Silabs License Agreement. See the file
// "Silabs_License_Agreement.txt" for details. Before using this software for
// any purpose, you must agree to the terms of that agreement.
//
// -----------------------------------------------------------------------------
#ifndef HAL_CONFIG_TYPES_H
#define HAL_CONFIG_TYPES_H
#include "em_gpio.h"

#define HAL_GPIO_MODE_INPUT               gpioModeInput
#define HAL_GPIO_MODE_INPUT_PULL          gpioModeInputPull
#define HAL_GPIO_MODE_INPUT_PULL_FILTER   gpioModeInputPullFilter

#define HAL_GPIO_DOUT_LOW  0
#define HAL_GPIO_DOUT_HIGH 1

#define HAL_CLK_HFCLK_SOURCE_HFRCO 1
#define HAL_CLK_HFCLK_SOURCE_HFXO  2

#define HAL_CLK_LFCLK_SOURCE_LFRCO 1
#define HAL_CLK_LFCLK_SOURCE_LFXO  2

#define HAL_PTI_MODE_SPI          1
#define HAL_PTI_MODE_UART         2
#define HAL_PTI_MODE_UART_ONEWIRE 3

#define HAL_SERIAL_PORT_VCP     0x10
#define HAL_SERIAL_PORT_USART0  0x20
#define HAL_SERIAL_PORT_USART1  0x21
#define HAL_SERIAL_PORT_USART2  0x22
#define HAL_SERIAL_PORT_USART3  0x23
#define HAL_SERIAL_PORT_LEUART0 0x40
#define HAL_SERIAL_PORT_LEUART1 0x41
#define HAL_SERIAL_PORT_USB     0x50

#define HAL_USART_FLOW_CONTROL_NONE    0
#define HAL_USART_FLOW_CONTROL_SW      1
#define HAL_USART_FLOW_CONTROL_HW      2
#define HAL_USART_FLOW_CONTROL_HWUART  3

typedef struct {
  GPIO_Port_TypeDef   port;
  uint8_t             pin;
  GPIO_Mode_TypeDef   pUpMode;
  uint8_t             pUpOut;
  GPIO_Mode_TypeDef   pDownMode;
  uint8_t             pDownOut;
} tGpioArray;

#endif // HAL_CONFIG_TYPES_H
