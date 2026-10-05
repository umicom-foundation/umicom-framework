/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/network_descriptor.c
 *
 * PURPOSE:
 *   Implement provider-neutral blockchain or distributed-ledger network metadata.
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

#include "umicom/finance/digital_asset/network_descriptor.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_network_descriptor_init(UmiDigitalNetworkDescriptor *value, const char *id, const char *name, UmiDigitalNetworkFamily family, uint32_t minimum_confirmations)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || family == UMI_DIGITAL_NETWORK_UNKNOWN) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->name, sizeof value->name, name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->family = family;
    value->minimum_confirmations = minimum_confirmations;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_network_descriptor_valid(const UmiDigitalNetworkDescriptor *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->name, '\0', sizeof(value->name)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->name) && value->family != UMI_DIGITAL_NETWORK_UNKNOWN && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalNetworkDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8430daccb5aca701);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalNetworkDescriptor *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalNetworkDescriptor *)0)->name)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalNetworkDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalNetworkDescriptor *)0)->id.value) - 1U +
        8U + sizeof(((UmiDigitalNetworkDescriptor *)0)->name) - 1U +
        8U +
        8U +
        8U;
}
static void UmiDigitalNetworkDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalNetworkDescriptor *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteSigned(writer, (int64_t)value->family);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->minimum_confirmations);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiDigitalNetworkDescriptorArchiveRead(UmiArchiveReader *reader, UmiDigitalNetworkDescriptor *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    value->family = (UmiDigitalNetworkFamily)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->minimum_confirmations = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDigitalNetworkDescriptorArchiveValidate(const UmiDigitalNetworkDescriptor *value)
{
    return umi_digital_asset_network_descriptor_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_network_descriptor_archive_encode, umi_digital_asset_network_descriptor_archive_decode,
    UmiDigitalNetworkDescriptor, UmiDigitalNetworkDescriptorArchiveSchema, UmiDigitalNetworkDescriptorArchiveBound, UmiDigitalNetworkDescriptorArchiveWrite, UmiDigitalNetworkDescriptorArchiveRead, UmiDigitalNetworkDescriptorArchiveValidate)
