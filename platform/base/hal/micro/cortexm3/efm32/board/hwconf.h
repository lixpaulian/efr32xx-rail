/** @file hal/micro/cortexm3/efm32/board/hwconf.h
 * Board header for use with HWCONF
 * See @ref board for detailed documentation.
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.                        *80*-->
 */

/** @addtogroup board
 *  @brief Functions and definitions specific to the breakout board.
 *
 *@{
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#ifndef EMBER_AF_USE_HWCONF
#warning The hwconf board header is designed to be used with HW Configurator.
#endif

#include "inc/InitDevice.h"

/**************             Main Board Definitions               **************/

/** @name LED Definitions
 *
 * The following are used to aid in the abstraction with the LED
 * connections.  The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have different connections.  The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 *
 * The ::HalBoardLedPins enum values should always be used when manipulating the
 * state of LEDs, as they directly refer to the GPIOs to which the LEDs are
 * connected.
 *
 * \b Note: LEDs 0 and 1 are on the mainboard. There are no LEDs on the RCM
 *
 * \b Note: LEDs 2 and 3 simply redirects to LEDs 0 and 1.
 */
//@{

/**
 * @brief Assign each GPIO with an LED connected to a convenient name.
 * ::BOARD_ACTIVITY_LED and ::BOARD_HEARTBEAT_LED provide a further layer of
 * abstraction on top of the 3 LEDs for verbose coding.
 */
enum HalBoardLedPins {
  BOARDLED0 = 0,
  BOARDLED1 = 1,
  BOARDLED2 = BOARDLED0,
  BOARDLED3 = BOARDLED1,
  BOARD_ACTIVITY_LED  = BOARDLED0,
  BOARD_HEARTBEAT_LED = BOARDLED1
};
#define BSP_NO_OF_LEDS 2
#define BSP_GPIO_LEDARRAY_INIT  {{BSP_GPIO_LED0_PORT,BSP_GPIO_LED0_PIN}, {BSP_GPIO_LED1_PORT,BSP_GPIO_LED1_PIN}}

/** @} END OF LED DEFINITIONS  */

/** @name Button Definitions
 *
 * The following are used to aid in the abstraction with the Button
 * connections.  The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have different connections.  The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 *
 * The BUTTONn macros should always be used with manipulating the buttons
 * as they directly refer to the GPIOs to which the buttons are connected.
 *
 */
//@{
/**
 * @brief The interrupt service routine for all buttons.
 */
#define BUTTON_ISR buttonIrqCallback
/**
 * @brief The actual GPIO BUTTON0 is connected to.  This define should
 * be used whenever referencing BUTTON0.
 */
#define BUTTON0         (BSP_GPIO_PB0_PIN)
/**
 * @brief The GPIO port for BUTTON0.
 */
#define BUTTON0_PORT        (BSP_GPIO_PB0_PORT)

/**
 * @brief The actual GPIO BUTTON1 is connected to.  This define should
 * be used whenever referencing BUTTON1, such as controlling if pieces
 * are compiled in.
 */
#define BUTTON1         (BSP_GPIO_PB1_PIN)
/**
 * @brief The GPIO input register for BUTTON1.
 */
#define BUTTON1_PORT        (BSP_GPIO_PB1_PORT)
//@} //END OF BUTTON DEFINITIONS


/** @name UART Definitions
 *
 * The following are used to aid in the abstraction with the UART.
 * The microcontroller-specific sources use these definitions so they are able 
 * to work across a variety of boards which could have different connections.  
 * The names and ports/pins used below are intended to match with a schematic 
 * of the system to provide the abstraction.
 *
 */
//@{
/**
 * @brief Enable VCOM Pin, passthrough UART via the WSTK
 */
#define halEnableVCOM()

// Turn on Flow Control
#define COM_USART0_HW_FC

