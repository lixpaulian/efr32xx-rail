/** @file hal/micro/pta.h
 * @brief Packet traffic arbitration interface.
 * See @ref micro for documentation.
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.                        *80*-->
 */

/** @addtogroup micro
 * See also hal/micro/pta.h for source code.
 *@{
 */

#ifndef __PTA_H__
#define __PTA_H__

// Include AppBuilder generated header
#if   defined(ZA_GENERATED_HEADER)
  #include ZA_GENERATED_HEADER  // AFV2
#endif

// Let AppBuilder contribute coexistence configurations.
#ifdef EMBER_AF_API_COEXISTENCE
  #include EMBER_AF_API_COEXISTENCE
#endif

/** @name Radio HoldOff Configuration Definitions
 *
 * The following are used to aid in the abstraction with Radio
 * HoldOff (RHO).  The microcontroller-specific sources use these
 * definitions so they are able to work across a variety of boards
 * which could have different connections.  The names and ports/pins
 * used below are intended to match with a schematic of the system to
 * provide the abstraction.
 *
 * The Radio HoldOff input GPIO is abstracted like BUTTON0/1.
 */
//@{
/**
 * @brief The GPIO configuration register for Radio HoldOff.
 */
#define RHO_GPIOCFG gpioModeInputPull

/** @brief The following definitions are helpers for managing
 *  Radio HoldOff and should not be modified.
 */
#if     (defined(RADIO_HOLDOFF) && defined(RHO_GPIO))
  // Initial bootup configuration is for Radio HoldOff
  #define halInternalInitRadioHoldOff() halSetRadioHoldOff(true)
#else//!(defined(RADIO_HOLDOFF) && defined(RHO_GPIO))
  // Initial bootup configuration is for default
  #define halInternalInitRadioHoldOff() /* no-op */
#endif//(defined(RADIO_HOLDOFF) && defined(RHO_GPIO))

//@} //END OF RADIO HOLDOFF CONFIGURATION DEFINITIONS

/** @name PTA Configuration Definitions
 *
 * The following are used to aid in the abstraction with Packet
 * Traffic Arbitration (PTA).  The microcontroller-specific sources
 * use these definitions so they are able to work across a variety of
 * boards which could have different connections.  The names and
 * ports/pins used below are intended to match with a schematic of the
 * system to provide the abstraction.
 *
 * PTA 2-wire (Request out/shared, Grant in) or 3-wire (Request out/shared,
 * Priority out, and Grant in) configurations are supported.
 */
//@{

#ifdef ENABLE_PTA_OPT_ACK_HOLDOFF
  #define DEFAULT_PTA_OPT_ACK_HOLDOFF PTA_OPT_ACK_HOLDOFF
#else //!ENABLE_PTA_OPT_ACK_HOLDOFF
  #define DEFAULT_PTA_OPT_ACK_HOLDOFF PTA_OPT_DISABLED
#endif //ENABLE_PTA_OPT_ACK_HOLDOFF

#ifdef ENABLE_PTA_OPT_ABORT_TX
  #define DEFAULT_PTA_OPT_ABORT_TX PTA_OPT_ABORT_TX
#else //!ENABLE_PTA_OPT_ABORT_TX
  #define DEFAULT_PTA_OPT_ABORT_TX PTA_OPT_DISABLED
#endif //ENABLE_PTA_OPT_ABORT_TX

#ifdef ENABLE_PTA_OPT_TX_HI_PRI
  #define DEFAULT_PTA_OPT_TX_HIPRI PTA_OPT_TX_HIPRI
#else //!ENABLE_PTA_OPT_TX_HIPRI
  #define DEFAULT_PTA_OPT_TX_HIPRI PTA_OPT_DISABLED
#endif //ENABLE_PTA_OPT_TX_HIPRI

#ifdef ENABLE_PTA_OPT_RX_HI_PRI
  #define DEFAULT_PTA_OPT_RX_HIPRI PTA_OPT_RX_HIPRI
#else //!ENABLE_PTA_OPT_RX_HIPRI
  #define DEFAULT_PTA_OPT_RX_HIPRI PTA_OPT_DISABLED
#endif //ENABLE_PTA_OPT_RX_HIPRI

#ifdef ENABLE_PTA_OPT_RX_RETRY_HI_PRI
  #define DEFAULT_PTA_OPT_RX_RETRY_HIPRI PTA_OPT_RX_RETRY_HIPRI
#else //!ENABLE_PTA_OPT_RX_RETRY_HIPRI
  #define DEFAULT_PTA_OPT_RX_RETRY_HIPRI PTA_OPT_DISABLED
#endif //ENABLE_PTA_OPT_RX_RETRY_HIPRI

