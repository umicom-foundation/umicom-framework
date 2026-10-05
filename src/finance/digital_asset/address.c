/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/address.c
 *
 * PURPOSE:
 *   Implement a network-qualified external or custody address without embedding network SDK types.
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

#include "umicom/finance/digital_asset/address.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_address_init(UmiDigitalAssetAddress *value, const char *network_id, const char *address, bool verified)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->network_id.value, sizeof value->network_id.value, network_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->value, sizeof value->value, address);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->verified = verified;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_address_valid(const UmiDigitalAssetAddress *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->network_id.value, '\0', sizeof(value->network_id.value)) == NULL) return 0;
    if (memchr(value->value, '\0', sizeof(value->value)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->network_id.value) && umi_digital_asset_text_valid(value->value));
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalAssetAddressArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x35227d2754ab7c73);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalAssetAddress *)0)->network_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalAssetAddress *)0)->value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalAssetAddressArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalAssetAddress *)0)->network_id.value) - 1U +
        8U + sizeof(((UmiDigitalAssetAddress *)0)->value) - 1U +
        8U;
}
static void UmiDigitalAssetAddressArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalAssetAddress *value)
{
    UmiArchiveWriteText(writer, value->network_id.value, sizeof(value->network_id.value));
    UmiArchiveWriteText(writer, value->value, sizeof(value->value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->verified);
}
static void UmiDigitalAssetAddressArchiveRead(UmiArchiveReader *reader, UmiDigitalAssetAddress *value)
{
    UmiArchiveReadText(reader, value->network_id.value, sizeof(value->network_id.value));
    UmiArchiveReadText(reader, value->value, sizeof(value->value));
    value->verified = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDigitalAssetAddressArchiveValidate(const UmiDigitalAssetAddress *value)
{
    return umi_digital_asset_address_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_address_archive_encode, umi_digital_asset_address_archive_decode,
    UmiDigitalAssetAddress, UmiDigitalAssetAddressArchiveSchema, UmiDigitalAssetAddressArchiveBound, UmiDigitalAssetAddressArchiveWrite, UmiDigitalAssetAddressArchiveRead, UmiDigitalAssetAddressArchiveValidate)