// Define USART0 TX/RX Location
#define USART0_TX_LOCATION _USART_ROUTELOC0_TXLOC_LOC0
#define USART0_RX_LOCATION _USART_ROUTELOC0_RXLOC_LOC0

// Define USART0 flow control pins
#ifndef USART0_CTS_PORT
  #define USART0_CTS_PORT gpioPortA
#endif
#ifndef USART0_CTS_PIN
  #define USART0_CTS_PIN 2
#endif
#ifndef USART0_RTS_PORT
  #define USART0_RTS_PORT gpioPortA
#endif
#ifndef USART0_RTS_PIN
  #define USART0_RTS_PIN 3
#endif
//@} //END OF UART DEFINITIONS
//
/** @name External Flash Definitions
 *
 * The following are used to aid in the abstraction with the external flash.
 * The microcontroller-specific sources use these definitions so they are able 
 * to work across a variety of boards which could have different connections.  
 * The names and ports/pins used below are intended to match with a schematic 
 * of the system to provide the abstraction.
 *
 * This default configuration sets the location to the WSTK expansion header
 */
//@{
// Define external flash pins

#ifndef EXTERNAL_FLASH_USART0_TXLOC
#define EXTERNAL_FLASH_USART0_TXLOC             USART_ROUTELOC0_TXLOC_LOC11
#endif
#ifndef EXTERNAL_FLASH_USART0_RXLOC
#define EXTERNAL_FLASH_USART0_RXLOC             USART_ROUTELOC0_RXLOC_LOC11
#endif
#ifndef EXTERNAL_FLASH_USART0_CLKLOC
#define EXTERNAL_FLASH_USART0_CLKLOC            USART_ROUTELOC0_CLKLOC_LOC11
#endif

#ifndef EXTERNAL_FLASH_MOSI_PORT
  #define EXTERNAL_FLASH_MOSI_PORT    gpioPortC
#endif
#ifndef EXTERNAL_FLASH_MOSI_PIN
  #define EXTERNAL_FLASH_MOSI_PIN     6
#endif
#ifndef EXTERNAL_FLASH_MISO_PORT
  #define EXTERNAL_FLASH_MISO_PORT    gpioPortC
#endif
#ifndef EXTERNAL_FLASH_MISO_PIN
  #define EXTERNAL_FLASH_MISO_PIN     7
#endif
#ifndef EXTERNAL_FLASH_CLK_PORT
  #define EXTERNAL_FLASH_CLK_PORT     gpioPortC
#endif
#ifndef EXTERNAL_FLASH_CLK_PIN
  #define EXTERNAL_FLASH_CLK_PIN      8
#endif
#ifndef EXTERNAL_FLASH_CS_PORT
  #define EXTERNAL_FLASH_CS_PORT      gpioPortC
#endif
#ifndef EXTERNAL_FLASH_CS_PIN
  #define EXTERNAL_FLASH_CS_PIN       9
#endif
//@} //END OF EXTERNAL FLASH DEFINITIONS

/**************    PRO2+ Transceiver Configuration Options    **************/
#if     (defined(PHY_PRO2PLUS) || defined(PHY_PRO2) || defined(PHY_DUAL))

/*
 * PRO2+ GPIOs:
 * Pin - Signal       Use               PwrUpCfg  PwrDnCfg #define option
 * GP0 * RX_STATE     T/R switch        RX_STATE  (same)
 * GP1 - CTS          CTS option        CTS       (same)
 * GP2 - TX_STATE     T/R switch        TX_STATE  (same)
 * GP3 - PTI          1-wire PTI        PKT_TRACE (same)
 * NIRQ- NIRQ         NIRQ              NIRQ      (same)
 * SDO - SDO          SDO               SDO       (same)
 * GEN_CONFIG         Drive Strength    LOW       (same)
 */

/** @name PRO2+ Interface Definitions
 *
 * The following provide the spi-master and ext-device driver configurations
 * for interfacing the PRO2+ transceiver.
 */

/** @brief PRO2+ SPI-Master configuration
 */
