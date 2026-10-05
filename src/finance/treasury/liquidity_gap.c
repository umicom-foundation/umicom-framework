/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/liquidity_gap.c
 *
 * PURPOSE:
 *   Implement represent a currency liquidity mismatch for a defined horizon.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/liquidity_gap.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury liquidity gap from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_liquidity_gap_init(UmiTreasuryLiquidityGap *value,
    const char *id,
    int32_t horizon_days,
    int64_t inflow_minor,
    int64_t outflow_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->horizon_days=horizon_days;
    value->inflow_minor=inflow_minor;
    value->outflow_minor=outflow_minor;
    return umi_treasury_liquidity_gap_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury liquidity gap satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_liquidity_gap_valid(const UmiTreasuryLiquidityGap *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->horizon_days >= 0 && value->inflow_minor >= 0 && value->outflow_minor >= 0);
}

/*
 * Provide the treasury liquidity gap net minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_liquidity_gap_net_minor(const UmiTreasuryLiquidityGap *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->inflow_minor - value->outflow_minor;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryLiquidityGapArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xf17683f6def67895);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryLiquidityGap *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryLiquidityGapArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryLiquidityGap *)0)->id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiTreasuryLiquidityGapArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryLiquidityGap *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->horizon_days);
    UmiArchiveWriteSigned(writer, (int64_t)value->inflow_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->outflow_minor);
}
static void UmiTreasuryLiquidityGapArchiveRead(UmiArchiveReader *reader, UmiTreasuryLiquidityGap *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->horizon_days = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
    value->inflow_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->outflow_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryLiquidityGapArchiveValidate(const UmiTreasuryLiquidityGap *value)
{
    return umi_treasury_liquidity_gap_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_liquidity_gap_archive_encode, umi_treasury_liquidity_gap_archive_decode,
    UmiTreasuryLiquidityGap, UmiTreasuryLiquidityGapArchiveSchema, UmiTreasuryLiquidityGapArchiveBound, UmiTreasuryLiquidityGapArchiveWrite, UmiTreasuryLiquidityGapArchiveRead, UmiTreasuryLiquidityGapArchiveValidate)
