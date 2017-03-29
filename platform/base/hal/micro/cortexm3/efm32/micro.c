// -----------------------------------------------------------------------------
// @file
// @brief EFM micro specific full HAL functions
//
// @author Silicon Laboratories Inc.
// @version 1.0.0
//
// @section License
// <b>(C) Copyright 2014 Silicon Laboratories, http://www.silabs.com</b>
//
// This file is licensed under the Silabs License Agreement. See the file
// "Silabs_License_Agreement.txt" for details. Before using this software for
// any purpose, you must agree to the terms of that agreement.
//
// -----------------------------------------------------------------------------
#include PLATFORM_HEADER
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_core.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_prs.h"
#include "rtcdriver.h"
#include "em_chip.h"
#include "gpiointerrupt.h"

#include "stack/include/ember.h"
#include "include/error.h"
#include "hal/hal.h"
#include "serial/serial.h"
#include "hal/micro/cortexm3/diagnostic.h"
#include "hal/micro/cortexm3/memmap.h"
#include "hal/micro/cortexm3/flash.h"

#ifdef _EFR_DEVICE
#include "hal/plugin/glib/graphics.h"
#include "pti.h"
#include "pa.h"
#include "tempdrv.h"
#include "sleep-efm32.h"
#endif

#ifdef BSP_STK
#include "bsp.h"
#endif

#ifdef RTOS
  #include "rtos/rtos.h"
#endif

#ifndef DEFAULT_PTA_OPTIONS
  #define DEFAULT_PTA_OPTIONS PTA_OPT_DISABLED
#endif
#ifndef CONST_PTA_OPTIONS
  #define CONST_PTA_OPTIONS (~PTA_OPT_DISABLED)
#endif
#define SET_BIT_MASK(src, mask, value) (src = (value ? (src | mask) : (src & (~mask))) )

HalPtaOptions halPtaOptions = DEFAULT_PTA_OPTIONS;
extern void emPhyCancelTransmit (void);
static void halStackRadioHoldOffPowerDown(void); // fwd ref
static void halStackRadioHoldOffPowerUp(void);   // fwd ref

static void GPIOINT_InitSafe(void)
{
  // Enable GPIO clock for configuring interrupts
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Turn on GPIO interrupts only if they weren't enabled elsewhere
  if (CORE_NvicIRQDisabled(GPIO_ODD_IRQn)
      || CORE_NvicIRQDisabled(GPIO_EVEN_IRQn)) {
    GPIOINT_Init();
  }
}

EmberStatus halPtaSetBool(HalPtaOptions option, bool value)
{
  HalPtaOptions ptaOptions = halPtaGetOptions();

  SET_BIT_MASK(ptaOptions, option, value);

  return halPtaSetOptions(ptaOptions);
}

HalPtaOptions halPtaGetOptions(void)
{
  return halPtaOptions;
}

EmberStatus halPtaSetOptions(HalPtaOptions options)
{
  EmberStatus status = EMBER_SUCCESS;

  HalPtaOptions oldOptions = halPtaOptions;
  if ( (options & CONST_PTA_OPTIONS) != (DEFAULT_PTA_OPTIONS & CONST_PTA_OPTIONS)) {
    //Return error if any options argument is unsupported or constant
    status = EMBER_BAD_ARGUMENT;
  } else {
    //Only modify public options
    halPtaOptions = options;
  }
  if ( (status == EMBER_SUCCESS)
     && (halPtaOptions & PTA_OPT_RHO_ENABLED) != (oldOptions & PTA_OPT_RHO_ENABLED) ) {
    status = halSetRadioHoldOff(options & PTA_OPT_RHO_ENABLED);
  }
#ifdef PTA_OPT_PTA_ENABLED
  if ( (status == EMBER_SUCCESS)
     && (options & PTA_OPT_PTA_ENABLED) != (oldOptions & PTA_OPT_PTA_ENABLED) ) {
    status = halPtaSetEnable(options & PTA_OPT_RHO_ENABLED);
  }
#endif //PTA_OPT_PTA_ENABLED
  return status;
}

#ifdef PTA_REQ_MAX_BACKOFF_MASK
static void ptaReqRandomBackoff()
{
  halCommonDelayMicroseconds(halCommonGetRandom() & PTA_REQ_MAX_BACKOFF_MASK);
}
#else //PTA_REQ_MAX_BACKOFF_MASK
#define ptaReqRandomBackoff()
#endif //PTA_REQ_MAX_BACKOFF_MASK

#ifdef _EFR_DEVICE
bool halInternalGetCtuneToken(uint16_t *startupCtune, uint16_t *steadyCtune)
{
  tokTypeMfgCTune ctuneMfgTok=0xFFFF;
  halCommonGetToken(&ctuneMfgTok, TOKEN_MFG_CTUNE);
  if(ctuneMfgTok!=0xFFFF) {
    *startupCtune = ctuneMfgTok;
    *steadyCtune = ctuneMfgTok;
    return true;
  }
  return false;
}

