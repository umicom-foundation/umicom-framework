/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/risk_measure.c
 *
 * PURPOSE:
 *   Implement define a calculated risk measure value, confidence and horizon.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_measure.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury risk measure from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_risk_measure_init(UmiTreasuryRiskMeasure *value,
    const char *id,
    int64_t value_minor,
    uint32_t confidence_bps,
    uint32_t horizon_days) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->value_minor=value_minor;
    value->confidence_bps=confidence_bps;
    value->horizon_days=horizon_days;
    return umi_treasury_risk_measure_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury risk measure satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_risk_measure_valid(const UmiTreasuryRiskMeasure *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->confidence_bps <= 10000U && value->horizon_days > 0U);
}

/*
 * Provide the treasury risk measure absolute value minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_risk_measure_absolute_value_minor(const UmiTreasuryRiskMeasure *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return umi_treasury_abs_i64(value->value_minor);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryRiskMeasureArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x19fddd738631951d);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryRiskMeasure *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryRiskMeasureArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryRiskMeasure *)0)->id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTreasuryRiskMeasureArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryRiskMeasure *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->value_minor);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->confidence_bps);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->horizon_days);
}
static void UmiTreasuryRiskMeasureArchiveRead(UmiArchiveReader *reader, UmiTreasuryRiskMeasure *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->value_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->confidence_bps = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->horizon_days = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiTreasuryRiskMeasureArchiveValidate(const UmiTreasuryRiskMeasure *value)
{
    return umi_treasury_risk_measure_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_risk_measure_archive_encode, umi_treasury_risk_measure_archive_decode,
    UmiTreasuryRiskMeasure, UmiTreasuryRiskMeasureArchiveSchema, UmiTreasuryRiskMeasureArchiveBound, UmiTreasuryRiskMeasureArchiveWrite, UmiTreasuryRiskMeasureArchiveRead, UmiTreasuryRiskMeasureArchiveValidate)
