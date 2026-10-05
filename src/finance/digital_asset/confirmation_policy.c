/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/digital_asset/confirmation_policy.c
 *
 * PURPOSE:
 *   Implement network confirmation thresholds for provisional and final settlement.
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

#include "umicom/finance/digital_asset/confirmation_policy.h"
#include "../../base/value_archive_internal.h"

#include <string.h>

/* Initialise the record without allocating memory or retaining caller buffers. */
UmiStatus umi_digital_asset_confirmation_policy_init(UmiDigitalConfirmationPolicy *value, const char *network_id, uint32_t required_confirmations, uint32_t final_confirmations)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || final_confirmations < required_confirmations) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    status = umi_digital_asset_copy_text(value->network_id.value, sizeof value->network_id.value, network_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->required_confirmations = required_confirmations;
    value->final_confirmations = final_confirmations;
    value->active = true;
    return UMI_STATUS_OK;
}

/* Keep shared validation deterministic and independent of application UI state. */
bool umi_digital_asset_confirmation_policy_valid(const UmiDigitalConfirmationPolicy *value)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->network_id.value, '\0', sizeof(value->network_id.value)) == NULL) return 0;

    return value != NULL && (umi_digital_asset_text_valid(value->network_id.value) && value->final_confirmations >= value->required_confirmations && value->active);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiDigitalConfirmationPolicyArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x2a3cc13222c46b5e);
    schema = (schema ^ (uint64_t)sizeof(((UmiDigitalConfirmationPolicy *)0)->network_id.value)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiDigitalConfirmationPolicyArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiDigitalConfirmationPolicy *)0)->network_id.value) - 1U +
        8U +
        8U +
        8U;
}
static void UmiDigitalConfirmationPolicyArchiveWrite(UmiArchiveWriter *writer, const UmiDigitalConfirmationPolicy *value)
{
    UmiArchiveWriteText(writer, value->network_id.value, sizeof(value->network_id.value));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->required_confirmations);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->final_confirmations);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->active);
}
static void UmiDigitalConfirmationPolicyArchiveRead(UmiArchiveReader *reader, UmiDigitalConfirmationPolicy *value)
{
    UmiArchiveReadText(reader, value->network_id.value, sizeof(value->network_id.value));
    value->required_confirmations = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->final_confirmations = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->active = (bool)UmiArchiveReadUnsigned(reader, 1U);
}
static UmiStatus UmiDigitalConfirmationPolicyArchiveValidate(const UmiDigitalConfirmationPolicy *value)
{
    return umi_digital_asset_confirmation_policy_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_digital_asset_confirmation_policy_archive_encode, umi_digital_asset_confirmation_policy_archive_decode,
    UmiDigitalConfirmationPolicy, UmiDigitalConfirmationPolicyArchiveSchema, UmiDigitalConfirmationPolicyArchiveBound, UmiDigitalConfirmationPolicyArchiveWrite, UmiDigitalConfirmationPolicyArchiveRead, UmiDigitalConfirmationPolicyArchiveValidate)
