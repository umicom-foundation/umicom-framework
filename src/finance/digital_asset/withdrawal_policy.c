/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/withdrawal_policy.c
 *
 * PURPOSE:
 *   Implement daily withdrawal and approval thresholds for a custody account.
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

#include "umicom/finance/digital_asset/withdrawal_policy.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_withdrawal_policy_init(UmiDigitalWithdrawalPolicy *value, const char *account_id, int64_t daily_limit_units, int64_t approval_threshold_units, int32_t scale, bool address_verification_required)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || daily_limit_units < 0 || approval_threshold_units < 0 || approval_threshold_units > daily_limit_units || scale < 0) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->account_id.value, sizeof value->account_id.value, account_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->daily_limit_units = daily_limit_units;
    value->approval_threshold_units = approval_threshold_units;
    value->scale = scale;
    value->address_verification_required = address_verification_required;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_withdrawal_policy_valid(const UmiDigitalWithdrawalPolicy *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->account_id.value, '\0', sizeof(value->account_id.value)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->account_id.value) && value->approval_threshold_units <= value->daily_limit_units && value->scale >= 0 && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalWithdrawalPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x36388b3d6155570d);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalWithdrawalPolicy *)0)->account_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalWithdrawalPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalWithdrawalPolicy *)0)->account_id.value) - 1U +
        8U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDigitalWithdrawalPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalWithdrawalPolicy *value)
{
    UmiArchiveWriteText(writer, value->account_id.value, sizeof(value->account_id.value));
    UmiArchiveWriteSigned(writer, (int64_t)value->daily_limit_units);
    UmiArchiveWriteSigned(writer, (int64_t)value->approval_threshold_units);
    UmiArchiveWriteSigned(writer, (int64_t)value->scale);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->address_verification_required);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiDigitalWithdrawalPolicyArchiveRead(UmiArchiveReader *reader, UmiDigitalWithdrawalPolicy *value)
{
    UmiArchiveReadText(reader, value->account_id.value, sizeof(value->account_id.value));
    value->daily_limit_units = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->approval_threshold_units = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->scale = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->address_verification_required = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDigitalWithdrawalPolicyArchiveValidate(const UmiDigitalWithdrawalPolicy *value)
{
    return umi_digital_asset_withdrawal_policy_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_withdrawal_policy_archive_encode, umi_digital_asset_withdrawal_policy_archive_decode,
    UmiDigitalWithdrawalPolicy, UmiDigitalWithdrawalPolicyArchiveSchema, UmiDigitalWithdrawalPolicyArchiveBound, UmiDigitalWithdrawalPolicyArchiveWrite, UmiDigitalWithdrawalPolicyArchiveRead, UmiDigitalWithdrawalPolicyArchiveValidate)
