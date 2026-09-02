/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/trading_profile.h
 *
 * PURPOSE:
 *   Describe canonical Trader profile capability names.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract isolates trading profile behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_TRADING_PROFILE_H
#define INCLUDE_UMICOM_TRADING_TRADING_PROFILE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Return the number of records represented by trading profile capability without changing
 * their state.
 */
size_t umi_trading_profile_capability_count(void);
/**
 * Find trading profile capability while leaving the underlying catalogue or model owned by
 * this module.
 */
const char *umi_trading_profile_capability_at(size_t index);
#ifdef __cplusplus
}
#endif
#endif