void halInternalEnableHFXO(void)
{
  #ifndef EMBER_AF_USE_HWCONF
    CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_WSTK_DEFAULT;
  #else
    CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
  #endif
  uint16_t customStartupCtune;
  uint16_t customSteadyCtune;
  if (halInternalGetCtuneToken(&customStartupCtune, &customSteadyCtune))
  {
    hfxoInit.ctuneStartup = customStartupCtune;
    hfxoInit.ctuneSteadyState = customSteadyCtune;
  }
  CMU_HFXOInit(&hfxoInit);

  /* Enable HFXO oscillator, and wait for it to be stable */
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

  /* Setting system HFXO frequency */
  SystemHFXOClockSet(38400000);

  /* Using HFXO as high frequency clock, HFCLK */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  /* HFRCO not needed when using HFXO */
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
}

// Provide HAL pointers to board-header-defined PA configuration(s)
// for use by App, RAIL, or PHY library.
#ifdef  RADIO_PA_2P4_INIT
 static const RADIO_PAInit_t paInit2p4 = RADIO_PA_2P4_INIT;
 const RADIO_PAInit_t* halInternalPa2p4GHzInit = &paInit2p4;
#else//!RADIO_PA_2P4_INIT
 const RADIO_PAInit_t* halInternalPa2p4GHzInit = NULL;
#endif//RADIO_PA_2P4_INIT

#ifdef  RADIO_PA_SUBGIG_INIT
 static const RADIO_PAInit_t paInitSub = RADIO_PA_SUBGIG_INIT;
 const RADIO_PAInit_t* halInternalPaSubGHzInit = &paInitSub;
#else//!RADIO_PA_SUBGIG_INIT
 const RADIO_PAInit_t* halInternalPaSubGHzInit = NULL;
#endif//RADIO_PA_SUBGIG_INIT

#endif//_EFR_DEVICE

// halInit is called on first initial boot, not on wakeup from sleep.
void halInit(void)
{
  /* Configure board. Select either EBI or SPI mode. */
#ifdef _EFR_DEVICE
  // HWCONF will handle clock initialization
  #ifndef EMBER_AF_USE_HWCONF
    CHIP_Init();
    halInternalEnableDCDC();
    halInternalEnableHFXO();
    CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
    CMU_ClockEnable(cmuClock_CORELE, true);
  #endif
  halInternalInitBoard();
 #ifdef  EMBER_STACK_CONNECT
  // In Connect we need to initialize the PA with the subGHz parameters.
  // But, only in case where we are not using the board in the 2.4 GHz band.
 #ifdef  CONNECT_2_4_PHY
  RADIO_PA_Init((RADIO_PAInit_t*)halInternalPa2p4GHzInit);
 #else//!CONNECT_2_4_PHY
  RADIO_PA_Init((RADIO_PAInit_t*)halInternalPaSubGHzInit);
 #endif//CONNECT_2_4_PHY
 #else//!EMBER_STACK_CONNECT
  // Other stacks init their PA later
 #endif//EMBER_STACK_CONNECT
  EMU_UnlatchPinRetention();
  RTCDRV_Init();
  TEMPDRV_Init();
 #ifndef DISABLE_WATCHDOG
  halInternalEnableWatchDog();
 #endif
  EMU_EM4Init_TypeDef em4Init = EMU_EM4INIT_DEFAULT;
  em4Init.em4State = emuEM4Hibernate;
  EMU_EM4Init(&em4Init);
  halInternalEm4Wakeup();
#elif defined(_EZR_DEVICE)
    CHIP_Init();
    halInternalInitButton();
    halInternalInitLed();
    halInternalInitRadioHoldOff();
    halInternalInitVCPPins();
    CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
    CMU_OscillatorEnable(cmuOsc_LFRCO, true, true);
    CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFRCO);
    CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
    RTCDRV_Init();
    halInternalStartSymbolTimer();    // TODO: move to macInit or emRadioInit
#endif

  halInternalStartSystemTimer();
}

void halReboot(void)
{
  halInternalSysReset(RESET_SOFTWARE_REBOOT);
}

void halPowerDown(void)
{
  halInternalPowerDownBoard();
}

// halPowerUp is called from sleep state, not from first initial boot.
void halPowerUp(void)
{
  halInternalPowerUpBoard();
}

//If the board file defines runtime powerup/powerdown GPIO configuration,
//instantiate the variables and implement halStackRadioPowerDownBoard/
//halStackRadioPowerUpBoard which the stack will use to control the
//power state of radio specific GPIO.  If the board file does not define
//runtime GPIO configuration, the compile time configuration will work as
//it always has.
#ifdef DEFINE_SPI_CONFIG_VARIABLES
 DEFINE_SPI_CONFIG_VARIABLES();
#endif

// Instantiate global variables needed by the board header file
#ifdef DEFINE_BOARD_GPIO_CFG_VARIABLES
 DEFINE_BOARD_GPIO_CFG_VARIABLES();
