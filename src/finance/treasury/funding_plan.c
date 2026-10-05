/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/funding_plan.c
 *
 * PURPOSE:
 *   Implement represent a funded amount and enforce that allocations do not exceed requirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/funding_plan.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury funding plan from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_funding_plan_init(UmiTreasuryFundingPlan *value,
    const char *id,
    int64_t requirement_minor,
    int64_t allocated_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->requirement_minor=requirement_minor;
    value->allocated_minor=allocated_minor;
    return umi_treasury_funding_plan_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury funding plan satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_funding_plan_valid(const UmiTreasuryFundingPlan *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->requirement_minor >= 0 && value->allocated_minor >= 0 && value->allocated_minor <= value->requirement_minor);
}

/*
 * Provide the treasury funding plan remaining minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_funding_plan_remaining_minor(const UmiTreasuryFundingPlan *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->requirement_minor - value->allocated_minor;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryFundingPlanArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8a1d7f951f8c4ee2);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryFundingPlan *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryFundingPlanArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryFundingPlan *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryFundingPlanArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryFundingPlan *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->requirement_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->allocated_minor);
}
static void UmiTreasuryFundingPlanArchiveRead(UmiArchiveReader *reader, UmiTreasuryFundingPlan *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->requirement_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->allocated_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryFundingPlanArchiveValidate(const UmiTreasuryFundingPlan *value)
{
    return umi_treasury_funding_plan_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_funding_plan_archive_encode, umi_treasury_funding_plan_archive_decode,
    UmiTreasuryFundingPlan, UmiTreasuryFundingPlanArchiveSchema, UmiTreasuryFundingPlanArchiveBound, UmiTreasuryFundingPlanArchiveWrite, UmiTreasuryFundingPlanArchiveRead, UmiTreasuryFundingPlanArchiveValidate)