#define PRO2_SPI_PORT USART1
#define PRO2_SPI_MHZ  1  // SPI clock rate in MHz //@TODO: Raise it to 10!

/** @brief PRO2+ Ext-Device configuration
 */
#define ENABLE_EXT_DEVICE       1
// SDN configuration
#define RF_SDN_PORT             gpioPortC // PC11 (BRD4155A compatible)
#define RF_SDN_PIN              11
// INT configuration
#define RF_INT_PORT             gpioPortC // PC10 (BRD4155A compatible)
#define RF_INT_PIN              10
// nSEL configuration
#define RF_USARTRF_CS_PORT      gpioPortC // PC9  (BRD4155A compatible)
#define RF_USARTRF_CS_PIN       9

//@} //END OF PRO2+ INTERFACE DEFINITIONS

/** @name PRO2+ GPIO Configuration
 */
//TODO: Currently no public PRO2+ API headers exist, so extract useful
//TODO: CMD_GPIO_ definitions to allow for custom GPIO configurations.
#ifndef CMD_GPIO_PIN_CFG
#define CMD_GPIO_PIN_CFG 0x13
#define CMD_GPIO_PIN_CFG_ARG_GPIO_PULL_CTL_MASK 0x40
#define CMD_GPIO_PIN_CFG_ARG_GPIO_PULL_CTL_LSB 6
#define CMD_GPIO_PIN_CFG_ARG_GPIO_PULL_CTL_ENUM_PULL_DIS 0
#define CMD_GPIO_PIN_CFG_ARG_GPIO_PULL_CTL_ENUM_PULL_EN 1
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_MASK 0x3f
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_LSB 0
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING 0
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_TRISTATE 1
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DRIVE0 2
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DRIVE1 3
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_INPUT 4
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_32K_CLK 5
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_BOOT_CLK 6
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DIV_CLK 7
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_CTS 8
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_INV_CTS 9
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_CMD_OVERLAP 10
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_SDO 11
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_POR 12
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_CAL_WUT 13
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_WUT 14
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_EN_PA 15
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_TX_DATA_CLK 16
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_RX_DATA_CLK 17
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_EN_LNA 18
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_TX_DATA 19
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_RX_DATA 20
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_RX_RAW_DATA 21
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_ANTENNA_1_SW 22
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_ANTENNA_2_SW 23
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_VALID_PREAMBLE 24
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_INVALID_PREAMBLE 25
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_SYNC_WORD_DETECT 26
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_CCA 27
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_IN_SLEEP 28
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_PKT_TRACE 29
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_BUFDIV_CLK 30
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_TX_RX_DATA_CLK 31
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_TX_STATE 32
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_RX_STATE 33
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_RX_FIFO_FULL 34
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_TX_FIFO_EMPTY 35
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_LOW_BATT 36
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_CCA_LATCH 37
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_HOPPED 38
#define CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_HOP_TABLE_WRAP 39
#define CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_MASK 0x60
#define CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_LSB 5
#define CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_ENUM_HIGH 0
#define CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_ENUM_MED_HIGH 1
#define CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_ENUM_MED_LOW 2
#define CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_ENUM_LOW 3
#endif//CMD_GPIO_PIN_CFG

//TODO: Want to use PRO2_GPIO_3 from phy.h here but too painful to #include it
//TODO: So 0=none, 1=GPIO_0, 2=GPIO_1, 3=GPIO_2, 4=GPIO_3
#define PRO2_PTI_GPIO (1+3) // GPIO3 for PTI data
#define PRO2_POWERUP_GPIO_CFG()                                               \
                   8, /*Number of bytes in following PRO2+ command*/          \
                   CMD_GPIO_PIN_CFG,                                          \
        /*GPIO0*/  CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_RX_STATE,         \
        /*GPIO1*/  CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_CTS,              \
        /*GPIO2*/  CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_TX_STATE,         \
        /*GPIO3*/  CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_PKT_TRACE,        \
        /*NIRQ */  CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING,        \
        /*SDO  */  CMD_GPIO_PIN_CFG_ARG_GPIO_GPIO_MODE_ENUM_DONOTHING,        \
        /*GEN  */ (CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_ENUM_LOW      \
                   << CMD_GPIO_PIN_CFG_ARG_GEN_CONFIG_DRV_STRENGTH_LSB),      \
                   0, /*Number of bytes in following PRO2+ command*/          \
        /*(end of list)*/

