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
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading commission schedule data shared with callers of this public
 * contract.
 */
typedef struct UmiTradingCommissionSchedule { int64_t per_lot_minor; int64_t minimum_minor; int64_t maximum_minor; } UmiTradingCommissionSchedule;
/* Initialise and validate define per-lot and minimum brokerage commission in integer minor units. */
UmiStatus umi_trading_commission_schedule_init(UmiTradingCommissionSchedule *value,int64_t per_lot_minor, int64_t minimum_minor, int64_t maximum_minor);
/* Validate the invariant set for this trading record. */
bool umi_trading_commission_schedule_valid(const UmiTradingCommissionSchedule *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_trading_commission_schedule_archive_encode(const UmiTradingCommissionSchedule *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_trading_commission_schedule_archive_decode(const void *bytes, size_t byte_count,
    UmiTradingCommissionSchedule *value);

#ifdef __cplusplus
}
#endif
#endif
