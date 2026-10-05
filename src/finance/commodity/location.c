/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/location.c
 *
 * PURPOSE:
 *   Implement a physical delivery, storage or logistics location.
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

#include "umicom/finance/commodity/location.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_location_init(UmiCommodityLocation *value, const char *id, const char *name, const char *country_code)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->name, sizeof value->name, name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->country_code, sizeof value->country_code, country_code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_location_valid(const UmiCommodityLocation *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->name, '\0', sizeof(value->name)) == NULL) return 0;
    if (memchr(value->country_code, '\0', sizeof(value->country_code)) == NULL) return 0;

    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->name) && strlen(value->country_code) == 2U && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCommodityLocationArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0eed721ec3407207);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityLocation *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityLocation *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityLocation *)0)->country_code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCommodityLocationArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCommodityLocation *)0)->id.value) - 1U +
        8U + sizeof(((UmiCommodityLocation *)0)->name) - 1U +
        8U + sizeof(((UmiCommodityLocation *)0)->country_code) - 1U +
        8U;
}
static void UmiCommodityLocationArchiveWrite(UmiArchiveWriter *writer, const UmiCommodityLocation *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->country_code, sizeof(value->country_code));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiCommodityLocationArchiveRead(UmiArchiveReader *reader, UmiCommodityLocation *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->country_code, sizeof(value->country_code));
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCommodityLocationArchiveValidate(const UmiCommodityLocation *value)
{
    return umi_commodity_location_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_commodity_location_archive_encode, umi_commodity_location_archive_decode,
    UmiCommodityLocation, UmiCommodityLocationArchiveSchema, UmiCommodityLocationArchiveBound, UmiCommodityLocationArchiveWrite, UmiCommodityLocationArchiveRead, UmiCommodityLocationArchiveValidate)
