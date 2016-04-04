//
// Copyright (c) 2015 Liviu Ionescu.
// This file is part of the xPacks project (https://xpacks.github.io).
// Modified for Silicon Labs EFR32FG family by LNP
//

#ifndef CMSIS_DEVICE_H_
#define CMSIS_DEVICE_H_

#include "em_device.h"

// The lines below are here because the CMSIS version of Silicon Labs is too old and may be removed
// at a later time; __set_BASEPRI_MAX is required by CMSIS++

/**
 \brief   Set Base Priority with condition
 \details Assigns the given value to the Base Priority register only if BASEPRI masking is disabled,
 or the new value increases the BASEPRI priority level.
 \param [in]    basePri  Base Priority value to set
 */
__attribute__((always_inline)) __STATIC_INLINE void __set_BASEPRI_MAX(uint32_t value)
{
    __ASM volatile ("MSR basepri_max, %0" : : "r" (value) : "memory");
}

#endif // CMSIS_DEVICE_H_
