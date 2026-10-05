/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/wallet.c
 *
 * PURPOSE:
 *   Implement a custody or operational wallet identity and its network binding.
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

#include "umicom/finance/digital_asset/wallet.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_wallet_init(UmiDigitalAssetWallet *value, const char *id, const char *label, const char *network_id, bool custodial)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->label, sizeof value->label, label);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->network_id.value, sizeof value->network_id.value, network_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->custodial = custodial;
    value->enabled = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_wallet_valid(const UmiDigitalAssetWallet *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->label, '\0', sizeof(value->label)) == NULL) return 0;
    if (memchr(value->network_id.value, '\0', sizeof(value->network_id.value)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->network_id.value) && value->enabled);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalAssetWalletArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xec9d17dfb7856ad8);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalAssetWallet *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalAssetWallet *)0)->label)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalAssetWallet *)0)->network_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalAssetWalletArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalAssetWallet *)0)->id.value) - 1U +
        8U + sizeof(((UmiDigitalAssetWallet *)0)->label) - 1U +
        8U + sizeof(((UmiDigitalAssetWallet *)0)->network_id.value) - 1U +
        8U +
        8U;
}
static void UmiDigitalAssetWalletArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalAssetWallet *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteText(writer, value->network_id.value, sizeof(value->network_id.value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->custodial);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
}
static void UmiDigitalAssetWalletArchiveRead(UmiArchiveReader *reader, UmiDigitalAssetWallet *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    UmiArchiveReadText(reader, value->network_id.value, sizeof(value->network_id.value));
    value->custodial = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDigitalAssetWalletArchiveValidate(const UmiDigitalAssetWallet *value)
{
    return umi_digital_asset_wallet_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_wallet_archive_encode, umi_digital_asset_wallet_archive_decode,
    UmiDigitalAssetWallet, UmiDigitalAssetWalletArchiveSchema, UmiDigitalAssetWalletArchiveBound, UmiDigitalAssetWalletArchiveWrite, UmiDigitalAssetWalletArchiveRead, UmiDigitalAssetWalletArchiveValidate)
