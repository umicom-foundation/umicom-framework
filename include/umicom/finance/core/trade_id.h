/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/trade_id.h
 *
 * PURPOSE:
 *   Provide a strongly typed trade id wrapper over the existing financial identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_TRADE_ID_H
#define UMICOM_FINANCE_CORE_TRADE_ID_H

#include "umicom/finance/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trade id data shared with callers of this public contract.
 */
typedef struct UmiTradeId { UmiFinancialId id; } UmiTradeId;
/* Assign identifier. */ UmiStatus umi_trade_id_set(UmiTradeId *id,const char *value);
/* Validate identifier. */ bool umi_trade_id_is_valid(const UmiTradeId *id);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_trade_id_archive_encode(const UmiTradeId *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_trade_id_archive_decode(const void *bytes, size_t byte_count,
    UmiTradeId *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTradeId object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_trade_id_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTradeId *workspace, UmiTradeId *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_trade_id_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTradeId *expected, UmiTradeId *value);


#ifdef __cplusplus
}
#endif

#endif