#ifdef ENABLE_PTA_OPT_LONG_REQ
  #define DEFAULT_PTA_OPT_LONG_REQ PTA_OPT_LONG_REQ
#else //!ENABLE_PTA_OPT_LONG_REQ
  #define DEFAULT_PTA_OPT_LONG_REQ PTA_OPT_DISABLED
#endif //ENABLE_PTA_OPT_LONG_REQ

#ifdef ENABLE_PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT
  #define DEFAULT_PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT
#else //!ENABLE_PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT
  #define DEFAULT_PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT PTA_OPT_DISABLED
#endif //ENABLE_PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT

#ifdef RADIO_HOLDOFF
  #define DEFAULT_PTA_OPT_RHO_ENABLED PTA_OPT_RHO_ENABLED
#else //!RADIO_HOLDOFF
  #define DEFAULT_PTA_OPT_RHO_ENABLED PTA_OPT_DISABLED
#endif //RADIO_HOLDOFF

#ifdef PTA_RX_RETRY_TIMEOUT_MS
  #define DEFAULT_PTA_OPT_RX_RETRY_TIMEOUT_MS PTA_RX_RETRY_TIMEOUT_MS
#else //!PTA_RX_RETRY_TIMEOUT_MS
  #define DEFAULT_PTA_OPT_RX_RETRY_TIMEOUT_MS PTA_OPT_DISABLED
#endif //PTA_RX_RETRY_TIMEOUT_MS

#ifdef PTA_RX_RETRY_REQ
  #define DEFAULT_PTA_OPT_RX_RETRY_REQ PTA_OPT_RX_RETRY_REQ
#else //!PTA_RX_RETRY_REQ
  #define DEFAULT_PTA_OPT_RX_RETRY_REQ PTA_OPT_DISABLED
#endif //PTA_RX_RETRY_TIMEOUT_MS

#define DEFAULT_PTA_OPTIONS ( 0                 \
  | DEFAULT_PTA_OPT_RX_RETRY_TIMEOUT_MS         \
  | DEFAULT_PTA_OPT_ACK_HOLDOFF                 \
  | DEFAULT_PTA_OPT_ABORT_TX                    \
  | DEFAULT_PTA_OPT_TX_HIPRI                    \
  | DEFAULT_PTA_OPT_RX_HIPRI                    \
  | DEFAULT_PTA_OPT_RX_RETRY_HIPRI              \
  | DEFAULT_PTA_OPT_RX_RETRY_REQ                \
  | DEFAULT_PTA_OPT_RHO_ENABLED                 \
  | DEFAULT_PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT \
  | DEFAULT_PTA_OPT_LONG_REQ                    \
  )

#ifdef PTA_REQ_GPIO
  #define PUBLIC_PTA_OPT_RX_RETRY_TIMEOUT_MS         \
          PTA_OPT_RX_RETRY_TIMEOUT_MS
  #define PUBLIC_PTA_OPT_RX_RETRY_REQ                \
          PTA_OPT_RX_RETRY_REQ
  #define PUBLIC_PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT \
          PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT
#else //!PTA_REQ_GPIO
  #define PUBLIC_PTA_OPT_RX_RETRY_TIMEOUT_MS           \
          PTA_OPT_DISABLED
  #define PUBLIC_PTA_OPT_RX_RETRY_REQ                  \
          PTA_OPT_DISABLED
  #define PUBLIC_PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT   \
          PTA_OPT_DISABLED
#endif

#ifdef PTA_PRI_GPIO
  #define PUBLIC_PTA_OPT_TX_HIPRI       PTA_OPT_TX_HIPRI
  #define PUBLIC_PTA_OPT_RX_HIPRI       PTA_OPT_RX_HIPRI
  #define PUBLIC_PTA_OPT_RX_RETRY_HIPRI PTA_OPT_RX_RETRY_HIPRI
#else //!PTA_PRI_GPIO
  #define PUBLIC_PTA_OPT_TX_HIPRI       PTA_OPT_DISABLED
  #define PUBLIC_PTA_OPT_RX_HIPRI       PTA_OPT_DISABLED
  #define PUBLIC_PTA_OPT_RX_RETRY_HIPRI PTA_OPT_DISABLED
#endif

#ifdef RHO_GPIO
  #define PUBLIC_PTA_OPT_RHO_ENABLED PTA_OPT_RHO_ENABLED
#else //!RHO_GPIO
  #define PUBLIC_PTA_OPT_RHO_ENABLED PTA_OPT_DISABLED
#endif

#define PUBLIC_PTA_OPT_ACK_HOLDOFF PTA_OPT_ACK_HOLDOFF
#define PUBLIC_PTA_OPT_ABORT_TX    PTA_OPT_ABORT_TX

