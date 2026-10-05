/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/core/quote.c
 *
 * PURPOSE:
 *   Implement timestamped quote validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/core/quote.h"

/* Initialize quote. */ UmiStatus umi_quote_init(UmiQuote *q,const UmiMarketDataKey *k,UmiQuoteSide side,UmiFinancialPrice p,int64_t ts){if(q==NULL||k==NULL||!umi_market_data_key_is_valid(k)||side>UMI_QUOTE_LAST||!umi_price_is_valid(&p)||ts<0)return UMI_STATUS_INVALID_ARGUMENT;q->key=*k;q->side=side;q->price=p;q->timestamp=ts;return UMI_STATUS_OK;}
/* Validate quote. */ bool umi_quote_is_valid(const UmiQuote *q){return q!=NULL&&umi_market_data_key_is_valid(&q->key)&&q->side<=UMI_QUOTE_LAST&&umi_price_is_valid(&q->price)&&q->timestamp>=0;}

/* Finance owns the market-data-key quote archive declared in its public
 * header. Retain the original schema and field order; moving its definition
 * from the wrong Trading owner does not invent a new file format or ABI. */
#include "../../base/value_archive_internal.h"

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
/* Bound every encoded field explicitly; do not serialise C structure padding. */
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
/* Emit the established Finance wire fields in their original schema order. */
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
/* Read the same fields with their original numeric and fixed-text limits. */
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
/* The generic codec validates a candidate before replacing the caller's value. */
static UmiStatus UmiQuoteArchiveValidate(const UmiQuote *value)
{
/* Use the Finance validator, including its bounded key validation. The
 * similarly named Trading validator accepts a different C structure.
 * The superseded implementation is retained below for engineering review. */
#if 0
    return umi_quote_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
#endif
    /* Validate this Finance value with its own domain contract before publication. */
    return umi_quote_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_quote_archive_encode, umi_quote_archive_decode,
    UmiQuote, UmiQuoteArchiveSchema, UmiQuoteArchiveBound, UmiQuoteArchiveWrite, UmiQuoteArchiveRead, UmiQuoteArchiveValidate)
