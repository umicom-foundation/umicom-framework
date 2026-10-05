/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/settlement_repair.c
 *
 * PURPOSE:
 *   Implement represent a settlement repair action with bounded attempt governance.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/settlement_repair.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury settlement repair from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_settlement_repair_init(UmiTreasurySettlementRepair *value,
    const char *id,
    uint32_t attempt,
    uint32_t maximum_attempts) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->attempt=attempt;
    value->maximum_attempts=maximum_attempts;
    return umi_treasury_settlement_repair_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury settlement repair satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_repair_valid(const UmiTreasurySettlementRepair *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->maximum_attempts > 0U && value->attempt <= value->maximum_attempts);
}

/*
 * Provide the treasury settlement repair retry allowed operation used by this module and
 * its client applications.
 */
bool umi_treasury_settlement_repair_retry_allowed(const UmiTreasurySettlementRepair *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return value->attempt < value->maximum_attempts;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasurySettlementRepairArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x80c8e1acd2805b42);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasurySettlementRepair *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasurySettlementRepairArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasurySettlementRepair *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasurySettlementRepairArchiveWrite(UmiArchiveWriter *writer, const UmiTreasurySettlementRepair *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->attempt);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->maximum_attempts);
}
static void UmiTreasurySettlementRepairArchiveRead(UmiArchiveReader *reader, UmiTreasurySettlementRepair *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->attempt = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->maximum_attempts = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiTreasurySettlementRepairArchiveValidate(const UmiTreasurySettlementRepair *value)
{
    return umi_treasury_settlement_repair_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_settlement_repair_archive_encode, umi_treasury_settlement_repair_archive_decode,
    UmiTreasurySettlementRepair, UmiTreasurySettlementRepairArchiveSchema, UmiTreasurySettlementRepairArchiveBound, UmiTreasurySettlementRepairArchiveWrite, UmiTreasurySettlementRepairArchiveRead, UmiTreasurySettlementRepairArchiveValidate)
