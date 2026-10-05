/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/price.h
 *
 * PURPOSE:
 *   Represent finite non-negative financial prices.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_PRICE_H
#define UMICOM_FINANCE_CORE_PRICE_H

#include "umicom/finance/core/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the financial price data shared with callers of this public contract.
 */
typedef struct UmiFinancialPrice { double value; uint8_t scale; } UmiFinancialPrice;
/* Initialize price. */ UmiStatus umi_price_init(UmiFinancialPrice *p,double value,uint8_t scale);
/* Validate price. */ bool umi_price_is_valid(const UmiFinancialPrice *p);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_price_archive_encode(const UmiFinancialPrice *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_price_archive_decode(const void *bytes, size_t byte_count,
    UmiFinancialPrice *value);

#ifdef __cplusplus
}
#endif

#endif
