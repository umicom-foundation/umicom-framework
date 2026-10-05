/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/quote.c
 *
 * PURPOSE:
 *   Calculate spread and midpoint from validated bid/ask quotes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of quote. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/quote.h"
#include "../base/value_archive_internal.h"
#include <math.h>
/* Check that quote satisfies its contract before another service relies on it. */
int umi_quote_valid(const UmiQuote *q){
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (q == NULL) return 0;
    if (memchr(q->key.key_id.value, '\0', sizeof(q->key.key_id.value)) == NULL) return 0;
    if (memchr(q->key.name, '\0', sizeof(q->key.name)) == NULL) return 0;
    if (memchr(q->key.code, '\0', sizeof(q->key.code)) == NULL) return 0;
return q!=NULL&&isfinite(q->bid)&&isfinite(q->ask)&&isfinite(q->bid_size)&&isfinite(q->ask_size)&&q->bid>0.0&&q->ask>=q->bid&&q->bid_size>=0.0&&q->ask_size>=0.0;}
/* Provide the quote mid operation used by this module and its client applications. */
/*
 * Valid endpoints satisfy 0 < bid <= ask and are finite, so ask - bid is
 * bounded by ask. Do not add bid + ask first: two valid large prices could
 * overflow that intermediate sum. Invalid quotes keep the existing 0 sentinel.
 */
double umi_quote_mid(const UmiQuote *q){return umi_quote_valid(q)?q->bid+(q->ask-q->bid)*0.5:0.0;}
/* Provide the quote spread operation used by this module and its client applications. */
double umi_quote_spread(const UmiQuote *q){return umi_quote_valid(q)?q->ask-q->bid:0.0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiQuoteArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7a3d3f9e5f369f33);
    schema = (schema ^ (uint64_t)sizeof(((UmiQuote *)0)->key.key_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiQuote *)0)->key.name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiQuote *)0)->key.code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiQuoteArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiQuote *)0)->key.key_id.value) - 1U +
        8U + sizeof(((UmiQuote *)0)->key.name) - 1U +
        8U + sizeof(((UmiQuote *)0)->key.code) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiQuoteArchiveWrite(UmiArchiveWriter *writer, const UmiQuote *value)
{
    UmiArchiveWriteText(writer, value->key.key_id.value, sizeof(value->key.key_id.value));
    UmiArchiveWriteText(writer, value->key.name, sizeof(value->key.name));
    UmiArchiveWriteText(writer, value->key.code, sizeof(value->key.code));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->key.state);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->key.active);
    UmiArchiveWriteSigned(writer, (int64_t)value->side);
    UmiArchiveWriteDouble(writer, value->price.value);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->price.scale);
    UmiArchiveWriteSigned(writer, (int64_t)value->timestamp);
}
static void UmiQuoteArchiveRead(UmiArchiveReader *reader, UmiQuote *value)
{
    UmiArchiveReadText(reader, value->key.key_id.value, sizeof(value->key.key_id.value));
    UmiArchiveReadText(reader, value->key.name, sizeof(value->key.name));
    UmiArchiveReadText(reader, value->key.code, sizeof(value->key.code));
    value->key.state = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->key.active = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->side = (UmiQuoteSide)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->price.value = UmiArchiveReadDouble(reader);
    value->price.scale = (uint8_t)UmiArchiveReadUnsigned(reader, UINT8_MAX);
    value->timestamp = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiQuoteArchiveValidate(const UmiQuote *value)
{
    return umi_quote_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_quote_archive_encode, umi_quote_archive_decode,
    UmiQuote, UmiQuoteArchiveSchema, UmiQuoteArchiveBound, UmiQuoteArchiveWrite, UmiQuoteArchiveRead, UmiQuoteArchiveValidate)
