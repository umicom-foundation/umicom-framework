/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/transport_leg.c
 *
 * PURPOSE:
 *   Implement one ordered physical leg within a commodity transport route.
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

#include "umicom/finance/commodity/transport_leg.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_transport_leg_init(UmiCommodityTransportLeg *value, const char *route_id, uint32_t sequence, const char *origin_location_id, const char *destination_location_id, int64_t planned_departure_ms, int64_t planned_arrival_ms)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || sequence == 0U || planned_departure_ms < 0 || planned_arrival_ms <= planned_departure_ms) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->route_id.value, sizeof value->route_id.value, route_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->origin_location_id.value, sizeof value->origin_location_id.value, origin_location_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->destination_location_id.value, sizeof value->destination_location_id.value, destination_location_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->sequence = sequence;
    value->planned_departure_ms = planned_departure_ms;
    value->planned_arrival_ms = planned_arrival_ms;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_transport_leg_valid(const UmiCommodityTransportLeg *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->route_id.value, '\0', sizeof(value->route_id.value)) == NULL) return 0;
    if (memchr(value->origin_location_id.value, '\0', sizeof(value->origin_location_id.value)) == NULL) return 0;
    if (memchr(value->destination_location_id.value, '\0', sizeof(value->destination_location_id.value)) == NULL) return 0;

    return value != NULL && (umi_commodity_text_valid(value->route_id.value) && value->sequence > 0U && value->planned_arrival_ms > value->planned_departure_ms);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCommodityTransportLegArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xc555949455fa4d33);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityTransportLeg *)0)->route_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityTransportLeg *)0)->origin_location_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityTransportLeg *)0)->destination_location_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCommodityTransportLegArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCommodityTransportLeg *)0)->route_id.value) - 1U +
        8U +
        8U + sizeof(((UmiCommodityTransportLeg *)0)->origin_location_id.value) - 1U +
        8U + sizeof(((UmiCommodityTransportLeg *)0)->destination_location_id.value) - 1U +
        8U +
        8U;
}
static void UmiCommodityTransportLegArchiveWrite(UmiArchiveWriter *writer, const UmiCommodityTransportLeg *value)
{
    UmiArchiveWriteText(writer, value->route_id.value, sizeof(value->route_id.value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->sequence);
    UmiArchiveWriteText(writer, value->origin_location_id.value, sizeof(value->origin_location_id.value));
    UmiArchiveWriteText(writer, value->destination_location_id.value, sizeof(value->destination_location_id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->planned_departure_ms);
    UmiArchiveWriteSigned(writer, (int64_t)value->planned_arrival_ms);
}
static void UmiCommodityTransportLegArchiveRead(UmiArchiveReader *reader, UmiCommodityTransportLeg *value)
{
    UmiArchiveReadText(reader, value->route_id.value, sizeof(value->route_id.value));
    value->sequence = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    UmiArchiveReadText(reader, value->origin_location_id.value, sizeof(value->origin_location_id.value));
    UmiArchiveReadText(reader, value->destination_location_id.value, sizeof(value->destination_location_id.value));
    value->planned_departure_ms = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->planned_arrival_ms = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiCommodityTransportLegArchiveValidate(const UmiCommodityTransportLeg *value)
{
    return umi_commodity_transport_leg_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_commodity_transport_leg_archive_encode, umi_commodity_transport_leg_archive_decode,
    UmiCommodityTransportLeg, UmiCommodityTransportLegArchiveSchema, UmiCommodityTransportLegArchiveBound, UmiCommodityTransportLegArchiveWrite, UmiCommodityTransportLegArchiveRead, UmiCommodityTransportLegArchiveValidate)