#endif

void halStackRadioPowerDownBoard(void)
{
  (void) halPtaSetRxRequest(PTA_REQ_OFF, NULL);
  // Don't touch PTA Tx Request here
  halStackRadioHoldOffPowerDown();
}

void halStackRadio2PowerDownBoard(void)
{
  // Neither PTA nor RHO are supported on Radio2
}

void halStackRadioPowerUpBoard(void)
{
  halStackRadioHoldOffPowerUp();
}

void halStackRadio2PowerUpBoard(void)
{
  // Neither PTA nor RHO are supported on Radio2
}

void halStackRadioPowerMainControl(bool powerUp)
{
  if (powerUp) {
    halRadioPowerUpHandler();
  } else {
    halRadioPowerDownHandler();
  }
}

void halStackProcessBootCount(void)
{
  //Note: Because this always counts up at every boot (called from emberInit),
  //and non-volatile storage has a finite number of write cycles, this will
  //eventually stop working.  Disable this token call if non-volatile write
  //cycles need to be used sparingly.
#if defined(CREATOR_STACK_BOOT_COUNTER)
  halCommonIncrementCounterToken(TOKEN_STACK_BOOT_COUNTER);
#endif
}

PGM_P halGetResetString(void)
{
  // Table used to convert from reset types to reset strings.
  #define RESET_BASE_DEF(basename, value, string)  string,
  #define RESET_EXT_DEF(basename, extname, extvalue, string)  /*nothing*/
  static PGM char resetStringTable[][4] = {
    #include "reset-def.h"
  };
  #undef RESET_BASE_DEF
  #undef RESET_EXT_DEF

  return resetStringTable[halGetResetInfo()];
}

// Note that this API should be used in conjunction with halGetResetString
//  to get the full information, as this API does not provide a string for
//  the base reset type
PGM_P halGetExtendedResetString(void)
{

  // Create a table of reset strings for each extended reset type
  typedef PGM char ResetStringTableType[][4];
  #define RESET_BASE_DEF(basename, value, string)   \
                         }; static ResetStringTableType basename##ResetStringTable = {
  #define RESET_EXT_DEF(basename, extname, extvalue, string)  string,
  {
    #include "reset-def.h"
  };
  #undef RESET_BASE_DEF
  #undef RESET_EXT_DEF

  // Create a table of pointers to each of the above tables
  #define RESET_BASE_DEF(basename, value, string)  (ResetStringTableType *)basename##ResetStringTable,
  #define RESET_EXT_DEF(basename, extname, extvalue, string)  /*nothing*/
  static ResetStringTableType * PGM extendedResetStringTablePtrs[] = {
    #include "reset-def.h"
  };
  #undef RESET_BASE_DEF
  #undef RESET_EXT_DEF

  uint16_t extResetInfo = halGetExtendedResetInfo();
  // access the particular table of extended strings we are interested in
  ResetStringTableType *extendedResetStringTable =
                    extendedResetStringTablePtrs[RESET_BASE_TYPE(extResetInfo)];

  // return the string from within the proper table
  return (*extendedResetStringTable)[((extResetInfo)&0xFF)];
}

