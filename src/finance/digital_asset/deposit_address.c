/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/deposit_address.c
 *
 * PURPOSE:
 *   Assign a verified network address for deposits into a custody account.
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

#include "umicom/finance/digital_asset/deposit_address.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_deposit_address_init(UmiDigitalDepositAddress *value, const char *account_id, const char *asset_id, const char *network_id, const char *address)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->account_id.value, sizeof value->account_id.value, account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->asset_id.value, sizeof value->asset_id.value, asset_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->network_id.value, sizeof value->network_id.value, network_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->address, sizeof value->address, address);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_deposit_address_valid(const UmiDigitalDepositAddress *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->account_id.value, '\0', sizeof(value->account_id.value)) == NULL) return 0;
    if (memchr(value->asset_id.value, '\0', sizeof(value->asset_id.value)) == NULL) return 0;
    if (memchr(value->network_id.value, '\0', sizeof(value->network_id.value)) == NULL) return 0;
    if (memchr(value->address, '\0', sizeof(value->address)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->account_id.value) && umi_digital_asset_text_valid(value->asset_id.value) && umi_digital_asset_text_valid(value->address) && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalDepositAddressArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x5aa25d1f08851024);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalDepositAddress *)0)->account_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalDepositAddress *)0)->asset_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalDepositAddress *)0)->network_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalDepositAddress *)0)->address)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalDepositAddressArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalDepositAddress *)0)->account_id.value) - 1U +
        8U + sizeof(((UmiDigitalDepositAddress *)0)->asset_id.value) - 1U +
        8U + sizeof(((UmiDigitalDepositAddress *)0)->network_id.value) - 1U +
        8U + sizeof(((UmiDigitalDepositAddress *)0)->address) - 1U +
        8U;
}
static void UmiDigitalDepositAddressArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalDepositAddress *value)
{
    UmiArchiveWriteText(writer, value->account_id.value, sizeof(value->account_id.value));
    UmiArchiveWriteText(writer, value->asset_id.value, sizeof(value->asset_id.value));
    UmiArchiveWriteText(writer, value->network_id.value, sizeof(value->network_id.value));
    UmiArchiveWriteText(writer, value->address, sizeof(value->address));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiDigitalDepositAddressArchiveRead(UmiArchiveReader *reader, UmiDigitalDepositAddress *value)
{
    UmiArchiveReadText(reader, value->account_id.value, sizeof(value->account_id.value));
    UmiArchiveReadText(reader, value->asset_id.value, sizeof(value->asset_id.value));
    UmiArchiveReadText(reader, value->network_id.value, sizeof(value->network_id.value));
    UmiArchiveReadText(reader, value->address, sizeof(value->address));
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDigitalDepositAddressArchiveValidate(const UmiDigitalDepositAddress *value)
{
    return umi_digital_asset_deposit_address_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_deposit_address_archive_encode, umi_digital_asset_deposit_address_archive_decode,
    UmiDigitalDepositAddress, UmiDigitalDepositAddressArchiveSchema, UmiDigitalDepositAddressArchiveBound, UmiDigitalDepositAddressArchiveWrite, UmiDigitalDepositAddressArchiveRead, UmiDigitalDepositAddressArchiveValidate)
