/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/pricing_context.h
 *
 * PURPOSE:
 *   Define pricing context identity and valuation date.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_PRICING_CONTEXT_H
#define UMICOM_FINANCE_CORE_PRICING_CONTEXT_H

#include "umicom/finance/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the pricing context data shared with callers of this public contract.
 */
typedef struct UmiPricingContext { UmiFinancialId context_id; char name[UMI_FINANCIAL_CORE_NAME_CAPACITY]; char code[UMI_FINANCIAL_CORE_CODE_CAPACITY]; UmiFinancialDate effective_date; bool active; } UmiPricingContext;
/* Initialize the typed financial record. */ UmiStatus umi_pricing_context_init(UmiPricingContext *item,const char *id,const char *name,const char *code,UmiFinancialDate effective_date);
/* Validate the typed financial record. */ bool umi_pricing_context_is_valid(const UmiPricingContext *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_pricing_context_archive_encode(const UmiPricingContext *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_pricing_context_archive_decode(const void *bytes, size_t byte_count,
    UmiPricingContext *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_pricing_context_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiPricingContext *expected, UmiPricingContext *value);


#ifdef __cplusplus
}
#endif

#endif
