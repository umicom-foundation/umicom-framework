/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/transport_route.c
 *
 * PURPOSE:
 *   Implement an auditable commodity logistics route between physical locations.
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

#include "umicom/finance/commodity/transport_route.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_transport_route_init(UmiCommodityTransportRoute *value, const char *id, const char *origin_location_id, const char *destination_location_id, const char *mode_code)
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
    status = umi_commodity_copy_text(value->origin_location_id.value, sizeof value->origin_location_id.value, origin_location_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->destination_location_id.value, sizeof value->destination_location_id.value, destination_location_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->mode_code, sizeof value->mode_code, mode_code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_transport_route_valid(const UmiCommodityTransportRoute *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->origin_location_id.value, '\0', sizeof(value->origin_location_id.value)) == NULL) return 0;
    if (memchr(value->destination_location_id.value, '\0', sizeof(value->destination_location_id.value)) == NULL) return 0;
    if (memchr(value->mode_code, '\0', sizeof(value->mode_code)) == NULL) return 0;

    return value != NULL && (umi_commodity_text_valid(value->id.value) && umi_commodity_text_valid(value->origin_location_id.value) && umi_commodity_text_valid(value->destination_location_id.value) && strcmp(value->origin_location_id.value, value->destination_location_id.value) != 0 && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCommodityTransportRouteArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb56f6299da38bcc6);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityTransportRoute *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityTransportRoute *)0)->origin_location_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityTransportRoute *)0)->destination_location_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityTransportRoute *)0)->mode_code)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCommodityTransportRouteArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCommodityTransportRoute *)0)->id.value) - 1U +
        8U + sizeof(((UmiCommodityTransportRoute *)0)->origin_location_id.value) - 1U +
        8U + sizeof(((UmiCommodityTransportRoute *)0)->destination_location_id.value) - 1U +
        8U + sizeof(((UmiCommodityTransportRoute *)0)->mode_code) - 1U +
        8U;
}
static void UmiCommodityTransportRouteArchiveWrite(UmiArchiveWriter *writer, const UmiCommodityTransportRoute *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->origin_location_id.value, sizeof(value->origin_location_id.value));
    UmiArchiveWriteText(writer, value->destination_location_id.value, sizeof(value->destination_location_id.value));
    UmiArchiveWriteText(writer, value->mode_code, sizeof(value->mode_code));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiCommodityTransportRouteArchiveRead(UmiArchiveReader *reader, UmiCommodityTransportRoute *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->origin_location_id.value, sizeof(value->origin_location_id.value));
    UmiArchiveReadText(reader, value->destination_location_id.value, sizeof(value->destination_location_id.value));
    UmiArchiveReadText(reader, value->mode_code, sizeof(value->mode_code));
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCommodityTransportRouteArchiveValidate(const UmiCommodityTransportRoute *value)
{
    return umi_commodity_transport_route_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_commodity_transport_route_archive_encode, umi_commodity_transport_route_archive_decode,
    UmiCommodityTransportRoute, UmiCommodityTransportRouteArchiveSchema, UmiCommodityTransportRouteArchiveBound, UmiCommodityTransportRouteArchiveWrite, UmiCommodityTransportRouteArchiveRead, UmiCommodityTransportRouteArchiveValidate)
