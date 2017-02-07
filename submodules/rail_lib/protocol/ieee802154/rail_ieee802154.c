/*
 * @file rail_ieee802154.c
 * @brief Implements ieee802154 specific RAIL functions
 */

#include "rail_types.h"
#include "rail.h"
#include "rail_rf_hal.h"

#include "rail_ieee802154.h"

RAIL_Status_t RAIL_IEEE802154_Init(RAIL_IEEE802154_Config_t *config)
{
  RAIL_Status_t status = RAIL_STATUS_NO_ERROR;

  RFHAL_IEEE802154Enable();

  RAIL_IEEE802154_SetPanCoordinator(config->isPanCoordinator);
  RAIL_IEEE802154_SetPromiscuousMode(config->promiscuousMode);
  RAIL_IEEE802154_AcceptFrames(config->framesMask);

  RAIL_AutoAckConfig_t ackConfig;
  ackConfig.defaultState = config->defaultState;
  ackConfig.idleTiming = config->idleTime;
  ackConfig.turnaroundTime = config->turnaroundTime;
  ackConfig.ackTimeout = config->ackTimeout;
  status = RAIL_AutoAckConfig(&ackConfig);
  if (status == RAIL_STATUS_NO_ERROR)
  {
    status = RFHAL_IEEE802154LoadAck();
  }

  // RAIL Address Filter Configurations
  // Initialize RAIL address filter for 802.15.4
  RAIL_AddressFilterReset();
  uint8_t offsets[2] = { 0, 0 };
  uint8_t sizes[2] = { 2, 8 };
  // Set match table to 0, as its populated by the sequencer
  RAIL_AddrConfig_t initial = { 2, offsets, sizes, 0 };
  bool success = true;
  success &= RAIL_AddressFilterConfig(&initial);
  success &= RFHAL_IEEE802154SetBroadcastAddresses();
  if (!success)
  {
    status = RAIL_STATUS_INVALID_PARAMETER;
  }
  // These functions can return false even if configuration succeeds
  RAIL_AddressFilterEnable();
  RAIL_IEEE802154_SetAddresses(config->addresses);

  return status;
}

static const RAIL_ChannelConfigEntry_t entry[] = {
  {11, 26, 5000000, 2405000000},
};
static const RAIL_ChannelConfig_t channels = {
  (RAIL_ChannelConfigEntry_t *) entry,
  1
};

RAIL_Status_t RAIL_IEEE802154_2p4GHzRadioConfig(void)
{
  RAIL_ChannelConfig(&channels);
  return RFHAL_IEEE802154RadioConfig2p4GHz();
}

RAIL_Status_t RAIL_IEEE802154_Deinit(void)
{
  if (RAIL_RfStateGet() != RAIL_RF_STATE_IDLE)
  {
    return RAIL_STATUS_INVALID_STATE;
  }

  RAIL_AddressFilterDisable();
  RAIL_AddressFilterReset();
  RAIL_AutoAckDisable();
  // Default timings are all 100 us
  RAIL_StateTiming_t timing = { 100, 100, 100, 100 };
  RAIL_SetStateTiming(&timing);
  RFHAL_IEEE802154Disable();
  return RAIL_STATUS_NO_ERROR;
}

bool RAIL_IEEE802154_IsEnabled(void)
{
  return RFHAL_IEEE802154IsEnabled();
}

bool RAIL_IEEE802154_SetAddresses(RAIL_IEEE802154_AddrConfig_t *addresses)
{
  if (addresses == NULL)
  {
    return false;
  }
  bool success = RAIL_IEEE802154_SetPanId(addresses->panId);
  success |= RAIL_IEEE802154_SetShortAddress(addresses->shortAddr);
  return (success | RAIL_IEEE802154_SetLongAddress(addresses->longAddr));
}

bool RAIL_IEEE802154_SetPanId(uint16_t panId)
{
  return RFHAL_IEEE802154SetPanId(panId);
}

bool RAIL_IEEE802154_SetShortAddress(uint16_t shortAddr)
{
  return RFHAL_IEEE802154SetShortAddress(shortAddr);
}

bool RAIL_IEEE802154_SetLongAddress(uint8_t *longAddr)
{
  return RFHAL_IEEE802154SetLongAddress(longAddr);
}

RAIL_Status_t RAIL_IEEE802154_SetPanCoordinator(bool isPanCoordinator)
{
  if (!RAIL_IEEE802154_IsEnabled())
  {
    return RAIL_STATUS_INVALID_STATE;
  }
  return RFHAL_IEEE802154SetPanCoordinator(isPanCoordinator);
}

RAIL_Status_t RAIL_IEEE802154_SetPromiscuousMode(bool enable)
{
  if (!RAIL_IEEE802154_IsEnabled())
  {
    return RAIL_STATUS_INVALID_STATE;
  }
  return RFHAL_IEEE802154SetPromiscuousMode(enable);
}

RAIL_Status_t RAIL_IEEE802154_AcceptFrames(uint8_t framesMask)
{
  if (!RAIL_IEEE802154_IsEnabled())
  {
    return RAIL_STATUS_INVALID_STATE;
  }
  return RFHAL_IEEE802154AcceptFrames(framesMask);
}

RAIL_Status_t RAIL_IEEE802154_SetFramePending(void)
{
  return RFHAL_IEEE802154SetFramePending();
}
