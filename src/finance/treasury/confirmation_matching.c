/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/confirmation_matching.c
 *
 * PURPOSE:
 *   Implement score confirmation field matching and expose exact-match status.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/confirmation_matching.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury confirmation matching from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_confirmation_matching_init(UmiTreasuryConfirmationMatching *value,
    const char *id,
    uint32_t matched_fields,
    uint32_t total_fields) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->matched_fields=matched_fields;
    value->total_fields=total_fields;
    return umi_treasury_confirmation_matching_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury confirmation matching satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_confirmation_matching_valid(const UmiTreasuryConfirmationMatching *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->total_fields > 0U && value->matched_fields <= value->total_fields);
}

/*
 * Provide the treasury confirmation matching exact operation used by this module and its
 * client applications.
 */
bool umi_treasury_confirmation_matching_exact(const UmiTreasuryConfirmationMatching *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return value->matched_fields == value->total_fields;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryConfirmationMatchingArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8f2ef87dc8554ab8);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryConfirmationMatching *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryConfirmationMatchingArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryConfirmationMatching *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryConfirmationMatchingArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryConfirmationMatching *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->matched_fields);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->total_fields);
}
static void UmiTreasuryConfirmationMatchingArchiveRead(UmiArchiveReader *reader, UmiTreasuryConfirmationMatching *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->matched_fields = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->total_fields = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiTreasuryConfirmationMatchingArchiveValidate(const UmiTreasuryConfirmationMatching *value)
{
    return umi_treasury_confirmation_matching_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_confirmation_matching_archive_encode, umi_treasury_confirmation_matching_archive_decode,
    UmiTreasuryConfirmationMatching, UmiTreasuryConfirmationMatchingArchiveSchema, UmiTreasuryConfirmationMatchingArchiveBound, UmiTreasuryConfirmationMatchingArchiveWrite, UmiTreasuryConfirmationMatchingArchiveRead, UmiTreasuryConfirmationMatchingArchiveValidate)