// Public PTA options can be modified using public PTA APIs
#define PUBLIC_PTA_OPTIONS (                   \
    PUBLIC_PTA_OPT_RX_RETRY_TIMEOUT_MS         \
  | PUBLIC_PTA_OPT_ACK_HOLDOFF                 \
  | PUBLIC_PTA_OPT_ABORT_TX                    \
  | PUBLIC_PTA_OPT_TX_HIPRI                    \
  | PUBLIC_PTA_OPT_RX_HIPRI                    \
  | PUBLIC_PTA_OPT_RX_RETRY_HIPRI              \
  | PUBLIC_PTA_OPT_RX_RETRY_REQ                \
  | PUBLIC_PTA_OPT_RHO_ENABLED                 \
  | PUBLIC_PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT \
  )
// Constant PTA options can not be modified using public PTA APIs
#define CONST_PTA_OPTIONS (~(PUBLIC_PTA_OPTIONS))

// PTA REQUEST signal (OUT or OUT_DO when shared with other radios):
#define PTA_REQ_GPIOCFG_NORMAL gpioModePushPull
#if (PTA_REQ_ASSERTED == 1)
#define PTA_REQ_GPIOCFG_SHARED gpioModeWiredOr
#else //(PTA_REQ_ASSERTED == 1)
#define PTA_REQ_GPIOCFG_SHARED gpioModeWiredAnd
#endif

#ifdef PTA_REQ_SHARED
#define PTA_REQ_GPIOCFG    PTA_REQ_GPIOCFG_SHARED
#else //!PTA_REQ_SHARED
#define PTA_REQ_GPIOCFG    PTA_REQ_GPIOCFG_NORMAL
#endif //PTA_REQ_SHARED

//Define ENABLE_PTA_REQ_PULSE to pulse request when grant is lost
//#define ENABLE_PTA_REQ_PULSE
// PTA GRANT signal (IN): [optional]
#define PTA_GNT_GPIOCFG    gpioModeInputPull


// PTA PRIORITY signal (OUT): [optional]
#define PTA_PRI_GPIOCFG    gpioModePushPull // or gpioModeWiredOr if shared


#if     (defined(ENABLE_PTA) && (defined(PTA_REQ_GPIO) || defined(PTA_GNT_GPIO)))
  // Initial bootup configuration is to enable PTA
  #define halInternalInitPta() halPtaSetEnable(true)
#else//!(defined(ENABLE_PTA) && (defined(PTA_REQ_GPIO) || defined(PTA_GNT_GPIO)))
  // Initial bootup configuration is not to enable PTA
  #define halInternalInitPta() /* no-op */
#endif//(defined(ENABLE_PTA) && (defined(PTA_REQ_GPIO) || defined(PTA_GNT_GPIO)))

