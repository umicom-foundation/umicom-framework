/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/liquidity_risk_snapshot.c
 *
 * PURPOSE:
 *   Implement capture liquidity-risk gap and liquidity buffer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/liquidity_risk_snapshot.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury liquidity risk snapshot from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_treasury_liquidity_risk_snapshot_init(UmiTreasuryLiquidityRiskSnapshot *value,
    const char *id,
    int64_t primary_minor,
    int64_t secondary_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->risk_class=UMI_TREASURY_RISK_LIQUIDITY;
    value->primary_minor=primary_minor;
    value->secondary_minor=secondary_minor;
    return umi_treasury_liquidity_risk_snapshot_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury liquidity risk snapshot satisfies its contract before another
 * service relies on it.
 */
bool umi_treasury_liquidity_risk_snapshot_valid(const UmiTreasuryLiquidityRiskSnapshot *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id));
}

/*
 * Provide the treasury liquidity risk snapshot combined absolute minor operation used by
 * this module and its client applications.
 */
int64_t umi_treasury_liquidity_risk_snapshot_combined_absolute_minor(const UmiTreasuryLiquidityRiskSnapshot *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return umi_treasury_abs_i64(value->primary_minor) + umi_treasury_abs_i64(value->secondary_minor);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryLiquidityRiskSnapshotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xba926faa0cfe6627);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryLiquidityRiskSnapshot *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryLiquidityRiskSnapshotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryLiquidityRiskSnapshot *)0)->id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTreasuryLiquidityRiskSnapshotArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryLiquidityRiskSnapshot *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->risk_class);
    UmiArchiveWriteSigned(writer, (int64_t)value->primary_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->secondary_minor);
}
static void UmiTreasuryLiquidityRiskSnapshotArchiveRead(UmiArchiveReader *reader, UmiTreasuryLiquidityRiskSnapshot *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->risk_class = (UmiTreasuryRiskClass)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->primary_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->secondary_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryLiquidityRiskSnapshotArchiveValidate(const UmiTreasuryLiquidityRiskSnapshot *value)
{
    return umi_treasury_liquidity_risk_snapshot_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_liquidity_risk_snapshot_archive_encode, umi_treasury_liquidity_risk_snapshot_archive_decode,
    UmiTreasuryLiquidityRiskSnapshot, UmiTreasuryLiquidityRiskSnapshotArchiveSchema, UmiTreasuryLiquidityRiskSnapshotArchiveBound, UmiTreasuryLiquidityRiskSnapshotArchiveWrite, UmiTreasuryLiquidityRiskSnapshotArchiveRead, UmiTreasuryLiquidityRiskSnapshotArchiveValidate)
