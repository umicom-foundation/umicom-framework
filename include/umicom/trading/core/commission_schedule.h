/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/commission_schedule.h
 *
 * PURPOSE:
 *   Define per-lot and minimum brokerage commission in integer minor units.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_COMMISSION_SCHEDULE_H
#define UMICOM_TRADING_CORE_COMMISSION_SCHEDULE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTradingCommissionSchedule { int64_t per_lot_minor; int64_t minimum_minor; int64_t maximum_minor; } UmiTradingCommissionSchedule;
/* Initialise and validate define per-lot and minimum brokerage commission in integer minor units. */
UmiStatus umi_trading_commission_schedule_init(UmiTradingCommissionSchedule *value,int64_t per_lot_minor, int64_t minimum_minor, int64_t maximum_minor);
/* Validate the invariant set for this trading record. */
bool umi_trading_commission_schedule_valid(const UmiTradingCommissionSchedule *value);
#ifdef __cplusplus
}
#endif
#endif
