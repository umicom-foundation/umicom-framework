/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/shipment.c
 *
 * PURPOSE:
 *   Implement a physical shipment and its planned versus delivered quantity.
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

#include "umicom/finance/commodity/shipment.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_shipment_init(UmiCommodityShipment *value, const char *id, const char *contract_id, const char *route_id, int64_t planned_units, int32_t scale, const char *unit_code)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || planned_units <= 0 || scale < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->contract_id.value, sizeof value->contract_id.value, contract_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->route_id.value, sizeof value->route_id.value, route_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->planned_quantity.unit_code, sizeof value->planned_quantity.unit_code, unit_code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->planned_quantity.units = planned_units;
    value->planned_quantity.scale = scale;
    value->state = UMI_COMMODITY_SHIPMENT_PLANNED;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_shipment_valid(const UmiCommodityShipment *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->contract_id.value, '\0', sizeof(value->contract_id.value)) == NULL) return 0;
    if (memchr(value->route_id.value, '\0', sizeof(value->route_id.value)) == NULL) return 0;
    if (memchr(value->planned_quantity.unit_code, '\0', sizeof(value->planned_quantity.unit_code)) == NULL) return 0;

    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->contract_id.value) && value->planned_quantity.units > 0 && value->delivered_units >= 0 && value->delivered_units <= value->planned_quantity.units);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCommodityShipmentArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa530620565f0f01b);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityShipment *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityShipment *)0)->contract_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityShipment *)0)->route_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityShipment *)0)->planned_quantity.unit_code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCommodityShipmentArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCommodityShipment *)0)->id.value) - 1U +
        8U + sizeof(((UmiCommodityShipment *)0)->contract_id.value) - 1U +
        8U + sizeof(((UmiCommodityShipment *)0)->route_id.value) - 1U +
        8U +
        8U +
        8U + sizeof(((UmiCommodityShipment *)0)->planned_quantity.unit_code) - 1U +
        8U +
        8U;
}
static void UmiCommodityShipmentArchiveWrite(UmiArchiveWriter *writer, const UmiCommodityShipment *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->contract_id.value, sizeof(value->contract_id.value));
    UmiArchiveWriteText(writer, value->route_id.value, sizeof(value->route_id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->planned_quantity.units);
    UmiArchiveWriteSigned(writer, (int64_t)value->planned_quantity.scale);
    UmiArchiveWriteText(writer, value->planned_quantity.unit_code, sizeof(value->planned_quantity.unit_code));
    UmiArchiveWriteSigned(writer, (int64_t)value->delivered_units);
    UmiArchiveWriteSigned(writer, (int64_t)value->state);
}
static void UmiCommodityShipmentArchiveRead(UmiArchiveReader *reader, UmiCommodityShipment *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->contract_id.value, sizeof(value->contract_id.value));
    UmiArchiveReadText(reader, value->route_id.value, sizeof(value->route_id.value));
    value->planned_quantity.units = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->planned_quantity.scale = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    UmiArchiveReadText(reader, value->planned_quantity.unit_code, sizeof(value->planned_quantity.unit_code));
    value->delivered_units = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->state = (UmiCommodityShipmentState)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiCommodityShipmentArchiveValidate(const UmiCommodityShipment *value)
{
    return umi_commodity_shipment_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_commodity_shipment_archive_encode, umi_commodity_shipment_archive_decode,
    UmiCommodityShipment, UmiCommodityShipmentArchiveSchema, UmiCommodityShipmentArchiveBound, UmiCommodityShipmentArchiveWrite, UmiCommodityShipmentArchiveRead, UmiCommodityShipmentArchiveValidate)
