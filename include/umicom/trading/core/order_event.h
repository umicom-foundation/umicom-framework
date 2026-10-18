/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/order_event.h
 *
 * PURPOSE:
 *   Capture sequence-ordered evidence for an order lifecycle transition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_ORDER_EVENT_H
#define UMICOM_TRADING_CORE_ORDER_EVENT_H
#include "umicom/trading/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trading order event data shared with callers of this public contract.
 */
typedef struct UmiTradingOrderEvent { UmiFinancialId client_order_id; uint64_t sequence; int64_t event_time_ms; UmiTradingCoreOrderState state; } UmiTradingOrderEvent;
/* Initialise and validate capture sequence-ordered evidence for an order lifecycle transition. */
UmiStatus umi_trading_order_event_init(UmiTradingOrderEvent *value,const UmiFinancialId * client_order_id, uint64_t sequence, int64_t event_time_ms, UmiTradingCoreOrderState state);
/* Validate the invariant set for this trading record. */
bool umi_trading_order_event_valid(const UmiTradingOrderEvent *value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_trading_order_event_archive_encode(const UmiTradingOrderEvent *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_trading_order_event_archive_decode(const void *bytes, size_t byte_count,
    UmiTradingOrderEvent *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTradingOrderEvent object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_trading_order_event_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTradingOrderEvent *workspace, UmiTradingOrderEvent *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_trading_order_event_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTradingOrderEvent *expected, UmiTradingOrderEvent *value);


#ifdef __cplusplus
}
#endif
#endif
