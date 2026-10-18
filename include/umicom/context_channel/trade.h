/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/trade.h
 *
 * PURPOSE:
 *   Define the canonical trade context shared by applications and panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_TRADE_H
#define UMICOM_CONTEXT_CHANNEL_TRADE_H
#include "umicom/context_channel/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the trade context data shared with callers of this public contract.
 */
typedef struct UmiTradeContext {
    uint32_t structure_size;
    char trade_id[UMI_CONTEXT_TEXT_CAPACITY];
    char source_system[64U];
    char product_type[96U];
    char book_id[UMI_CONTEXT_TEXT_CAPACITY];
    char counterparty_id[UMI_CONTEXT_TEXT_CAPACITY];
    uint64_t version;
    uint64_t revision;
} UmiTradeContext;
/**
 * Initialise trade context from caller-provided values so later operations receive a known
 * state.
 */
void umi_trade_context_init(UmiTradeContext *context);
/**
 * Check that trade context satisfies its contract before another service relies on it.
 */
UmiStatus umi_trade_context_validate(const UmiTradeContext *context);
/**
 * Copy trade context into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_trade_context_copy(UmiTradeContext *destination, const UmiTradeContext *source);
/**
 * Provide the trade context set trade id operation used by this module and its client
 * applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_trade_context_set_trade_id(UmiTradeContext *context, const char *value);
/**
 * Provide the trade context set source system operation used by this module and its client
 * applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_trade_context_set_source_system(UmiTradeContext *context, const char *value);
/**
 * Provide the trade context set product type operation used by this module and its client
 * applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_trade_context_set_product_type(UmiTradeContext *context, const char *value);
/**
 * Provide the trade context set book id operation used by this module and its client
 * applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_trade_context_set_book_id(UmiTradeContext *context, const char *value);
/**
 * Provide the trade context set counterparty id operation used by this module and its
 * client applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_trade_context_set_counterparty_id(UmiTradeContext *context, const char *value);
/**
 * Provide the trade context set version operation used by this module and its client
 * applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_trade_context_set_version(UmiTradeContext *context, uint64_t value);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_trade_context_archive_encode(const UmiTradeContext *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_trade_context_archive_decode(const void *bytes, size_t byte_count,
    UmiTradeContext *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiTradeContext object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_trade_context_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiTradeContext *workspace, UmiTradeContext *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_trade_context_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiTradeContext *expected, UmiTradeContext *value);


#ifdef __cplusplus
}
#endif
#endif