// Translate EM3xx reset codes to the codes previously used by the EM2xx.
// If there is no corresponding code, return the EM3xx base code with bit 7 set.
uint8_t halGetEm2xxResetInfo(void)
{
  uint8_t reset = halGetResetInfo();

  // Any reset with an extended value field of zero is considered an unknown
  // reset, except for FIB resets.
  if ( (RESET_EXTENDED_FIELD(halGetExtendedResetInfo()) == 0) &&
       (reset != RESET_FIB) ) {
     return EM2XX_RESET_UNKNOWN;
  }

 switch (reset) {
  case RESET_UNKNOWN:
    return EM2XX_RESET_UNKNOWN;
  case RESET_BOOTLOADER:
    return EM2XX_RESET_BOOTLOADER;
  case RESET_EXTERNAL:      // map pin resets to poweron for EM2xx compatibility
//    return EM2XX_RESET_EXTERNAL;
  case RESET_POWERON:
    return EM2XX_RESET_POWERON;
  case RESET_WATCHDOG:
    return EM2XX_RESET_WATCHDOG;
  case RESET_SOFTWARE:
    return EM2XX_RESET_SOFTWARE;
  case RESET_CRASH:
    return EM2XX_RESET_ASSERT;
  default:
    return (reset | 0x80);      // set B7 for all other reset codes
  }
}

  // halPta Implementation:

  // Board header is expected to define:
  // PTA REQUEST signal (OUT or OUT_OD): [optional]
  // #define PTA_REQ_GPIO       // PORTx_PIN(y) (x=A/B/C/D/E/F/..., y=0-15)
  // #define PTA_REQ_GPIOCFG_NORMAL // gpioModePushPull
  // #define PTA_REQ_GPIOCFG_SHARED // gpioModeWiredAnd/Or
  // #define PTA_REQ_GPIOCFG    // PTA_REQ_GPIOCFG_NORMAL or _SHARED as above
  // #define PTA_REQ_ASSERTED   // 0 if negated logic; 1 if positive logic
  //
  // PTA GRANT signal (IN): [optional]
  // #define PTA_GNT_GPIO       // PORTx_PIN(y) (x=A/B/C/D/E/F/..., y=0-15)
  // #define PTA_GNT_GPIOCFG    // gpioModeInputPull[ASSERTED] or gpioModeInput
  // #define PTA_GNT_ASSERTED   // 0 if negated logic; 1 if positive logic
  // Note that REQ and GNT can share the same IRQn if necessary
  //
  // PTA PRIORITY signal (OUT): [optional]
  // #define PTA_PRI_GPIO       // PORTx_PIN(y) (x=A/B/C/D/E/F/..., y=0-15)
  // #define PTA_PRI_GPIOCFG    // gpioModePushPull or gpioModeWiredOr/And if shared
  // #define PTA_PRI_ASSERTED   // 0 if negated logic; 1 if positive logic

 #ifdef  PTA_REQ_GPIO

  static bool ptaReqAsserted = PTA_REQ_ASSERTED;

  static inline void ptaReqGpioSet(bool enable)
  {
    if (enable == ptaReqAsserted) {
      halGpioSet(PTA_REQ_GPIO);
    } else {
      halGpioClear(PTA_REQ_GPIO);
    }
  }

  static inline void ptaReqGpioCfg(void)
  {
    // Only configure GPIO if it was not set up prior
    if (halGpioGetConfig(PTA_REQ_GPIO) == gpioModeDisabled)
    {
     #if     PTA_REQ_ASSERTED
      halGpioClear(PTA_REQ_GPIO);
     #else//!PTA_REQ_ASSERTED
      halGpioSet(PTA_REQ_GPIO);
     #endif//PTA_REQ_ASSERTED
      halGpioSetConfig(PTA_REQ_GPIO, PTA_REQ_GPIOCFG);
    }
    // Here we sense asserted state is opposite of its current output state.
    ptaReqAsserted = !halGpioReadOutput(PTA_REQ_GPIO);
  }

  #define ptaReqGpioCfgIsShared() \
            (halGpioGetConfig(PTA_REQ_GPIO) != PTA_REQ_GPIOCFG_NORMAL)

  #define ptaReqGpioInAsserted() \
            (!!halGpioRead(PTA_REQ_GPIO) == !!ptaReqAsserted)

  #define ptaReqGpioOutAsserted() \
            (!!halGpioReadOutput(PTA_REQ_GPIO) == !!ptaReqAsserted)

  #define ptaReqAndGntIrqShared() \
            ( (defined(PTA_GNT_GPIO))                          /*Have GNT*/ \
            &&(GPIO_PIN(PTA_REQ_GPIO) == GPIO_PIN(PTA_GNT_GPIO)) )/*Shared IRQ*/

 #if     (ptaReqAndGntIrqShared())
  #define PTA_REQ_ISR PTA_GNT_ISR // REQUEST and GRANT share same IRQ & ISR
 #endif//(ptaReqAndGntIrqShared())

  static void PTA_REQ_ISR(uint8_t pin);

  static inline void ptaReqGpioIntAcknowledge(void)
  {
    GPIO_IntClear(GPIO_FLAG(PTA_REQ_GPIO));
  }

  static inline void ptaReqGpioIntDisable(void)
  {
    GPIO_IntDisable(GPIO_FLAG(PTA_REQ_GPIO));
    ptaReqGpioIntAcknowledge();
  }

  static inline void ptaReqGpioIntEnable(void)
  {
    // Disable triggering and clear any stale events
    GPIO_IntConfig(GPIO_PORT(PTA_REQ_GPIO), GPIO_PIN(PTA_REQ_GPIO),
                   false, false, false);
    // Register callback before setting up and enabling pin interrupt
    GPIOINT_CallbackRegister(GPIO_PIN(PTA_REQ_GPIO), PTA_REQ_ISR);
    // Enable deasserting edge interrupt only
    GPIO_IntConfig(GPIO_PORT(PTA_REQ_GPIO), GPIO_PIN(PTA_REQ_GPIO),
                   !ptaReqAsserted, ptaReqAsserted, true);
  }

 #else//!PTA_REQ_GPIO

  #define ptaReqGpioSet(enable)      /* no-op */
  #define ptaReqGpioCfg()            /* no-op */
  #define ptaReqGpioCfgIsShared()    0
  #define ptaReqGpioInAsserted()     0
  #define ptaReqGpioIntAcknowledge() /* no-op */
  #define ptaReqGpioIntDisable()     /* no-op */
  #define ptaReqGpioIntEnable()      /* no-op */
  #define ptaReqGpioOutAsserted()    1
  #define ptaReqAndGntIrqShared()    0

 #endif//PTA_REQ_GPIO

 #ifdef  PTA_GNT_GPIO

  static void PTA_GNT_ISR(uint8_t pin);

  static bool ptaGntAsserted = PTA_GNT_ASSERTED;
  static bool gntWasAsserted = false;

  static inline void ptaGntGpioCfg(void)
  {
    // Only configure GPIO if it was not set up prior
    if (halGpioGetConfig(PTA_GNT_GPIO) == gpioModeDisabled)
    {
     #if     PTA_GNT_ASSERTED
      halGpioSet(PTA_GNT_GPIO); // pull up
     #else//!PTA_GNT_ASSERTED
      halGpioClear(PTA_GNT_GPIO); // pull down
     #endif//PTA_GNT_ASSERTED
      halGpioSetConfig(PTA_GNT_GPIO, PTA_GNT_GPIOCFG);
    }
    // Here we sense asserted state is same as its current output state.
    ptaGntAsserted = !!halGpioReadOutput(PTA_GNT_GPIO);
  }

  #define ptaGntGpioInAsserted() \
            (!!halGpioRead(PTA_GNT_GPIO) == !!ptaGntAsserted)

  static inline void ptaGntGpioIntAcknowledge(void)
  {
    GPIO_IntClear(GPIO_FLAG(PTA_GNT_GPIO));
  }

  static inline void ptaGntGpioIntDisable(void)
  {
    GPIO_IntDisable(GPIO_FLAG(PTA_GNT_GPIO));
    ptaGntGpioIntAcknowledge();
  }

  static inline void ptaGntGpioIntEnable(void)
  {
    // Disable triggering and clear any stale events
    GPIO_IntConfig(GPIO_PORT(PTA_GNT_GPIO), GPIO_PIN(PTA_GNT_GPIO),
                   false, false, false);
    gntWasAsserted = false; // Ensures we won't miss GNT assertion
    // Register callbacks before setting up and enabling pin interrupt
    GPIOINT_CallbackRegister(GPIO_PIN(PTA_GNT_GPIO), PTA_GNT_ISR);
    // Enable both edges' interrupt
    GPIO_IntConfig(GPIO_PORT(PTA_GNT_GPIO), GPIO_PIN(PTA_GNT_GPIO),
                   true, true, true);
  }

  static inline void ptaGntGpioIntPend(void)
  {
    GPIO_IntSet(GPIO_FLAG(PTA_GNT_GPIO));
  }

 #else//!PTA_GNT_GPIO

  #define ptaGntGpioCfg()            /* no-op */
  #define ptaGntGpioInAsserted()     1
  #define ptaGntGpioIntAcknowledge() /* no-op */
  #define ptaReqGpioIntDisable()     /* no-op */
  #define ptaGntGpioIntDisable()     /* no-op */
  #define ptaGntGpioIntEnable()      /* no-op */
  #define ptaGntGpioIntPend()        (halInternalPtaOrRhoNotifyRadio())

 #endif//PTA_GNT_GPIO

 #ifdef  PTA_PRI_GPIO

  static bool ptaPriAsserted = PTA_PRI_ASSERTED;

  static inline void ptaPriGpioSet(bool enable)
  {
    if (enable != ptaPriAsserted) {
      halGpioClear(PTA_PRI_GPIO);
    } else {
      halGpioSet(PTA_PRI_GPIO);
    }
  }

  static inline void ptaPriGpioCfg(void)
  {
    // Only configure GPIO if it was not set up prior
    if (halGpioGetConfig(PTA_PRI_GPIO) == gpioModeDisabled)
    {
     #if     PTA_PRI_ASSERTED
      halGpioClear(PTA_PRI_GPIO);
     #else//!PTA_PRI_ASSERTED
      halGpioSet(PTA_PRI_GPIO);
     #endif//PTA_PRI_ASSERTED
      halGpioSetConfig(PTA_PRI_GPIO, PTA_PRI_GPIOCFG);
    }
    // Here we sense asserted state is opposite of its current output state.
    ptaPriAsserted = !halGpioReadOutput(PTA_PRI_GPIO);
  }

 #else//!PTA_PRI_GPIO

  #define ptaPriGpioSet(enable)      /* no-op */
  #define ptaPriGpioCfg()            /* no-op */

 #endif//PTA_PRI_GPIO

  static void halInternalPtaOrRhoNotifyRadio(void);
  static volatile bool ptaEnabled;
  static volatile halPtaReq_t txReq, rxReq;
  static volatile halPtaCb_t  txCb,  rxCb;

  // Must be called with interrupts disabled
  static void ptaUpdateReqIsr(void)
  {
    halPtaReq_t txReqL = txReq; // Local non-volatile flavor avoids warnings
    halPtaReq_t rxReqL = rxReq; // Local non-volatile flavor avoids warnings
    bool myReq = !!((txReqL | rxReqL) & PTA_REQ_ON);     // I need to REQUEST
    bool force = !!((txReqL | rxReqL) & PTA_REQ_FORCE);  // (ignoring others)
    bool exReq;                                          // external requestor?
    if (ptaReqGpioOutAsserted()) {  // in GRANT phase
      exReq = false;                // ignore external requestors
    } else {                        // in REQUEST phase
      ptaReqGpioIntAcknowledge();   // Before sampling REQUEST, avoids race
      exReq = ptaReqGpioCfgIsShared() && ptaReqGpioInAsserted();
    }
    if (myReq) {                    // want to assert REQUEST
      if (force || !exReq) {        // can assert REQUEST
        ptaGntGpioIntAcknowledge();
        ptaGntGpioIntEnable();
        ptaReqGpioSet(true);
        ptaPriGpioSet(!!((txReqL | rxReqL) & PTA_REQ_HIPRI));

        // Issue callbacks on REQUEST assertion
        // These are one-shot callbacks
        if ((rxCb != NULL) && (rxReq & PTA_REQCB_REQUESTED)) {
          (*rxCb)(PTA_REQCB_REQUESTED);
          rxReq &= ~PTA_REQCB_REQUESTED;
        }
        if ((txCb != NULL) && (txReq & PTA_REQCB_REQUESTED)) {
          (*txCb)(PTA_REQCB_REQUESTED);
          txReq &= ~PTA_REQCB_REQUESTED;
        }
        ptaGntGpioIntPend(); // Manually force GRANT check if missed/no edge
      } else {                      // must wait for REQUEST
        ptaReqGpioIntEnable();
      }
    } else {                        // negate REQUEST
      ptaPriGpioSet(false);
      ptaReqGpioSet(false);
      ptaGntGpioIntDisable();
      ptaReqGpioIntDisable();
      halInternalPtaOrRhoNotifyRadio(); // Reassess (assert) RHO
    }
  }

 #ifdef  PTA_GNT_GPIO
  // Triggered on both GRANT edges
  static void PTA_GNT_ISR(uint8_t pin)
  {
    ptaGntGpioIntAcknowledge();
    if (ptaReqGpioOutAsserted()) {  // GRANT phase
      bool newGnt = ptaGntGpioInAsserted(); // Sample GPIO once, now
      if (newGnt != gntWasAsserted) {
        gntWasAsserted = newGnt;
        halInternalPtaOrRhoNotifyRadio();
        // Issue callbacks on GRANT assert or negate
        // These are not one-shot callbacks
        halPtaReq_t newState = (newGnt ? PTA_REQCB_GRANTED : PTA_REQCB_NEGATED);
        if ((rxCb != NULL) && (rxReq & newState)) {
          (*rxCb)(newState);
        }
        if ((txCb != NULL) && (txReq & newState)) {
          (*txCb)(newState);
        }
        // Do we need this to meet GRANT -> REQUEST timing?
        // On GNT deassertion, pulse REQUEST to keep us going.
        // Don't want to revert to REQUEST phase here but stay in GRANT phase.
        // This seems dangerous in that it could allow a peer to assert their
        // REQUEST causing a conflict/race.

        if (!newGnt) {
          // If grant is lost mid transmit,
          // cancel request if we are transmitting
          // or if ack hold off is enabled and we are receiving
          if ((halPtaOptions & PTA_OPT_ABORT_TX)
             && ((txReq != PTA_REQ_OFF)
             || ((halPtaGetOptions() & PTA_OPT_ACK_HOLDOFF)
             && (rxReq != PTA_REQ_OFF)))) {
            emPhyCancelTransmit();
          }
       #ifdef ENABLE_PTA_REQ_PULSE
          ptaReqGpioSet(false);
          ptaReqGpioSet(true);
       #endif
        }
      }
    } else {                        // REQUEST phase
     #if     (ptaReqAndGntIrqShared())
      if (ptaReqGpioCfgIsShared()) {
        // External REQUEST deasserted so we can assert ours
        ptaReqGpioIntDisable(); // This is a one-shot event
        //TODO: Perform some random backoff before claiming REQUEST??
        ptaReqRandomBackoff();
        ptaUpdateReqIsr();
      } else {
        // Ignore GRANT changes unless we are REQUESTing
      }
     #endif//(ptaReqAndGntIrqShared())
    }
  }
  // Certain radios may want to override this with their own
  WEAK(void emPhyCancelTransmit(void))
  {
  }
 #endif//PTA_GNT_GPIO

 #if     ( defined(PTA_REQ_GPIO) && (!ptaReqAndGntIrqShared()) )
  // This IRQ is triggered on the negate REQUEST edge,
  // needed only when REQUEST signal is shared,
  // and not piggybacking GNT and REQ on same IRQ.
  static void PTA_REQ_ISR(uint8_t pin)
  {
    // External REQUEST deasserted so we can assert ours
    //ptaReqGpioIntAcknowledge(); // Covered within ptaReqGpioIntDisable()
    ptaReqGpioIntDisable(); // This is a one-shot event
    //TODO: Perform some random backoff before claiming REQUEST??
    ptaReqRandomBackoff();
    ptaUpdateReqIsr();
  }
 #endif//( defined(PTA_REQ_GPIO) && (!ptaReqAndGntIrqShared()) )

  // Public API

  EmberStatus halPtaSetTxRequest(halPtaReq_t ptaReq, halPtaCb_t ptaCb)
  {
    EmberStatus status = EMBER_ERR_FATAL;
    ATOMIC(
      if (ptaEnabled) {
        // Signal old OFF callback when unrequesting
        // in case PTA is disabled whilst in the midst of a request,
        // so the requestor's state machine doesn't lock up.
        if ( (ptaReq == PTA_REQ_OFF)
           &&(txReq != PTA_REQ_OFF)
           &&(txCb != NULL)
           &&(txReq & PTA_REQCB_OFF) ) {
            (*txCb)(PTA_REQCB_OFF);
        }
        txCb  = ptaCb;
        if (txReq == ptaReq) {
          // Save a little time if redundant request
        } else {
          txReq = ptaReq;
          ptaUpdateReqIsr();
        }
        status = EMBER_SUCCESS;
      }
    )//ATOMIC
    return status;
  }

  EmberStatus halPtaSetRxRequest(halPtaReq_t ptaReq, halPtaCb_t ptaCb)
  {
    EmberStatus status = EMBER_ERR_FATAL;
    ATOMIC(
      if (ptaEnabled) {
        // Signal old OFF callback when unrequesting
        // in case PTA is disabled whilst in the midst of a request,
        // so the requestor's state machine doesn't lock up.
        if ( (ptaReq == PTA_REQ_OFF)
           &&(rxReq != PTA_REQ_OFF)
           &&(rxCb != NULL)
           &&(rxReq & PTA_REQCB_OFF) ) {
            (*rxCb)(PTA_REQCB_OFF);
        }
        rxCb  = ptaCb;
        if (rxReq == ptaReq) {
          // Save a little time if redundant request
        } else {
          rxReq = ptaReq;
          ptaUpdateReqIsr();
        }
        status = EMBER_SUCCESS;
      }
    )//ATOMIC
    return status;
  }

  // Certain radios may want to override this with their own
  WEAK(void emRadioEnablePta(bool enable))
  {
  }

  EmberStatus halPtaSetEnable(bool enabled)
  {
    if (enabled != ptaEnabled) {
      if (enabled) {
        // Safely turn on GPIO interrupts
        GPIOINT_InitSafe();

        ptaReqGpioCfg();
        ptaPriGpioCfg();
        ptaGntGpioCfg();
      } else {
        // Shut any pending PTA operation down
        (void) halPtaSetRxRequest(PTA_REQ_OFF, NULL);
        (void) halPtaSetTxRequest(PTA_REQ_OFF, NULL);
      }
      ptaEnabled = enabled;
      // Inform the radio in case PTA requires radio state manipulations
      emRadioEnablePta(enabled);
      halInternalPtaOrRhoNotifyRadio(); //Notify Radio land of new config
  #ifdef PTA_OPT_PTA_ENABLED
      SET_BIT_MASK(halPtaOptions, PTA_OPT_PTA_ENABLED, ptaEnabled);
  #endif// PTA_OPT_PTA_ENABLED
    }
    return EMBER_SUCCESS;
  }

  bool halPtaIsEnabled(void)
  {
    return ptaEnabled;
  }

