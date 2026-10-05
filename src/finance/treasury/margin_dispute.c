/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/margin_dispute.c
 *
 * PURPOSE:
 *   Implement track margin dispute amount and resolution state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/margin_dispute.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury margin dispute from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_margin_dispute_init(UmiTreasuryMarginDispute *value,
    const char *id,
    int64_t disputed_minor,
    int64_t resolved_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->disputed_minor=disputed_minor;
    value->resolved_minor=resolved_minor;
    return umi_treasury_margin_dispute_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury margin dispute satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_margin_dispute_valid(const UmiTreasuryMarginDispute *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->disputed_minor >= 0 && value->resolved_minor >= 0 && value->resolved_minor <= value->disputed_minor);
}

/*
 * Provide the treasury margin dispute outstanding minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_margin_dispute_outstanding_minor(const UmiTreasuryMarginDispute *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->disputed_minor - value->resolved_minor;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryMarginDisputeArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xa349b52fb5d6ddea);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryMarginDispute *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryMarginDisputeArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryMarginDispute *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryMarginDisputeArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryMarginDispute *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->disputed_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->resolved_minor);
}
static void UmiTreasuryMarginDisputeArchiveRead(UmiArchiveReader *reader, UmiTreasuryMarginDispute *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->disputed_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->resolved_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryMarginDisputeArchiveValidate(const UmiTreasuryMarginDispute *value)
{
    return umi_treasury_margin_dispute_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_margin_dispute_archive_encode, umi_treasury_margin_dispute_archive_decode,
    UmiTreasuryMarginDispute, UmiTreasuryMarginDisputeArchiveSchema, UmiTreasuryMarginDisputeArchiveBound, UmiTreasuryMarginDisputeArchiveWrite, UmiTreasuryMarginDisputeArchiveRead, UmiTreasuryMarginDisputeArchiveValidate)
