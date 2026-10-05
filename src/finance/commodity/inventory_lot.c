/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/inventory_lot.c
 *
 * PURPOSE:
 *   Implement an auditable physical inventory lot and its available quantity.
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

#include "umicom/finance/commodity/inventory_lot.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_inventory_lot_init(UmiCommodityInventoryLot *value, const char *id, const char *commodity_id, const char *facility_id, int64_t units, int32_t scale, const char *unit_code, int64_t received_time_ms)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || units < 0 || scale < 0 || received_time_ms < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->commodity_id.value, sizeof value->commodity_id.value, commodity_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->facility_id.value, sizeof value->facility_id.value, facility_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->quantity.unit_code, sizeof value->quantity.unit_code, unit_code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->quantity.units = units;
    value->quantity.scale = scale;
    value->received_time_ms = received_time_ms;
    value->quality_accepted = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_inventory_lot_valid(const UmiCommodityInventoryLot *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->commodity_id.value, '\0', sizeof(value->commodity_id.value)) == NULL) return 0;
    if (memchr(value->facility_id.value, '\0', sizeof(value->facility_id.value)) == NULL) return 0;
    if (memchr(value->quantity.unit_code, '\0', sizeof(value->quantity.unit_code)) == NULL) return 0;

    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->commodity_id.value) && umi_commodity_text_valid(value->facility_id.value) && value->quantity.units >= 0 && value->quality_accepted);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCommodityInventoryLotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x663e394aaaf9a54e);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityInventoryLot *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityInventoryLot *)0)->commodity_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityInventoryLot *)0)->facility_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityInventoryLot *)0)->quantity.unit_code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCommodityInventoryLotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCommodityInventoryLot *)0)->id.value) - 1U +
        8U + sizeof(((UmiCommodityInventoryLot *)0)->commodity_id.value) - 1U +
        8U + sizeof(((UmiCommodityInventoryLot *)0)->facility_id.value) - 1U +
        8U +
        8U +
        8U + sizeof(((UmiCommodityInventoryLot *)0)->quantity.unit_code) - 1U +
        8U +
        8U;
}
static void UmiCommodityInventoryLotArchiveWrite(UmiArchiveWriter *writer, const UmiCommodityInventoryLot *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->commodity_id.value, sizeof(value->commodity_id.value));
    UmiArchiveWriteText(writer, value->facility_id.value, sizeof(value->facility_id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->quantity.units);
    UmiArchiveWriteSigned(writer, (int64_t)value->quantity.scale);
    UmiArchiveWriteText(writer, value->quantity.unit_code, sizeof(value->quantity.unit_code));
    UmiArchiveWriteSigned(writer, (int64_t)value->received_time_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->quality_accepted);
}
static void UmiCommodityInventoryLotArchiveRead(UmiArchiveReader *reader, UmiCommodityInventoryLot *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->commodity_id.value, sizeof(value->commodity_id.value));
    UmiArchiveReadText(reader, value->facility_id.value, sizeof(value->facility_id.value));
    value->quantity.units = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->quantity.scale = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    UmiArchiveReadText(reader, value->quantity.unit_code, sizeof(value->quantity.unit_code));
    value->received_time_ms = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->quality_accepted = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCommodityInventoryLotArchiveValidate(const UmiCommodityInventoryLot *value)
{
    return umi_commodity_inventory_lot_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_commodity_inventory_lot_archive_encode, umi_commodity_inventory_lot_archive_decode,
    UmiCommodityInventoryLot, UmiCommodityInventoryLotArchiveSchema, UmiCommodityInventoryLotArchiveBound, UmiCommodityInventoryLotArchiveWrite, UmiCommodityInventoryLotArchiveRead, UmiCommodityInventoryLotArchiveValidate)
