/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/pricing_result.h
 *
 * PURPOSE:
 *   Represent provider-neutral pricing results.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_PRICING_RESULT_H
#define UMICOM_FINANCE_CORE_PRICING_RESULT_H

#include "umicom/finance/core/business_date.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the pricing result data shared with callers of this public contract.
 */
typedef struct UmiPricingResult { UmiFinancialId id; UmiMoney amount; UmiFinancialDate date; uint32_t state; } UmiPricingResult;
/* Initialize monetary record. */ UmiStatus umi_pricing_result_init(UmiPricingResult *x,const char *id,UmiMoney amount,UmiFinancialDate date,uint32_t state);
/* Validate monetary record. */ bool umi_pricing_result_is_valid(const UmiPricingResult *x);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_pricing_result_archive_encode(const UmiPricingResult *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_pricing_result_archive_decode(const void *bytes, size_t byte_count,
    UmiPricingResult *value);

#ifdef __cplusplus
}
#endif

#endif
