/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/signing_policy.c
 *
 * PURPOSE:
 *   Implement threshold and hardware requirements for governed transaction signing.
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

#include "umicom/finance/digital_asset/signing_policy.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_signing_policy_init(UmiDigitalSigningPolicy *value, const char *id, uint32_t required_approvals, uint32_t available_approvers, bool hardware_required)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || required_approvals == 0U || available_approvers == 0U || required_approvals > available_approvers) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->required_approvals = required_approvals;
    value->available_approvers = available_approvers;
    value->hardware_required = hardware_required;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_signing_policy_valid(const UmiDigitalSigningPolicy *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && value->required_approvals > 0U && value->required_approvals <= value->available_approvers && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalSigningPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xae487e9678297907);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalSigningPolicy *)0)->id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalSigningPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalSigningPolicy *)0)->id.value) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiDigitalSigningPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalSigningPolicy *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required_approvals);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->available_approvers);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->hardware_required);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiDigitalSigningPolicyArchiveRead(UmiArchiveReader *reader, UmiDigitalSigningPolicy *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    value->required_approvals = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->available_approvers = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->hardware_required = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDigitalSigningPolicyArchiveValidate(const UmiDigitalSigningPolicy *value)
{
    return umi_digital_asset_signing_policy_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_signing_policy_archive_encode, umi_digital_asset_signing_policy_archive_decode,
    UmiDigitalSigningPolicy, UmiDigitalSigningPolicyArchiveSchema, UmiDigitalSigningPolicyArchiveBound, UmiDigitalSigningPolicyArchiveWrite, UmiDigitalSigningPolicyArchiveRead, UmiDigitalSigningPolicyArchiveValidate)