#ifdef  RHO_GPIO // BOARD_HEADER supports Radio HoldOff

#define RHO_ENABLED_MASK  0x01u // RHO is enabled
#define RHO_RADIO_ON_MASK 0x02u // Radio is on (not sleeping)
static uint8_t rhoState;
static bool rhoAsserted = !!RHO_ASSERTED;

bool halGetRadioHoldOff(void)
{
  return (!!(rhoState & RHO_ENABLED_MASK));
}

// Return active state of Radio HoldOff GPIO pin
static bool halInternalRhoPinIsActive(void)
{
  return ( (rhoState & RHO_ENABLED_MASK)
         &&(((bool)halGpioRead(RHO_GPIO)) == rhoAsserted) );
}

#if defined(PTA_REQ_PULSE_ON_RHO_DEASSERT) && defined(PTA_GNT_GPIO) && defined(PTA_REQ_GPIO)
static void halInternalTogglePtaReq()
{
  if( ptaEnabled                      // PTA is enabled and
      && ptaReqGpioOutAsserted()      // REQUESTing and
      && !halInternalRhoPinIsActive() // RHO not asserted
      && !ptaGntGpioInAsserted()) {   // GRANT not asserted
    ptaReqGpioSet(false);
    ptaReqGpioSet(true);
 }
}
#else //defined(PTA_REQ_PULSE_ON_RHO_DEASSERT) && defined(PTA_GNT_GPIO) && defined(PTA_REQ_GPIO)
#define halInternalTogglePtaReq()
#endif //defined(PTA_REQ_PULSE_ON_RHO_DEASSERT) && defined(PTA_GNT_GPIO) && defined(PTA_REQ_GPIO)

