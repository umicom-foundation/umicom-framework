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
 */
UmiStatus umi_trade_context_set_trade_id(UmiTradeContext *context, const char *value);
/**
 * Provide the trade context set source system operation used by this module and its client
 * applications.
 */
UmiStatus umi_trade_context_set_source_system(UmiTradeContext *context, const char *value);
/**
 * Provide the trade context set product type operation used by this module and its client
 * applications.
 */
UmiStatus umi_trade_context_set_product_type(UmiTradeContext *context, const char *value);
/**
 * Provide the trade context set book id operation used by this module and its client
 * applications.
 */
UmiStatus umi_trade_context_set_book_id(UmiTradeContext *context, const char *value);
/**
 * Provide the trade context set counterparty id operation used by this module and its
 * client applications.
 */
UmiStatus umi_trade_context_set_counterparty_id(UmiTradeContext *context, const char *value);
/**
 * Provide the trade context set version operation used by this module and its client
 * applications.
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

#ifdef __cplusplus
}
#endif
#endif
