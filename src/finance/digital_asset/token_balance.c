/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/token_balance.c
 *
 * PURPOSE:
 *   Implement a custody-account balance for one digital asset using integer minor units.
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

#include "umicom/finance/digital_asset/token_balance.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_token_balance_init(UmiDigitalTokenBalance *value, const char *account_id, const char *asset_id, int64_t available_units, int64_t reserved_units, int32_t scale)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || available_units < 0 || reserved_units < 0 || reserved_units > available_units || scale < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_digital_asset_copy_text(value->account_id.value, sizeof value->account_id.value, account_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_digital_asset_copy_text(value->asset_id.value, sizeof value->asset_id.value, asset_id) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    value->available_units = available_units;
    value->reserved_units = reserved_units;
    value->scale = scale;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_token_balance_valid(const UmiDigitalTokenBalance *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->account_id.value, '\0', sizeof(value->account_id.value)) == NULL) return 0;
    if (memchr(value->asset_id.value, '\0', sizeof(value->asset_id.value)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->account_id.value) && umi_digital_asset_text_valid(value->asset_id.value) && value->available_units >= value->reserved_units && value->scale >= 0);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalTokenBalanceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xda11d9bd1609dece);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalTokenBalance *)0)->account_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalTokenBalance *)0)->asset_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalTokenBalanceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalTokenBalance *)0)->account_id.value) - 1U +
        8U + sizeof(((UmiDigitalTokenBalance *)0)->asset_id.value) - 1U +
        8U +
        8U +
        8U;
}
static void UmiDigitalTokenBalanceArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalTokenBalance *value)
{
    UmiArchiveWriteText(writer, value->account_id.value, sizeof(value->account_id.value));
    UmiArchiveWriteText(writer, value->asset_id.value, sizeof(value->asset_id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->available_units);
    UmiArchiveWriteSigned(writer, (int64_t)value->reserved_units);
    UmiArchiveWriteSigned(writer, (int64_t)value->scale);
}
static void UmiDigitalTokenBalanceArchiveRead(UmiArchiveReader *reader, UmiDigitalTokenBalance *value)
{
    UmiArchiveReadText(reader, value->account_id.value, sizeof(value->account_id.value));
    UmiArchiveReadText(reader, value->asset_id.value, sizeof(value->asset_id.value));
    value->available_units = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->reserved_units = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->scale = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
}
static UmiStatus UmiDigitalTokenBalanceArchiveValidate(const UmiDigitalTokenBalance *value)
{
    return umi_digital_asset_token_balance_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_token_balance_archive_encode, umi_digital_asset_token_balance_archive_decode,
    UmiDigitalTokenBalance, UmiDigitalTokenBalanceArchiveSchema, UmiDigitalTokenBalanceArchiveBound, UmiDigitalTokenBalanceArchiveWrite, UmiDigitalTokenBalanceArchiveRead, UmiDigitalTokenBalanceArchiveValidate)
