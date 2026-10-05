/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/cash_sweep_plan.c
 *
 * PURPOSE:
 *   Implement represent an executable cash sweep amount subject to a maximum.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/cash_sweep_plan.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise treasury cash sweep plan from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_cash_sweep_plan_init(UmiTreasuryCashSweepPlan *value,
    const char *id,
    int64_t requested_minor,
    int64_t maximum_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->requested_minor=requested_minor;
    value->maximum_minor=maximum_minor;
    return umi_treasury_cash_sweep_plan_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury cash sweep plan satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_cash_sweep_plan_valid(const UmiTreasuryCashSweepPlan *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return 0;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return 0;

    return value != NULL && (umi_treasury_id_valid(value->id) && value->requested_minor >= 0 && value->maximum_minor >= 0);
}

/*
 * Provide the treasury cash sweep plan executable minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_cash_sweep_plan_executable_minor(const UmiTreasuryCashSweepPlan *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->requested_minor < value->maximum_minor ? value->requested_minor : value->maximum_minor;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiTreasuryCashSweepPlanArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xb19beb096d68ef13);
    schema = (schema ^ (uint64_t)sizeof(((UmiTreasuryCashSweepPlan *)0)->id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiTreasuryCashSweepPlanArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiTreasuryCashSweepPlan *)0)->id) - 1U +
        8U +
        8U;
}
static void UmiTreasuryCashSweepPlanArchiveWrite(UmiArchiveWriter *writer, const UmiTreasuryCashSweepPlan *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteSigned(writer, (int64_t)value->requested_minor);
    UmiArchiveWriteSigned(writer, (int64_t)value->maximum_minor);
}
static void UmiTreasuryCashSweepPlanArchiveRead(UmiArchiveReader *reader, UmiTreasuryCashSweepPlan *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    value->requested_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
    value->maximum_minor = (int64_t)UmiArchiveReadSigned(reader, INT64_MIN, INT64_MAX);
}
static UmiStatus UmiTreasuryCashSweepPlanArchiveValidate(const UmiTreasuryCashSweepPlan *value)
{
    return umi_treasury_cash_sweep_plan_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_treasury_cash_sweep_plan_archive_encode, umi_treasury_cash_sweep_plan_archive_decode,
    UmiTreasuryCashSweepPlan, UmiTreasuryCashSweepPlanArchiveSchema, UmiTreasuryCashSweepPlanArchiveBound, UmiTreasuryCashSweepPlanArchiveWrite, UmiTreasuryCashSweepPlanArchiveRead, UmiTreasuryCashSweepPlanArchiveValidate)
