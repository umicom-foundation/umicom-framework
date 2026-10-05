/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/core/quote.h
 *
 * PURPOSE:
 *   Represent timestamped market quotes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_CORE_QUOTE_H
#define UMICOM_FINANCE_CORE_QUOTE_H

#include "umicom/finance/core/price.h"
#include "umicom/base/value_archive.h"
#include "umicom/finance/core/market_data_key.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the quote data shared with callers of this public contract.
 */
typedef struct UmiQuote { UmiMarketDataKey key; UmiQuoteSide side; UmiFinancialPrice price; int64_t timestamp; } UmiQuote;
/* Initialize quote. */ UmiStatus umi_quote_init(UmiQuote *q,const UmiMarketDataKey *k,UmiQuoteSide side,UmiFinancialPrice p,int64_t ts);
/* Validate quote. */ bool umi_quote_is_valid(const UmiQuote *q);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_quote_archive_encode(const UmiQuote *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_quote_archive_decode(const void *bytes, size_t byte_count,
    UmiQuote *value);

#ifdef __cplusplus
}
#endif

#endif
