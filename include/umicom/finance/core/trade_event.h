/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/trade_event.h
 *
 * PURPOSE:
 *   Describe immutable trade lifecycle evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_TRADE_EVENT_H
#define UMICOM_FINANCE_CORE_TRADE_EVENT_H

#include "umicom/finance/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trade event data shared with callers of this public contract.
 */
typedef struct UmiTradeEvent { UmiFinancialId event_id; UmiFinancialId parent_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; UmiFinancialDate effective_date; uint32_t state; bool active; } UmiTradeEvent;
/* Initialize the typed financial record. */ UmiStatus umi_trade_event_init(UmiTradeEvent *item,const char *id,const char *name,const char *parent_id,UmiFinancialDate effective_date,uint32_t state);
/* Validate the typed financial record. */ bool umi_trade_event_is_valid(const UmiTradeEvent *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_trade_event_archive_encode(const UmiTradeEvent *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_trade_event_archive_decode(const void *bytes, size_t byte_count,
    UmiTradeEvent *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTradeEvent object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_trade_event_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTradeEvent *workspace, UmiTradeEvent *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_trade_event_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTradeEvent *expected, UmiTradeEvent *value);


#ifdef __cplusplus
}
#endif

#endif
