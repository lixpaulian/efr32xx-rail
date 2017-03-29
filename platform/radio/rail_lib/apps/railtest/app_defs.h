/***************************************************************************//**
 * @file app_defs.h
 * @brief Defines to configure the main test application for this specific use
 * use case. These defines set things like channels, packet configuration, etc.
 * @copyright Copyright 2015 Silicon Laboratories, Inc. http://www.silabs.com
 ******************************************************************************/

#ifndef __APP_DEFS_H_
#define __APP_DEFS_H_

/******************************************************************************
 * Application Memory Defintions
 *****************************************************************************/
// Memory manager configuration
#define BUFFER_POOL_SIZE 30
#define MAX_BUFFER_SIZE  512
#define MAX_QUEUE_LENGTH (BUFFER_POOL_SIZE)

// General application memory sizes
#define APP_MAX_PACKET_LENGTH  (MAX_BUFFER_SIZE - 12) /* sizeof(RAIL_RxPacketInfo_t) == 12) */
#define APP_CONTINUOUS_TRANSFER_PERIOD 250UL
#define APP_COMMAND_INTERFACE_BUFFER_SIZE 256
#define APP_DISPLAY_BUFFER_SIZE 64

/******************************************************************************
 * Application User Interface
 *****************************************************************************/
#define APP_DEMO_STRING_INIT "RAIL Test App"

#endif
