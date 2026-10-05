/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/custody_account.c
 *
 * PURPOSE:
 *   Implement a client or house digital-asset custody account bound to a wallet.
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

#include "umicom/finance/digital_asset/custody_account.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_custody_account_init(UmiDigitalCustodyAccount *value, const char *id, const UmiFinancialId *owner_party_id, const char *wallet_id, bool segregated)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || owner_party_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->wallet_id.value, sizeof value->wallet_id.value, wallet_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->owner_party_id = *owner_party_id;
    value->segregated = segregated;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_custody_account_valid(const UmiDigitalCustodyAccount *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->owner_party_id.value, '\0', sizeof(value->owner_party_id.value)) == NULL) return 0;
    if (memchr(value->wallet_id.value, '\0', sizeof(value->wallet_id.value)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->wallet_id.value) && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalCustodyAccountArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd32ececf33b94aee);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalCustodyAccount *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalCustodyAccount *)0)->owner_party_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalCustodyAccount *)0)->wallet_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalCustodyAccountArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalCustodyAccount *)0)->id.value) - 1U +
        8U + sizeof(((UmiDigitalCustodyAccount *)0)->owner_party_id.value) - 1U +
        8U + sizeof(((UmiDigitalCustodyAccount *)0)->wallet_id.value) - 1U +
        8U +
        8U;
}
static void UmiDigitalCustodyAccountArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalCustodyAccount *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->owner_party_id.value, sizeof(value->owner_party_id.value));
    UmiArchiveWriteText(writer, value->wallet_id.value, sizeof(value->wallet_id.value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->segregated);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiDigitalCustodyAccountArchiveRead(UmiArchiveReader *reader, UmiDigitalCustodyAccount *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->owner_party_id.value, sizeof(value->owner_party_id.value));
    UmiArchiveReadText(reader, value->wallet_id.value, sizeof(value->wallet_id.value));
    value->segregated = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDigitalCustodyAccountArchiveValidate(const UmiDigitalCustodyAccount *value)
{
    return umi_digital_asset_custody_account_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_custody_account_archive_encode, umi_digital_asset_custody_account_archive_decode,
    UmiDigitalCustodyAccount, UmiDigitalCustodyAccountArchiveSchema, UmiDigitalCustodyAccountArchiveBound, UmiDigitalCustodyAccountArchiveWrite, UmiDigitalCustodyAccountArchiveRead, UmiDigitalCustodyAccountArchiveValidate)
