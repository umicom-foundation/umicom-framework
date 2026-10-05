/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/storage_facility.c
 *
 * PURPOSE:
 *   Implement storage capacity at a physical commodity location.
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

#include "umicom/finance/commodity/storage_facility.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_storage_facility_init(UmiCommodityStorageFacility *value, const char *id, const char *location_id, int64_t capacity_units, int32_t scale, const char *unit_code)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || capacity_units < 0 || scale < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->location_id.value, sizeof value->location_id.value, location_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->capacity.unit_code, sizeof value->capacity.unit_code, unit_code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->capacity.units = capacity_units;
    value->capacity.scale = scale;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_storage_facility_valid(const UmiCommodityStorageFacility *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->location_id.value, '\0', sizeof(value->location_id.value)) == NULL) return 0;
    if (memchr(value->capacity.unit_code, '\0', sizeof(value->capacity.unit_code)) == NULL) return 0;

    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->location_id.value) && value->capacity.units >= 0 && umi_commodity_text_valid(value->capacity.unit_code) && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCommodityStorageFacilityArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8ea39e73ed928751);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityStorageFacility *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityStorageFacility *)0)->location_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityStorageFacility *)0)->capacity.unit_code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCommodityStorageFacilityArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCommodityStorageFacility *)0)->id.value) - 1U +
        8U + sizeof(((UmiCommodityStorageFacility *)0)->location_id.value) - 1U +
        8U +
        8U +
        8U + sizeof(((UmiCommodityStorageFacility *)0)->capacity.unit_code) - 1U +
        8U;
}
static void UmiCommodityStorageFacilityArchiveWrite(UmiArchiveWriter *writer, const UmiCommodityStorageFacility *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->location_id.value, sizeof(value->location_id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->capacity.units);
    UmiArchiveWriteSigned(writer, (int64_t)value->capacity.scale);
    UmiArchiveWriteText(writer, value->capacity.unit_code, sizeof(value->capacity.unit_code));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiCommodityStorageFacilityArchiveRead(UmiArchiveReader *reader, UmiCommodityStorageFacility *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->location_id.value, sizeof(value->location_id.value));
    value->capacity.units = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->capacity.scale = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    UmiArchiveReadText(reader, value->capacity.unit_code, sizeof(value->capacity.unit_code));
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCommodityStorageFacilityArchiveValidate(const UmiCommodityStorageFacility *value)
{
    return umi_commodity_storage_facility_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_commodity_storage_facility_archive_encode, umi_commodity_storage_facility_archive_decode,
    UmiCommodityStorageFacility, UmiCommodityStorageFacilityArchiveSchema, UmiCommodityStorageFacilityArchiveBound, UmiCommodityStorageFacilityArchiveWrite, UmiCommodityStorageFacilityArchiveRead, UmiCommodityStorageFacilityArchiveValidate)
