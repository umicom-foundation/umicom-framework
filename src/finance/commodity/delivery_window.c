/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/delivery_window.c
 *
 * PURPOSE:
 *   Implement the permitted start and end time for a commodity delivery.
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

#include "umicom/finance/commodity/delivery_window.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_delivery_window_init(UmiCommodityDeliveryWindow *value, int64_t start_time_ms, int64_t end_time_ms, bool inclusive_end)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || start_time_ms < 0 || end_time_ms <= start_time_ms) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    value->start_time_ms = start_time_ms;
    value->end_time_ms = end_time_ms;
    value->inclusive_end = inclusive_end;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_delivery_window_valid(const UmiCommodityDeliveryWindow *value)
{
    return value != NULL && (value->start_time_ms >= 0 && value->end_time_ms > value->start_time_ms);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCommodityDeliveryWindowArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x185a1d630a6b7a9f);

    return schema;
}
static size_t UmiCommodityDeliveryWindowArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiCommodityDeliveryWindowArchiveWrite(UmiArchiveWriter *writer, const UmiCommodityDeliveryWindow *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->start_time_ms);
    UmiArchiveWriteSigned(writer, (int64_t)value->end_time_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->inclusive_end);
}
static void UmiCommodityDeliveryWindowArchiveRead(UmiArchiveReader *reader, UmiCommodityDeliveryWindow *value)
{
    value->start_time_ms = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->end_time_ms = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->inclusive_end = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCommodityDeliveryWindowArchiveValidate(const UmiCommodityDeliveryWindow *value)
{
    return umi_commodity_delivery_window_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_commodity_delivery_window_archive_encode, umi_commodity_delivery_window_archive_decode,
    UmiCommodityDeliveryWindow, UmiCommodityDeliveryWindowArchiveSchema, UmiCommodityDeliveryWindowArchiveBound, UmiCommodityDeliveryWindowArchiveWrite, UmiCommodityDeliveryWindowArchiveRead, UmiCommodityDeliveryWindowArchiveValidate)
