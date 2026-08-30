/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/fee_schedule.h
 *
 * PURPOSE:
 *   Define maker/taker exchange fees in minor units per lot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_FEE_SCHEDULE_H
#define UMICOM_TRADING_CORE_FEE_SCHEDULE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTradingFeeSchedule { int64_t maker_minor_per_lot; int64_t taker_minor_per_lot; int64_t regulatory_minor_per_lot; } UmiTradingFeeSchedule;
/* Initialise and validate define maker/taker exchange fees in minor units per lot. */
UmiStatus umi_trading_fee_schedule_init(UmiTradingFeeSchedule *value,int64_t maker_minor_per_lot, int64_t taker_minor_per_lot, int64_t regulatory_minor_per_lot);
/* Validate the invariant set for this trading record. */
bool umi_trading_fee_schedule_valid(const UmiTradingFeeSchedule *value);
#ifdef __cplusplus
}
#endif
#endif
