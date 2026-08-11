/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/ibkr_boundary.h
 *
 * PURPOSE:
 *   Validate Interactive Brokers connection settings without exposing vendor SDK types.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract isolates ibkr boundary behaviour so Studio, Trader, TMS, tests and broker adapters can reuse the same rule.
 */

#ifndef INCLUDE_UMICOM_TRADING_IBKR_BOUNDARY_H
#define INCLUDE_UMICOM_TRADING_IBKR_BOUNDARY_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/trading/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiIbkrConnectionSettings { char host[64]; uint16_t port; int32_t client_id; UmiTradingEnvironment environment; } UmiIbkrConnectionSettings;
int umi_ibkr_settings_valid(const UmiIbkrConnectionSettings *settings);
#ifdef __cplusplus
}
#endif
#endif