//@} //END OF PTA CONFIGURATION DEFINITIONS


  // halPta Public API:
  //
  // halPtaReq_t is a bitmask of features:
  //     7     6     5     4     3     2     1     0
  // +-----+-----+-----+-----+-----+-----+-----+-----+
  // |   0 | no  | no  |wait |wait |force|hipri| req |
  // |     | req |grant|grant| req |     |     |     |
  // +-----+-----+-----+-----+-----+-----+-----+-----+
  //
  // This mask is used for both requests and callbacks to
  // represent status.

  #define PTA_REQ_OFF         0          // Negate request
  #define PTA_REQ_ON          (1u << 0)  // Assert request
  #define PTA_REQ_HIPRI       (1u << 1)  // Request is hi-pri
  #define PTA_REQ_FORCE       (1u << 2)  // Force assertion immediately
  #define PTA_REQCB_REQUESTED (1u << 3)  // Callback when REQUEST asserted
  #define PTA_REQCB_GRANTED   (1u << 4)  // Callback when GRANT asserted
  #define PTA_REQCB_NEGATED   (1u << 5)  // Callback when GRANT negated
  #define PTA_REQCB_OFF       (1u << 6)  // Callback when REQUEST removed

  typedef uint8_t halPtaReq_t;

  typedef void (*halPtaCb_t)(halPtaReq_t ptaStatus);

  //
  // HalPtaOptions is a bitmask of features:
  //       7       6       5       4       3       2       1       0
  // +-------+-------+-------+-------+-------+-------+-------+-------+
  // |rxretry|rxretry|rxretry|rxretry|rxretry|rxretry|rxretry|rxretry|
  // |timeout|timeout|timeout|timeout|timeout|timeout|timeout|timeout|
  // +-------+-------+-------+-------+-------+-------+-------+-------+
  //
  //      15      14      13      12      11      10       9       8
  // +-------+-------+-------+-------+-------+-------+-------+-------+
  // |toggle |rho    |retry  |rxretry|rx     |tx     |abort  |ack    |
  // |req    |on     |req    |hipri  |hipri  |hipri  |tx     |holdoff|
  // +-------+-------+-------+-------+-------+-------+-------+-------+
  //
  //      23      22      21      20      19      18      17      16
  // +-------+-------+-------+-------+-------+-------+-------+-------+
  // |   0   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |
  // |       |       |       |       |       |       |       |       |
  // +-------+-------+-------+-------+-------+-------+-------+-------+
  //
  //      31      30      29      28      27      26      25      24
  // +-------+-------+-------+-------+-------+-------+-------+-------+
  // |long   |   0   |   0   |   0   |   0   |   0   |   0   |   0   |
  // |req    |       |       |       |       |       |       |       |
  // +-------+-------+-------+-------+-------+-------+-------+-------+
  // This mask is used for both requests and callbacks to
  // represent status.

  #define PTA_OPT_DISABLED         0           // Disable option
  #define PTA_OPT_RX_RETRY_TIMEOUT_MS (0xffu)// Rx retry request timeout
  #define PTA_OPT_ACK_HOLDOFF      (1u << 8)   // Enable ack radio holdoff
  #define PTA_OPT_ABORT_TX         (1u << 9)   // Abort mid TX if grant is lost
  #define PTA_OPT_TX_HIPRI         (1u << 10)  // Tx request is hi-pri
  #define PTA_OPT_RX_HIPRI         (1u << 11)  // Rx request is hi-pri
  #define PTA_OPT_RX_RETRY_HIPRI   (1u << 12)  // Rx retry request is hi-pri
  #define PTA_OPT_RX_RETRY_REQ     (1u << 13)  // Request on corrupt packet
  #define PTA_OPT_RHO_ENABLED      (1u << 14)  // Enable RHO
  #define PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT  \
                                   (1u << 15)  // Enable toggle on retransmit
  //  #define PTA_OPT_PTA_ENABLED      (1u << 16)  // Enable PTA
  #define PTA_OPT_LONG_REQ         (1u << 31)  // Hold request across CCA failures

  #define PTA_OPT_SHIFT_RX_RETRY_TIMEOUT_MS (0)
  #define PTA_OPT_MASK1                     (0xff00)
  #define PTA_OPT_SHIFT1                    (8)
  #define PTA_TX_PRI ((halPtaGetOptions() & PTA_OPT_TX_HIPRI) ? PTA_REQ_HIPRI : PTA_REQ_OFF)
  #define PTA_RX_PRI ((halPtaGetOptions() & PTA_OPT_RX_HIPRI) ? PTA_REQ_HIPRI : PTA_REQ_OFF)
  #define PTA_RX_RETRY_PRI ((halPtaGetOptions() & PTA_OPT_RX_RETRY_HIPRI) ? PTA_REQ_HIPRI : PTA_REQ_OFF)

  #define halPtaGetOptionMask(mask,shift)       ((halPtaGetOptions()&mask)>>shift)
  #define halPtaSetOptionMask(value,mask,shift) (halPtaSetOptions((halPtaGetOptions()&(~mask))|((((HalPtaOptions)value)<<shift)&mask)))

  typedef uint32_t HalPtaOptions;

  #ifdef PTA_SUPPORT
  // Release PTA request if long request not enabled or toggle request on macretransmit enabled
  #define  halPtaGetTxReqRelease() \
    (((PTA_OPT_LONG_REQ | PTA_OPT_TOGGLE_REQ_ON_MACRETRANSMIT) & halPtaGetOptions()) != PTA_OPT_LONG_REQ)
  HalPtaOptions halPtaGetOptions(void);

  EmberStatus halPtaSetOptions(HalPtaOptions options);

  EmberStatus halPtaSetBool(HalPtaOptions option, bool value);

  EmberStatus halPtaSetEnable(bool enabled);

  bool halPtaIsEnabled(void);

  EmberStatus halPtaSetTxRequest(halPtaReq_t ptaReq, halPtaCb_t ptaCb);

  EmberStatus halPtaSetRxRequest(halPtaReq_t ptaReq, halPtaCb_t ptaCb);

  #else //!PTA_SUPPORT
  // halPta Stub API:
  #define halPtaGetTxReqRelease()           (false)
  #define halPtaSetEnable(enabled)          (EMBER_ERR_FATAL)
  #define halPtaIsEnabled()                 (false)
  #define halPtaSetTxRequest(ptaReq, ptaCb) (EMBER_ERR_FATAL)
  #define halPtaSetRxRequest(ptaReq, ptaCb) (EMBER_ERR_FATAL)
  #define halPtaGetOptions(void)            (0)
  #define halPtaSetOptions(options)         (EMBER_ERR_FATAL)  
  #endif //PTA_SUPPORT
#endif //__PTA_H__
/**@} // END micro group
 */

