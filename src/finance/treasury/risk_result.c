/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/risk_result.c
 *
 * PURPOSE:
 *   Implement record observed, stressed and limit risk values for governance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_result.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury risk result from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_risk_result_init(UmiTreasuryRiskResult *value,
    const char *id,
    int64_t observed_minor,
    int64_t stressed_minor,
    int64_t limit_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->observed_minor=observed_minor;
    value->stressed_minor=stressed_minor;
    value->limit_minor=limit_minor;
    return umi_treasury_risk_result_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury risk result satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_risk_result_valid(const UmiTreasuryRiskResult *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->limit_minor >= 0);
}

/*
 * Provide the treasury risk result within limit operation used by this module and its
 * client applications.
 */
bool umi_treasury_risk_result_within_limit(const UmiTreasuryRiskResult *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return umi_treasury_abs_i64(value->stressed_minor) <= value->limit_minor;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryRiskResultArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x796922fc15051c6d);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryRiskResult *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryRiskResultArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryRiskResult *)0)->id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTreasuryRiskResultArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryRiskResult *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->observed_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->stressed_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->limit_minor);
}
static void UmiTreasuryRiskResultArchiveRead(UmiArchiveReader *reader, UmiTreasuryRiskResult *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->observed_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->stressed_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->limit_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryRiskResultArchiveValidate(const UmiTreasuryRiskResult *value)
{
    return umi_treasury_risk_result_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_risk_result_archive_encode, umi_treasury_risk_result_archive_decode,
    UmiTreasuryRiskResult, UmiTreasuryRiskResultArchiveSchema, UmiTreasuryRiskResultArchiveBound, UmiTreasuryRiskResultArchiveWrite, UmiTreasuryRiskResultArchiveRead, UmiTreasuryRiskResultArchiveValidate)
