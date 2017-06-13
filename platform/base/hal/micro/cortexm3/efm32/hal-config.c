// -----------------------------------------------------------------------------
// @file hal-config.c
// @brief HAL config initialization
//
// @section License
// <b>(C) Copyright 2016 Silicon Laboratories, http://www.silabs.com</b>
//
// This file is licensed under the Silabs License Agreement. See the file
// "Silabs_License_Agreement.txt" for details. Before using this software for
// any purpose, you must agree to the terms of that agreement.
//
// -----------------------------------------------------------------------------
#include PLATFORM_HEADER
#include "hal/hal.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "ecode.h"
#include "plugin/serial/com.h"
#include "plugin/serial/com_config.h"
#ifdef _EFR_DEVICE
#include "pti.h"
#endif
#if BSP_VCOM_IOEXP_ENABLE
#include "bsp.h"
#endif

static void halConfigClockInit(void)
{
  /* HFXO */
#if (HAL_CLK_HFCLK_SOURCE == HAL_CLK_HFCLK_SOURCE_HFXO)
  CMU_HFXOInit_TypeDef hfxoInit = BSP_CLK_HFXO_INIT;
  #if defined(_CMU_HFXOCTRL_MASK)
  uint16_t customStartupCtune;
  uint16_t customSteadyCtune;
  if (halInternalGetCtuneToken(&customStartupCtune, &customSteadyCtune)) {
    hfxoInit.ctuneStartup = customStartupCtune;
    hfxoInit.ctuneSteadyState = customSteadyCtune;
  }
  #endif //_CMU_HFXOCTRL_MASK
  CMU_HFXOInit(&hfxoInit);

  /* Enable HFXO oscillator, and wait for it to be stable */
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

  /* Setting system HFXO frequency */
  SystemHFXOClockSet(BSP_CLK_HFXO_FREQ);

  /* Using HFXO as high frequency clock, HFCLK */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* HFRCO not needed when using HFXO */
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
#elif (HAL_CLK_HFCLK_SOURCE == HAL_CLK_HFCLK_SOURCE_HFRCO)
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
#else
  #error Must define HAL_CLK_HFCLK_SOURCE
#endif

  /* LFXO */
#if (HAL_CLK_LFCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFXO)
  /* LFXO Initialization is optional */
  #ifdef BSP_CLK_LFXO_INIT
  CMU_LFXOInit_TypeDef lfxoInit = BSP_CLK_LFXO_INIT;
  CMU_LFXOInit(&lfxoInit);
  #endif

  /* Enable LFXO oscillator, and wait for it to be stable */
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

  /* Setting system LFXO frequency */
  SystemLFXOClockSet(BSP_CLK_LFXO_FREQ);

  /* Using LFXO as low frequency clock, LFA/LFB */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);

  /* LFRCO not needed when using LFXO */
  CMU_OscillatorEnable(cmuOsc_LFRCO, false, false);
#elif (HAL_CLK_LFCLK_SOURCE == HAL_CLK_LFCLK_SOURCE_LFRCO)
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
  CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
#else
  #error Must define HAL_CLK_LFCLK_SOURCE
#endif
}