static void RHO_ISR(uint8_t pin)
{
  // Ack interrupt before reading GPIO to avoid potential of missing int
  GPIO_IntClear(GPIO_FLAG(RHO_GPIO));
  if (rhoState & RHO_ENABLED_MASK) {
    // Notify Radio land of state change
    halInternalTogglePtaReq();
    halInternalPtaOrRhoNotifyRadio();
  }
}

EmberStatus halSetRadioHoldOff(bool enabled)
{
  // If enabling afresh or disabling after having been enabled
  // restart from a fresh state just in case.
  if (enabled || (rhoState & RHO_ENABLED_MASK)) {
    // Disable RHO interrupt & callback
    GPIO_IntConfig(GPIO_PORT(RHO_GPIO), GPIO_PIN(RHO_GPIO),
                   false, false, false);
    GPIOINT_CallbackUnRegister(GPIO_PIN(RHO_GPIO));
  }

  rhoState = (rhoState & ~RHO_ENABLED_MASK) | (enabled ? RHO_ENABLED_MASK : 0);

  if (enabled) {
    // Safely turn on GPIO interrupts
    GPIOINT_InitSafe();

    // Only configure GPIO if it was not set up prior
    if (halGpioGetConfig(RHO_GPIO) == gpioModeDisabled) {
      // Configure GPIO as input and if pulling, pull it toward deasserted state
      GPIO_PinModeSet(GPIO_PORT(RHO_GPIO), GPIO_PIN(RHO_GPIO),
                      RHO_GPIOCFG, !RHO_ASSERTED);
    }
    // Here we sense asserted state is opposite of its current output state.
    rhoAsserted = !halGpioReadOutput(RHO_GPIO);
    // Register callbacks before setting up and enabling pin interrupt.
    GPIOINT_CallbackRegister(GPIO_PIN(RHO_GPIO), RHO_ISR);
    // Set rising and falling edge interrupts; don't enable interrupt yet
    GPIO_IntConfig(GPIO_PORT(RHO_GPIO), GPIO_PIN(RHO_GPIO),
                   true, true, false);

    // Only update radio if it's on, otherwise defer to when it gets turned on
    if (rhoState & RHO_RADIO_ON_MASK) {
      halInternalPtaOrRhoNotifyRadio(); //Notify Radio land of current state
      GPIO_IntEnable(GPIO_FLAG(RHO_GPIO));
      // Interrupt on now, ISR will maintain proper state
    }
  } else {
    halInternalPtaOrRhoNotifyRadio(); //Notify Radio land of configured state
    // Leave interrupt state untouched (probably turned off above)
  }
  SET_BIT_MASK(halPtaOptions, PTA_OPT_RHO_ENABLED, rhoState & RHO_ENABLED_MASK);
  return EMBER_SUCCESS;
}

