/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/signing_request.c
 *
 * PURPOSE:
 *   Implement a governed signing request without carrying private key material.
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

#include "umicom/finance/digital_asset/signing_request.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_signing_request_init(UmiDigitalSigningRequest *value, const char *id, const char *transaction_id, const char *policy_id, uint32_t required_approvals)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || required_approvals == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->id.value, sizeof value->id.value, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->transaction_id.value, sizeof value->transaction_id.value, transaction_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_digital_asset_copy_text(value->policy_id.value, sizeof value->policy_id.value, policy_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->required_approvals = required_approvals;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_signing_request_valid(const UmiDigitalSigningRequest *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id.value, '\0', sizeof(value->id.value)) == NULL) return 0;
    if (memchr(value->transaction_id.value, '\0', sizeof(value->transaction_id.value)) == NULL) return 0;
    if (memchr(value->policy_id.value, '\0', sizeof(value->policy_id.value)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->id.value) && umi_digital_asset_text_valid(value->transaction_id.value) && value->required_approvals > 0U && value->received_approvals <= value->required_approvals);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalSigningRequestArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7c43eccb1acdf23a);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalSigningRequest *)0)->id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalSigningRequest *)0)->transaction_id.value)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalSigningRequest *)0)->policy_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalSigningRequestArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalSigningRequest *)0)->id.value) - 1U +
        8U + sizeof(((UmiDigitalSigningRequest *)0)->transaction_id.value) - 1U +
        8U + sizeof(((UmiDigitalSigningRequest *)0)->policy_id.value) - 1U +
        8U +
        8U +
        8U;
}
static void UmiDigitalSigningRequestArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalSigningRequest *value)
{
    UmiArchiveWriteText(writer, value->id.value, sizeof(value->id.value));
    UmiArchiveWriteText(writer, value->transaction_id.value, sizeof(value->transaction_id.value));
    UmiArchiveWriteText(writer, value->policy_id.value, sizeof(value->policy_id.value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required_approvals);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->received_approvals);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->closed);
}
static void UmiDigitalSigningRequestArchiveRead(UmiArchiveReader *reader, UmiDigitalSigningRequest *value)
{
    UmiArchiveReadText(reader, value->id.value, sizeof(value->id.value));
    UmiArchiveReadText(reader, value->transaction_id.value, sizeof(value->transaction_id.value));
    UmiArchiveReadText(reader, value->policy_id.value, sizeof(value->policy_id.value));
    value->required_approvals = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->received_approvals = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->closed = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDigitalSigningRequestArchiveValidate(const UmiDigitalSigningRequest *value)
{
    return umi_digital_asset_signing_request_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_signing_request_archive_encode, umi_digital_asset_signing_request_archive_decode,
    UmiDigitalSigningRequest, UmiDigitalSigningRequestArchiveSchema, UmiDigitalSigningRequestArchiveBound, UmiDigitalSigningRequestArchiveWrite, UmiDigitalSigningRequestArchiveRead, UmiDigitalSigningRequestArchiveValidate)