Ecode_t halConfigInit(void)
{
  Ecode_t status;

#if defined (_EMU_DCDCCTRL_MASK)
#if BSP_DCDC_PRESENT
  EMU_DCDCInit_TypeDef dcdcInit = BSP_DCDC_INIT;
  #if HAL_DCDC_BYPASS
  dcdcInit.dcdcMode = emuDcdcMode_Bypass;
  #endif
  EMU_DCDCInit(&dcdcInit);
#else
  EMU_DCDCPowerOff();
#endif
#endif //_EMU_DCDCCTRL_MASK

  halConfigClockInit();

#if (HAL_BUTTON_COUNT > 0)
  halInternalInitButton();
#endif

#if (HAL_LED_COUNT > 0)
  halInternalInitLed();
#endif

#if (HAL_GPIO_MAX > 0)
  halConfigInitGpio();
#endif

#ifdef _EFR_DEVICE
#if (HAL_PTI_ENABLE)
  #if HAL_PTI_MODE == HAL_PTI_MODE_SPI
  // SPI Mode
    #define PTI_MODE RADIO_PTI_MODE_SPI
  #elif HAL_PTI_MODE == HAL_PTI_MODE_UART
  // UART Mode
    #define PTI_MODE RADIO_PTI_MODE_UART
  // DCLK unused in UART mode
    #undef BSP_PTI_DCLK_LOC
    #undef BSP_PTI_DCLK_PORT
    #undef BSP_PTI_DCLK_PIN
    #define BSP_PTI_DCLK_LOC  0
    #define BSP_PTI_DCLK_PORT 0
    #define BSP_PTI_DCLK_PIN  0
  #elif HAL_PTI_MODE == HAL_PTI_MODE_UART_ONEWIRE
  // Onewire UART Mode
    #define PTI_MODE RADIO_PTI_MODE_UART_ONEWIRE
  // DCLK unused in onewire mode
    #undef BSP_PTI_DCLK_LOC
    #undef BSP_PTI_DCLK_PORT
    #undef BSP_PTI_DCLK_PIN
    #define BSP_PTI_DCLK_LOC    0
    #define BSP_PTI_DCLK_PORT   0
    #define BSP_PTI_DCLK_PIN    0
  // DFRAME unused in onewire mode
    #undef BSP_PTI_DFRAME_LOC
    #undef BSP_PTI_DFRAME_PORT
    #undef BSP_PTI_DFRAME_PIN
    #define BSP_PTI_DFRAME_LOC  0
    #define BSP_PTI_DFRAME_PORT 0
    #define BSP_PTI_DFRAME_PIN  0
  #else
    #error HAL_PTI_MODE not recognized
  #endif //HAL_PTI_MODE
#else //HAL_PTI_ENABLE
  // PTI not enabled
  #define PTI_MODE RADIO_PTI_MODE_DISABLED
  // Reset all settings to 0 when disabled
  #undef HAL_PTI_BAUD_RATE
  #undef BSP_PTI_DOUT_LOC
  #undef BSP_PTI_DOUT_PORT
  #undef BSP_PTI_DOUT_PIN
  #undef BSP_PTI_DCLK_LOC
  #undef BSP_PTI_DCLK_PORT
  #undef BSP_PTI_DCLK_PIN
  #undef BSP_PTI_DFRAME_LOC
  #undef BSP_PTI_DFRAME_PORT
  #undef BSP_PTI_DFRAME_PIN
  #define HAL_PTI_BAUD_RATE   0
  #define BSP_PTI_DOUT_LOC    0
  #define BSP_PTI_DOUT_PORT   0
  #define BSP_PTI_DOUT_PIN    0
  #define BSP_PTI_DCLK_LOC    0
  #define BSP_PTI_DCLK_PORT   0
  #define BSP_PTI_DCLK_PIN    0
  #define BSP_PTI_DFRAME_LOC  0
  #define BSP_PTI_DFRAME_PORT 0
  #define BSP_PTI_DFRAME_PIN  0
#endif //HAL_PTI_ENABLE
  RADIO_PTIInit_t ptiInit = { PTI_MODE,
                              HAL_PTI_BAUD_RATE,   /* 1.6 MHz baud */
                              BSP_PTI_DOUT_LOC,    /* DOUT location */
                              BSP_PTI_DOUT_PORT,   /* DOUT port */
                              BSP_PTI_DOUT_PIN,    /* DOUT pin */
                              BSP_PTI_DCLK_LOC,    /* DCLK location */
                              BSP_PTI_DCLK_PORT,   /* DCLK port */
                              BSP_PTI_DCLK_PIN,    /* DCLK pin */
                              BSP_PTI_DFRAME_LOC,  /* DFRAME location */
                              BSP_PTI_DFRAME_PORT, /* DFRAME port */
                              BSP_PTI_DFRAME_PIN }; /* DFRAME pin */
  RADIO_PTI_Init(&ptiInit);
#endif // _EFR_DEVICE

#if (HAL_PTA_ENABLE)
  HalPtaOptions ptaOptions = HAL_PTA_OPTIONS;
  halPtaSetOptions(ptaOptions);
  halInternalInitPta();
#endif

#if (HAL_RHO_ENABLE)
  halInternalInitRadioHoldOff();
#endif

#if (HAL_ANTDIV_ENABLE)
  halInternalInitAntennaDiversity();
#endif

#if (HAL_SERIAL_USART0_ENABLE)
  COM_Init_t initDataUsart0 = (COM_Init_t) COM_USART0_DEFAULT;
  status = COM_Init(comPortUsart0, &initDataUsart0);
  if (status != ECODE_OK) {
    return status;
  }
#endif
#if (HAL_SERIAL_USART1_ENABLE)
  COM_Init_t initDataUsart1 = (COM_Init_t) COM_USART1_DEFAULT;
  status = COM_Init(comPortUsart1, &initDataUsart1);
  if (status != ECODE_OK) {
    return status;
  }
#endif
#if (HAL_SERIAL_USART2_ENABLE)
  COM_Init_t initDataUsart2 = (COM_Init_t) COM_USART2_DEFAULT;
  status = COM_Init(comPortUsart2, &initDataUsart2);
  if (status != ECODE_OK) {
    return status;
  }
#endif
#if (HAL_SERIAL_USART3_ENABLE)
  COM_Init_t initDataUsart3 = (COM_Init_t) COM_USART3_DEFAULT;
  status = COM_Init(comPortUsart3, &initDataUsart3);
  if (status != ECODE_OK) {
    return status;
  }
#endif
#if (HAL_SERIAL_LEUART0_ENABLE)
  COM_Init_t initDataLeuart0 = (COM_Init_t) COM_LEUART0_DEFAULT;
  status = COM_Init(comPortLeuart0, &initDataLeuart0);
  if (status != ECODE_OK) {
    return status;
  }
#endif
#if (HAL_SERIAL_LEUART1_ENABLE)
  COM_Init_t initDataLeuart1 = (COM_Init_t) COM_LEUART1_DEFAULT;
  status = COM_Init(comPortLeuart1, &initDataLeuart1);
  if (status != ECODE_OK) {
    return status;
  }
#endif

#if (HAL_SERIAL_VCOM_ENABLE)
  #if BSP_VCOM_IOEXP_ENABLE
  BSP_Init(BSP_INIT_IOEXP);
  BSP_PeripheralAccess(BSP_IOEXP_VCOM, true);
  #else
  GPIO_PinModeSet(BSP_VCOM_ENABLE_PORT, BSP_VCOM_ENABLE_PIN, gpioModePushPull, 1);
  #endif
#endif

#if (HAL_SERIAL_RXWAKE_ENABLE)
  COM_RxGpioWakeInit();
#endif

#if (HAL_ENABLE_WATCHDOG)
  halInternalEnableWatchDog();
#endif
  return ECODE_OK;
}