static void halStackRadioHoldOffPowerDown(void)
{
  rhoState &= ~RHO_RADIO_ON_MASK;
  if (rhoState & RHO_ENABLED_MASK) {
    // When sleeping radio, no need to monitor RHO anymore
    GPIO_IntDisable(GPIO_FLAG(RHO_GPIO)); //clear RHO top level int enable
  }
}

static void halStackRadioHoldOffPowerUp(void)
{
  rhoState |= RHO_RADIO_ON_MASK;
  if (rhoState & RHO_ENABLED_MASK) {
    // When waking radio, set up initial state and resume monitoring
    GPIO_IntDisable(GPIO_FLAG(RHO_GPIO)); //ensure RHO interrupt is off
    RHO_ISR(0); // Manually call ISR to assess current state
    GPIO_IntEnable(GPIO_FLAG(RHO_GPIO)); //enable RHO interrupt
  }
}

#else//!RHO_GPIO

// Stubs in case someone insists on referencing them or for PTA w/o RHO

bool halGetRadioHoldOff(void)
{
  return false;
}

EmberStatus halSetRadioHoldOff(bool enabled)
{
  return (enabled ? EMBER_BAD_ARGUMENT : EMBER_SUCCESS);
}

static void halStackRadioHoldOffPowerDown(void)
{
}

static void halStackRadioHoldOffPowerUp(void)
{
  halInternalPtaOrRhoNotifyRadio();
}

#define halInternalRhoPinIsActive() (false)

#endif//RHO_GPIO // Board header supports Radio HoldOff

extern void emRadioHoldOffIsr(bool active);

static void halInternalPtaOrRhoNotifyRadio(void)
{
  bool ptaRho = ( ptaEnabled                       // PTA is enabled and:
                &&( (!ptaReqGpioOutAsserted())     // not REQUESTing or
                  ||(!ptaGntGpioInAsserted()) ) ); // REQUEST not GRANTed
  emRadioHoldOffIsr(ptaRho || halInternalRhoPinIsActive());
}
