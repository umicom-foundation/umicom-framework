/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/treasury_snapshot.c
 *
 * PURPOSE:
 *   Implement capture aggregate cash, liquidity, risk and collateral state at a point in time.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/treasury_snapshot.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury treasury snapshot from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_treasury_snapshot_init(UmiTreasuryTreasurySnapshot *value,
    const char *id,
    int64_t cash_minor,
    int64_t liquidity_gap_minor,
    int64_t risk_minor,
    int64_t collateral_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->cash_minor=cash_minor;
    value->liquidity_gap_minor=liquidity_gap_minor;
    value->risk_minor=risk_minor;
    value->collateral_minor=collateral_minor;
    return umi_treasury_treasury_snapshot_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury treasury snapshot satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_treasury_snapshot_valid(const UmiTreasuryTreasurySnapshot *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->collateral_minor >= 0);
}

/*
 * Provide the treasury treasury snapshot net liquidity minor operation used by this module
 * and its client applications.
 */
int64_t umi_treasury_treasury_snapshot_net_liquidity_minor(const UmiTreasuryTreasurySnapshot *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->cash_minor + value->liquidity_gap_minor;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryTreasurySnapshotArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb18916d9b67562e5);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryTreasurySnapshot *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryTreasurySnapshotArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryTreasurySnapshot *)0)->id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiTreasuryTreasurySnapshotArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryTreasurySnapshot *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->cash_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->liquidity_gap_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->risk_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->collateral_minor);
}
static void UmiTreasuryTreasurySnapshotArchiveRead(UmiArchiveReader *reader, UmiTreasuryTreasurySnapshot *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->cash_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->liquidity_gap_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->risk_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->collateral_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryTreasurySnapshotArchiveValidate(const UmiTreasuryTreasurySnapshot *value)
{
    return umi_treasury_treasury_snapshot_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_treasury_snapshot_archive_encode, umi_treasury_treasury_snapshot_archive_decode,
    UmiTreasuryTreasurySnapshot, UmiTreasuryTreasurySnapshotArchiveSchema, UmiTreasuryTreasurySnapshotArchiveBound, UmiTreasuryTreasurySnapshotArchiveWrite, UmiTreasuryTreasurySnapshotArchiveRead, UmiTreasuryTreasurySnapshotArchiveValidate)
