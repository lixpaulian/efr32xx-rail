/***************************************************************************//**
 * @file rail_ble.c
 * @brief Implementation of the RAIL BLE layer.
 * @copyright Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com
 ******************************************************************************/

#include "em_device.h"

#include "rail.h"
#include "rail_rf_hal.h"
#include "rail_ble.h"

// Defines for the BLE whitening configuration
#define BLE_WHITEPOLY                    (0x0044U)

// State globals for the RAIL BLE module
static bool isBleEnabled = false;

void RAIL_BLE_Init(void)
{
  RFHAL_BleInit();

  // @TODO: Should we load the BLE channel configuration here as well???

  // Track whether BLE mode is enabled
  isBleEnabled = true;

  RAIL_SetPtiProtocol(RAIL_PTI_PROTOCOL_BLE);
}

void RAIL_BLE_Deinit(void)
{
  // Only deinitialize BLE if it was actually initialized
  if (isBleEnabled)
  {
    RFHAL_BleDeinit();

    // Clear the BLE mode tracker
    isBleEnabled = false;
    RAIL_SetPtiProtocol(RAIL_PTI_PROTOCOL_CUSTOM);
  }
}

bool RAIL_BLE_IsEnabled(void)
{
  return isBleEnabled;
}

void RAIL_BLE_SetupChannelRadioParams(uint32_t crcInit,
                                      uint32_t accessAddress,
                                      uint8_t channel,
                                      bool disableWhitening)
{
  // Reverse and set the CRC initializtion value
  crcInit = __RBIT (crcInit);
  crcInit = (crcInit >> 8);
  crcInit &= 0x00FFFFFFUL;
  RFHAL_SetCRCInitVal(crcInit);

  // Set the access address
  RFHAL_BLESyncWordSet(accessAddress);

  // Configure the preamble to match the access address
  if (accessAddress & 0x01UL)
  {
    // If the LSB of the sync (first on air) is 1, set preamble to 0x55, which
    // will be transmitted LSB first and result in a 0b10101010 preamble on air.
    RFHAL_BLEPreambleSet(0x55);
  }
  else
  {
    // Else the LSB of the sync (first on air) is 0, set preamble to 0xAA, which
    // will be transmitted LSB first and result in a 0b01010101 preamble on air.
    RFHAL_BLEPreambleSet(0xAA);
  }

  // Configure the whitener
  if(!disableWhitening)
  {
    uint16_t whiteInit = (0x01UL << 6UL) | (uint32_t)channel;

    // Turn whitening on
    RFHAL_BLEWhiteningSet(BLE_WHITEPOLY, whiteInit, true);
  }
  else
  {
    // Turn whitening off
    RFHAL_BLEWhiteningSet(0, 0, false);
  }
}
