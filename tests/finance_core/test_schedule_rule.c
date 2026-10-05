/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_schedule_rule.c
 *
 * PURPOSE:
 *   Exercise the schedule rule financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <string.h>
#include "umicom/finance/core/schedule_rule.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/core/schedule_rule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiScheduleRuleTransferEqual(const UmiScheduleRule *a, const UmiScheduleRule *b)
{
    return a->start.year == b->start.year &&
        a->start.month == b->start.month &&
        a->start.day == b->start.day &&
        a->end.year == b->end.year &&
        a->end.month == b->end.month &&
        a->end.day == b->end.day &&
        a->frequency.amount == b->frequency.amount &&
        a->frequency.unit == b->frequency.unit &&
        a->convention == b->convention;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiScheduleRuleTransferTails(UmiScheduleRule *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiScheduleRuleTransferMalformed(const UmiScheduleRule *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiScheduleRuleTransferCases, UmiScheduleRule,
    umi_schedule_rule_archive_encode, umi_schedule_rule_archive_decode,
    UmiScheduleRuleTransferEqual, UmiScheduleRuleTransferTails, UmiScheduleRuleTransferMalformed)

int main(void)
{
    UmiScheduleRule r={(UmiFinancialDate){2026,1U,1U},(UmiFinancialDate){2027,1U,1U},{3U,UMI_TENOR_MONTHS},UMI_BUSINESS_DAY_FOLLOWING}; CHECK(umi_schedule_rule_is_valid(&r));
    if (UmiScheduleRuleTransferCases(&r) != 0) return 1;

    return 0;
}
