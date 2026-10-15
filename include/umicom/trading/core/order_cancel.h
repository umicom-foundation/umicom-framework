/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/order_cancel.h
 *
 * PURPOSE:
 *   Describe a cancellable order request with version control and reason code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_ORDER_CANCEL_H
#define UMICOM_TRADING_CORE_ORDER_CANCEL_H
#include "umicom/trading/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading order cancel data shared with callers of this public contract.
 */
typedef struct UmiTradingOrderCancel { UmiFinancialId client_order_id; uint64_t expected_version; uint32_t reason_code; } UmiTradingOrderCancel;
/* Initialise and validate describe a cancellable order request with version control and reason code. */
UmiStatus umi_trading_order_cancel_init(UmiTradingOrderCancel *value,const UmiFinancialId * client_order_id, uint64_t expected_version, uint32_t reason_code);
/* Validate the invariant set for this trading record. */
bool umi_trading_order_cancel_valid(const UmiTradingOrderCancel *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_trading_order_cancel_archive_encode(const UmiTradingOrderCancel *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_trading_order_cancel_archive_decode(const void *bytes, size_t byte_count,
    UmiTradingOrderCancel *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_trading_order_cancel_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTradingOrderCancel *expected, UmiTradingOrderCancel *value);


#ifdef __cplusplus
}
#endif
#endif
