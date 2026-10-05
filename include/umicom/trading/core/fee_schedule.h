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
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading fee schedule data shared with callers of this public contract.
 */
typedef struct UmiTradingFeeSchedule { int64_t maker_minor_per_lot; int64_t taker_minor_per_lot; int64_t regulatory_minor_per_lot; } UmiTradingFeeSchedule;
/* Initialise and validate define maker/taker exchange fees in minor units per lot. */
UmiStatus umi_trading_fee_schedule_init(UmiTradingFeeSchedule *value,int64_t maker_minor_per_lot, int64_t taker_minor_per_lot, int64_t regulatory_minor_per_lot);
/* Validate the invariant set for this trading record. */
bool umi_trading_fee_schedule_valid(const UmiTradingFeeSchedule *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_trading_fee_schedule_archive_encode(const UmiTradingFeeSchedule *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_trading_fee_schedule_archive_decode(const void *bytes, size_t byte_count,
    UmiTradingFeeSchedule *value);

#ifdef __cplusplus
}
#endif
#endif
