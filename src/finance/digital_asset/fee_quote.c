/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/fee_quote.c
 *
 * PURPOSE:
 *   Implement a time-bounded fee quote for a digital-asset network.
 *
 * ARCHITECTURE:
 *   This capability is Framework-owned and reusable by thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/digital_asset/fee_quote.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_fee_quote_init(UmiDigitalFeeQuote *value, const char *network_id, int64_t fee_units, int32_t scale, const char *asset_symbol, int64_t expires_time_ms)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || fee_units < 0 || scale < 0 || expires_time_ms < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->network_id.value, sizeof value->network_id.value, network_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->estimated_fee.asset_symbol, sizeof value->estimated_fee.asset_symbol, asset_symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->estimated_fee.units = fee_units;
    value->estimated_fee.scale = scale;
    value->expires_time_ms = expires_time_ms;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_fee_quote_valid(const UmiDigitalFeeQuote *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->network_id.value, '\0', sizeof(value->network_id.value)) == NULL) return 0;
    if (memchr(value->estimated_fee.asset_symbol, '\0', sizeof(value->estimated_fee.asset_symbol)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->network_id.value) && value->estimated_fee.units >= 0 && value->expires_time_ms >= 0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalFeeQuoteArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x26e1a044893d86f1);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalFeeQuote *)0)->network_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalFeeQuote *)0)->estimated_fee.asset_symbol)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalFeeQuoteArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalFeeQuote *)0)->network_id.value) - 1U +
        8U +
        8U +
        8U + sizeof(((UmiDigitalFeeQuote *)0)->estimated_fee.asset_symbol) - 1U +
        8U;
}
static void UmiDigitalFeeQuoteArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalFeeQuote *value)
{
    UmiArchiveWriteText(writer, value->network_id.value, sizeof(value->network_id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->estimated_fee.units);
    UmiArchiveWriteSigned(writer, (int64_t)value->estimated_fee.scale);
    UmiArchiveWriteText(writer, value->estimated_fee.asset_symbol, sizeof(value->estimated_fee.asset_symbol));
    UmiArchiveWriteSigned(writer, (int64_t)value->expires_time_ms);
}
static void UmiDigitalFeeQuoteArchiveRead(UmiArchiveReader *reader, UmiDigitalFeeQuote *value)
{
    UmiArchiveReadText(reader, value->network_id.value, sizeof(value->network_id.value));
    value->estimated_fee.units = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->estimated_fee.scale = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    UmiArchiveReadText(reader, value->estimated_fee.asset_symbol, sizeof(value->estimated_fee.asset_symbol));
    value->expires_time_ms = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiDigitalFeeQuoteArchiveValidate(const UmiDigitalFeeQuote *value)
{
    return umi_digital_asset_fee_quote_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_fee_quote_archive_encode, umi_digital_asset_fee_quote_archive_decode,
    UmiDigitalFeeQuote, UmiDigitalFeeQuoteArchiveSchema, UmiDigitalFeeQuoteArchiveBound, UmiDigitalFeeQuoteArchiveWrite, UmiDigitalFeeQuoteArchiveRead, UmiDigitalFeeQuoteArchiveValidate)
