// -----------------------------------------------------------------------------
// @file nodetest.h
// @brief Nodetest HAL configuration
//
// @section License
// <b>(C) Copyright 2016 Silicon Laboratories, http://www.silabs.com</b>
//
// This file is licensed under the Silabs License Agreement. See the file
// "Silabs_License_Agreement.txt" for details. Before using this software for
// any purpose, you must agree to the terms of that agreement.
//
// -----------------------------------------------------------------------------
#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

#include "hal-config-types.h"
#include BOARD_HEADER

#define HAL_SERIAL_APP_PORT 1

#define HAL_SERIAL_USART0_ENABLE 1
#define HAL_SERIAL_VCOM_ENABLE 1

#define HAL_USART0_BAUD_RATE             115200
#define HAL_USART0_FLOW_CONTROL          HAL_USART_FLOW_CONTROL_HW
#define HAL_USART0_RXSTART               16
#define HAL_USART0_RXSTOP                16
#define HAL_USART0_RX_QUEUE_SIZE         64
#define HAL_USART0_TX_QUEUE_SIZE         128

#endif //HAL_CONFIG_H