// There's no need for a PRO2_POWERDOWN_GPIO_CFG because SDN will kill the PRO2+
//@} //END OF PRO2+ GPIO CONFIGURATION

/**
 * @brief Initialize PRO2 configuration variables.
 */
#define DEFINE_SPI_CONFIG_VARIABLES()                                         \
  const USART_TypeDef* pro2SpiPort = PRO2_SPI_PORT;                           \
  const uint8_t pro2SpiClockMHz = PRO2_SPI_MHZ;                               \
  const uint8_t pro2GpioCfgPowerUp[] = { PRO2_POWERUP_GPIO_CFG() };           \
  int8_t phy2PtiGpio = PRO2_PTI_GPIO;

#ifndef PRO2_SHUTDOWN_WHEN_ASLEEP
#define PRO2_SHUTDOWN_WHEN_ASLEEP 1 // 0=SLEEP PRO2+, 1=SDN PRO2+
#endif//PRO2_SHUTDOWN_WHEN_ASLEEP

#if     PRO2_SHUTDOWN_WHEN_ASLEEP
  // Override the below definitions to shut down the PRO2+ when sleeping
  #define halInternalPowerDownBoard() do {                           \
    extern void emRadioPowerDown(void);                              \
    emRadioPowerDown();                                              \
  } while(0)

  #define halInternalPowerUpBoard() do {                             \
    extern void emRadioPowerUp(void);                                \
    emRadioPowerUp();                                                \
  } while(0)
#endif//PRO2_SHUTDOWN_WHEN_ASLEEP

#ifndef PHY_DUAL
  // Don't configure EFR32 PTI; that radio is effectively disabled
  #define halInternalConfigPti() /* no-op */
#endif//PHY_DUAL

#ifdef  EXTERNAL_FLASH_CS_PORT
  #define halInternalInitDataFlash() do {                            \
    /* Deselect DataFlash, in case there's no pullup on the board */ \
    GPIO_PinModeSet(EXTERNAL_FLASH_CS_PORT, EXTERNAL_FLASH_CS_PIN,   \
                    gpioModePushPull, 1);                            \
  } while(0)
#endif//EXTERNAL_FLASH_CS_PORT

#endif//(defined(PHY_PRO2PLUS) || defined(PHY_PRO2) || defined(PHY_DUAL))

/**************            Common Configuration Options          **************/

/** @name Packet Trace
 *
 * Packet Trace Interface (PTI) will be turned on by default unless 
 * ::DISABLE_PTI is defined
 *
 * @note PTI uses PB11-13
 */
//@{
#define RADIO_PTI_INIT                                                        \
  {                                                                           \
    RADIO_PTI_MODE_UART,    /* Simplest output mode is UART mode */           \
    1600000,                /* Choose 1.6 MHz for best compatibility */       \
    6,                      /* WSTK uses location 6 for DOUT */               \
    gpioPortB,              /* Get the port for this loc */                   \
    12,                     /* Get the pin, location should match above */    \
    6,                      /* WSTK uses location 6 for DCLK */               \
    gpioPortB,              /* Get the port for this loc */                   \
    11,                     /* Get the pin, location should match above */    \
    6,                      /* WSTK uses location 6 for DFRAME */             \
    gpioPortB,              /* Get the port for this loc */                   \
    13,                     /* Get the pin, location should match above */    \
  }

