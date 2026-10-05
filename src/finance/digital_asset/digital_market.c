/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/digital_market.c
 *
 * PURPOSE:
 *   Implement a digital-asset market pair that can be routed through canonical trading services.
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

#include "umicom/finance/digital_asset/digital_market.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_digital_market_init(UmiDigitalMarket *value, const char *id, const char *base_asset_id, const char *quote_asset_id, const char *venue, int64_t minimum_quantity_units)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || minimum_quantity_units < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->base_asset_id.value, sizeof value->base_asset_id.value, base_asset_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->quote_asset_id.value, sizeof value->quote_asset_id.value, quote_asset_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->venue, sizeof value->venue, venue);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->minimum_quantity_units = minimum_quantity_units;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_digital_market_valid(const UmiDigitalMarket *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->base_asset_id.value, '\0', sizeof(value->base_asset_id.value)) == NULL) return 0;
    if (memchr(value->quote_asset_id.value, '\0', sizeof(value->quote_asset_id.value)) == NULL) return 0;
    if (memchr(value->venue, '\0', sizeof(value->venue)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->base_asset_id.value) && umi_digital_asset_text_valid(value->quote_asset_id.value) && strcmp(value->base_asset_id.value, value->quote_asset_id.value) != 0 && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalMarketArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc21fbec25c07a1f2);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalMarket *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalMarket *)0)->base_asset_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalMarket *)0)->quote_asset_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalMarket *)0)->venue)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalMarketArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalMarket *)0)->id.value) - 1U +
        8U + sizeof(((UmiDigitalMarket *)0)->base_asset_id.value) - 1U +
        8U + sizeof(((UmiDigitalMarket *)0)->quote_asset_id.value) - 1U +
        8U + sizeof(((UmiDigitalMarket *)0)->venue) - 1U +
        8U +
        8U;
}
static void UmiDigitalMarketArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalMarket *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->base_asset_id.value, sizeof(value->base_asset_id.value));
    UmiArchiveWriteText(writer, value->quote_asset_id.value, sizeof(value->quote_asset_id.value));
    UmiArchiveWriteText(writer, value->venue, sizeof(value->venue));
    UmiArchiveWriteSigned(writer, (int64_t)value->minimum_quantity_units);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiDigitalMarketArchiveRead(UmiArchiveReader *reader, UmiDigitalMarket *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->base_asset_id.value, sizeof(value->base_asset_id.value));
    UmiArchiveReadText(reader, value->quote_asset_id.value, sizeof(value->quote_asset_id.value));
    UmiArchiveReadText(reader, value->venue, sizeof(value->venue));
    value->minimum_quantity_units = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDigitalMarketArchiveValidate(const UmiDigitalMarket *value)
{
    return umi_digital_asset_digital_market_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_digital_market_archive_encode, umi_digital_asset_digital_market_archive_decode,
    UmiDigitalMarket, UmiDigitalMarketArchiveSchema, UmiDigitalMarketArchiveBound, UmiDigitalMarketArchiveWrite, UmiDigitalMarketArchiveRead, UmiDigitalMarketArchiveValidate)
