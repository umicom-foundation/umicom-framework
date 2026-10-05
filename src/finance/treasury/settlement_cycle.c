/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/settlement_cycle.c
 *
 * PURPOSE:
 *   Implement define settlement cycle trade-date and settlement-date offsets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_cycle.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury settlement cycle from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_settlement_cycle_init(UmiTreasurySettlementCycle *value,
    const char *id,
    int32_t settlement_days) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->settlement_days=settlement_days;
    return umi_treasury_settlement_cycle_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury settlement cycle satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_cycle_valid(const UmiTreasurySettlementCycle *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->settlement_days >= 0 && value->settlement_days <= 30);
}

/*
 * Provide the treasury settlement cycle offset days operation used by this module and its
 * client applications.
 */
int32_t umi_treasury_settlement_cycle_offset_days(const UmiTreasurySettlementCycle *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int32_t)0;
    return value->settlement_days;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasurySettlementCycleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd8fd968e8c03a99f);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasurySettlementCycle *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasurySettlementCycleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasurySettlementCycle *)0)->id) - 1U +
        8U;
}
static void UmiTreasurySettlementCycleArchiveWrite(UmiArchiveWriter *writer, const UmiTreasurySettlementCycle *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->settlement_days);
}
static void UmiTreasurySettlementCycleArchiveRead(UmiArchiveReader *reader, UmiTreasurySettlementCycle *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->settlement_days = (int32_t)UmiArchiveReadSigned(reader, INT32_MIN, INT32_MAX);
}
static UmiStatus UmiTreasurySettlementCycleArchiveValidate(const UmiTreasurySettlementCycle *value)
{
    return umi_treasury_settlement_cycle_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_settlement_cycle_archive_encode, umi_treasury_settlement_cycle_archive_decode,
    UmiTreasurySettlementCycle, UmiTreasurySettlementCycleArchiveSchema, UmiTreasurySettlementCycleArchiveBound, UmiTreasurySettlementCycleArchiveWrite, UmiTreasurySettlementCycleArchiveRead, UmiTreasurySettlementCycleArchiveValidate)