#ifndef halInternalConfigPti
#ifndef DISABLE_PTI
#define halInternalConfigPti() do {                                \
  RADIO_PTIInit_t ptiInit = RADIO_PTI_INIT;                        \
  RADIO_PTI_Init(&ptiInit);                                        \
} while(0)
#else
#define halInternalConfigPti() do {                                \
  RADIO_PTIInit_t ptiInit = RADIO_PTI_INIT;                        \
  ptiInit.mode = RADIO_PTI_MODE_DISABLED;                          \
  RADIO_PTI_Init(&ptiInit);                                        \
} while(0)
#endif
#endif//halInternalConfigPti

//@} //END OF PACKET TRACE DEFINITIONS

/** @name DC-DC
 *
 * The DC to DC Interface will be turned on by default unless 
 * ::DISABLE_DCDC is defined, in which case the DC-DC will be put
 * in bypass mode.
 *
 * @note Use EMU_DCDCPowerOff() to poweroff the DC-DC if not connected
 * 
 * @note DC-DC configuration is write once on power-on, so a POR reset
 * is required to change DC-DC mode.
 */
//@{
#ifdef DISABLE_DCDC
  #warning DISABLE_DCDC has no effect when HWCONF is used.
#endif
//@} //END OF DC-DC DEFINITIONS

#ifndef halInternalInitPrs
#define halInternalInitPrs()  do {                                                               \
} while(0)
#endif //halInternalInitPrs

#ifndef halInternalInitDataFlash
#define halInternalInitDataFlash()  do {                             \
} while(0)
#endif //halInternalInitDataFlash

/**************            Essential Board Functions             **************/

/**
 * @name halInternalInitBoard
 *
 * Called on startup from halInit(). Should contain calls to all board
 * specific startup sequences.
 * 
 */
#define halInternalInitBoard() do {                                \
 enter_DefaultMode_from_RESET();                                   \
 halInternalInitButton();                                          \
 halInternalInitDataFlash();                                       \
 halInternalPowerUpBoard();                                        \
 halInternalConfigPti();                                           \
 halInternalInitRadioHoldOff();                                    \
 halInternalInitPta();                                             \
 halInternalInitPrs();                                             \
 halInternalInitAntennaDiversity();                                \
} while(0)

/**
 * @name halInternalPowerUpBoard
 *
 * Call after coming out of sleep to restore any features which need to
 * power off for sleep
 * 
 */
#ifndef halInternalPowerUpBoard
  #define halInternalPowerUpBoard() do {                             \
  } while(0)
#endif

/**
 * @name halInternalPowerDownBoard
 *
 * Call before going to sleep to power down any features which need to
 * off for sleep
 * 
 */
#ifndef halInternalPowerDownBoard
  #define halInternalPowerDownBoard() do {                           \
  } while(0)
#endif

/**
 *  Instantiate any variables that are used in the board header
 */
#ifndef DEFINE_BOARD_GPIO_CFG_VARIABLES
  #define DEFINE_BOARD_GPIO_CFG_VARIABLES()
#endif
/**
 * Define TEMP_SENSOR to satisfy Ember HAL. Should not be used
 */
#define TEMP_SENSOR_ADC_CHANNEL 0
#define TEMP_SENSOR_SCALE_FACTOR 0

/**************              Radio Module Options                **************/

/**
 * PA Configuration
 */
#ifndef RADIO_PA_2P4_INIT
#define RADIO_PA_2P4_INIT                                                     \
  {                                                                           \
    PA_SEL_2P4_HP,    /* Power Amplifier mode */                              \
    PA_VOLTMODE_DCDC, /* Power Amplifier vPA Voltage mode */                  \
    100,              /* Desired output power in dBm * 10 */                  \
    0,                /* Output power offset in dBm * 10 */                   \
    10,               /* Desired ramp time in us */                           \
  }
#endif//RADIO_PA_2P4_INIT

#endif //__BOARD_H__

/** @} END Board Specific Functions */

/** @} END addtogroup */
