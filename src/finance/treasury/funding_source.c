/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/funding_source.c
 *
 * PURPOSE:
 *   Implement model a funding facility with capacity, drawn amount and cost.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/funding_source.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury funding source from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_funding_source_init(UmiTreasuryFundingSource *value,
    const char *id,
    int64_t capacity_minor,
    int64_t drawn_minor,
    int32_t spread_bps) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->capacity_minor=capacity_minor;
    value->drawn_minor=drawn_minor;
    value->spread_bps=spread_bps;
    return umi_treasury_funding_source_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury funding source satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_funding_source_valid(const UmiTreasuryFundingSource *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->capacity_minor >= 0 && value->drawn_minor >= 0 && value->drawn_minor <= value->capacity_minor && value->spread_bps >= 0);
}

/*
 * Provide the treasury funding source available minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_funding_source_available_minor(const UmiTreasuryFundingSource *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->capacity_minor - value->drawn_minor;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryFundingSourceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x73b2a9ba925a36f9);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryFundingSource *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryFundingSourceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryFundingSource *)0)->id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTreasuryFundingSourceArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryFundingSource *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->capacity_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->drawn_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->spread_bps);
}
static void UmiTreasuryFundingSourceArchiveRead(UmiArchiveReader *reader, UmiTreasuryFundingSource *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->capacity_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->drawn_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->spread_bps = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
}
static UmiStatus UmiTreasuryFundingSourceArchiveValidate(const UmiTreasuryFundingSource *value)
{
    return umi_treasury_funding_source_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_funding_source_archive_encode, umi_treasury_funding_source_archive_decode,
    UmiTreasuryFundingSource, UmiTreasuryFundingSourceArchiveSchema, UmiTreasuryFundingSourceArchiveBound, UmiTreasuryFundingSourceArchiveWrite, UmiTreasuryFundingSourceArchiveRead, UmiTreasuryFundingSourceArchiveValidate)
