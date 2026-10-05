/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/commodity/transport_mode.c
 *
 * PURPOSE:
 *   Implement a reusable physical transport mode such as vessel, pipeline, rail or truck.
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

#include "umicom/finance/commodity/transport_mode.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_commodity_transport_mode_init(UmiCommodityTransportMode *value, const char *code, const char *name, bool supports_bulk)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_commodity_copy_text(value->code, sizeof value->code, code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_commodity_copy_text(value->name, sizeof value->name, name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->supports_bulk = supports_bulk;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_commodity_transport_mode_valid(const UmiCommodityTransportMode *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->code, '\0', sizeof(value->code)) == NULL) return 0;
    if (memchr(value->name, '\0', sizeof(value->name)) == NULL) return 0;

    return value != NULL && (umi_commodity_text_valid(value->code) && umi_commodity_text_valid(value->name) && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiCommodityTransportModeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5204530b3a29a450);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityTransportMode *)0)->code)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiCommodityTransportMode *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiCommodityTransportModeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiCommodityTransportMode *)0)->code) - 1U +
        8U + sizeof(((UmiCommodityTransportMode *)0)->name) - 1U +
        8U +
        8U;
}
static void UmiCommodityTransportModeArchiveWrite(UmiArchiveWriter *writer, const UmiCommodityTransportMode *value)
{
    UmiArchiveWriteText(writer, value->code, sizeof(value->code));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->supports_bulk);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiCommodityTransportModeArchiveRead(UmiArchiveReader *reader, UmiCommodityTransportMode *value)
{
    UmiArchiveReadText(reader, value->code, sizeof(value->code));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->supports_bulk = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiCommodityTransportModeArchiveValidate(const UmiCommodityTransportMode *value)
{
    return umi_commodity_transport_mode_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_commodity_transport_mode_archive_encode, umi_commodity_transport_mode_archive_decode,
    UmiCommodityTransportMode, UmiCommodityTransportModeArchiveSchema, UmiCommodityTransportModeArchiveBound, UmiCommodityTransportModeArchiveWrite, UmiCommodityTransportModeArchiveRead, UmiCommodityTransportModeArchiveValidate)
