/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/nomination.c
 *
 * PURPOSE:
 *   Implement a quantity nomination against a physical contract and delivery window.
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

#include "umicom/finance/commodity/nomination.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_nomination_init(UmiCommodityNomination *value, const char *id, const char *contract_id, int64_t units, int32_t scale, const char *unit_code, int64_t window_start_ms, int64_t window_end_ms)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || units <= 0 || scale < 0 || window_start_ms < 0 || window_end_ms <= window_start_ms) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->contract_id.value, sizeof value->contract_id.value, contract_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->quantity.unit_code, sizeof value->quantity.unit_code, unit_code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->quantity.units = units;
    value->quantity.scale = scale;
    value->window_start_ms = window_start_ms;
    value->window_end_ms = window_end_ms;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_nomination_valid(const UmiCommodityNomination *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->contract_id.value, '\0', sizeof(value->contract_id.value)) == NULL) return 0;
    if (memchr(value->quantity.unit_code, '\0', sizeof(value->quantity.unit_code)) == NULL) return 0;

    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->contract_id.value) && value->quantity.units > 0 && value->window_end_ms > value->window_start_ms);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCommodityNominationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x03654ca620912981);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityNomination *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityNomination *)0)->contract_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityNomination *)0)->quantity.unit_code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCommodityNominationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCommodityNomination *)0)->id.value) - 1U +
        8U + sizeof(((UmiCommodityNomination *)0)->contract_id.value) - 1U +
        8U +
        8U +
        8U + sizeof(((UmiCommodityNomination *)0)->quantity.unit_code) - 1U +
        8U +
        8U +
        8U;
}
static void UmiCommodityNominationArchiveWrite(UmiArchiveWriter *writer, const UmiCommodityNomination *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->contract_id.value, sizeof(value->contract_id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->quantity.units);
    UmiArchiveWriteSigned(writer, (int64_t)value->quantity.scale);
    UmiArchiveWriteText(writer, value->quantity.unit_code, sizeof(value->quantity.unit_code));
    UmiArchiveWriteSigned(writer, (int64_t)value->window_start_ms);
    UmiArchiveWriteSigned(writer, (int64_t)value->window_end_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->accepted);
}
static void UmiCommodityNominationArchiveRead(UmiArchiveReader *reader, UmiCommodityNomination *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->contract_id.value, sizeof(value->contract_id.value));
    value->quantity.units = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->quantity.scale = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    UmiArchiveReadText(reader, value->quantity.unit_code, sizeof(value->quantity.unit_code));
    value->window_start_ms = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->window_end_ms = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->accepted = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCommodityNominationArchiveValidate(const UmiCommodityNomination *value)
{
    return umi_commodity_nomination_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_commodity_nomination_archive_encode, umi_commodity_nomination_archive_decode,
    UmiCommodityNomination, UmiCommodityNominationArchiveSchema, UmiCommodityNominationArchiveBound, UmiCommodityNominationArchiveWrite, UmiCommodityNominationArchiveRead, UmiCommodityNominationArchiveValidate)
