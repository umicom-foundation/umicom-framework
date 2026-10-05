/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/asset_descriptor.c
 *
 * PURPOSE:
 *   Implement a fungible digital asset and the network on which it is represented.
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

#include "umicom/finance/digital_asset/asset_descriptor.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_asset_descriptor_init(UmiDigitalAssetDescriptor *value, const char *id, const char *symbol, const char *name, const char *network_id, uint32_t decimals, bool native_asset)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || decimals > 18U) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->symbol, sizeof value->symbol, symbol);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->name, sizeof value->name, name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->network_id.value, sizeof value->network_id.value, network_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->decimals = decimals;
    value->native_asset = native_asset;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_asset_descriptor_valid(const UmiDigitalAssetDescriptor *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->symbol, '\0', sizeof(value->symbol)) == NULL) return 0;
    if (memchr(value->name, '\0', sizeof(value->name)) == NULL) return 0;
    if (memchr(value->network_id.value, '\0', sizeof(value->network_id.value)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->symbol) && umi_digital_asset_text_valid(value->network_id.value) && value->decimals <= 18U && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalAssetDescriptorArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x766b16e960fac56d);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalAssetDescriptor *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalAssetDescriptor *)0)->symbol)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalAssetDescriptor *)0)->name)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalAssetDescriptor *)0)->network_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalAssetDescriptorArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalAssetDescriptor *)0)->id.value) - 1U +
        8U + sizeof(((UmiDigitalAssetDescriptor *)0)->symbol) - 1U +
        8U + sizeof(((UmiDigitalAssetDescriptor *)0)->name) - 1U +
        8U + sizeof(((UmiDigitalAssetDescriptor *)0)->network_id.value) - 1U +
        8U +
        8U +
        8U;
}
static void UmiDigitalAssetDescriptorArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalAssetDescriptor *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->symbol, sizeof(value->symbol));
    UmiArchiveWriteText(writer, value->name, sizeof(value->name));
    UmiArchiveWriteText(writer, value->network_id.value, sizeof(value->network_id.value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->decimals);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->native_asset);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiDigitalAssetDescriptorArchiveRead(UmiArchiveReader *reader, UmiDigitalAssetDescriptor *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->symbol, sizeof(value->symbol));
    UmiArchiveReadText(reader, value->name, sizeof(value->name));
    UmiArchiveReadText(reader, value->network_id.value, sizeof(value->network_id.value));
    value->decimals = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->native_asset = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDigitalAssetDescriptorArchiveValidate(const UmiDigitalAssetDescriptor *value)
{
    return umi_digital_asset_asset_descriptor_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_asset_descriptor_archive_encode, umi_digital_asset_asset_descriptor_archive_decode,
    UmiDigitalAssetDescriptor, UmiDigitalAssetDescriptorArchiveSchema, UmiDigitalAssetDescriptorArchiveBound, UmiDigitalAssetDescriptorArchiveWrite, UmiDigitalAssetDescriptorArchiveRead, UmiDigitalAssetDescriptorArchiveValidate)
