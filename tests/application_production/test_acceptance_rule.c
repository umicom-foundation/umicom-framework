/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_production/test_acceptance_rule.c
 *
 * PURPOSE:
 *   Implement the test acceptance rule behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework application production test | acceptance_rule | Sammy Hegab | Umicom Foundation | MIT */
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include "test_fixture.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/application/production/acceptance_rule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiApplicationProductionAcceptanceRuleTransferEqual(const UmiApplicationProductionAcceptanceRule *a, const UmiApplicationProductionAcceptanceRule *b)
{
    return a->require_manifest == b->require_manifest &&
        a->require_layout_projection == b->require_layout_projection &&
        a->require_capabilities == b->require_capabilities &&
        a->require_tests == b->require_tests &&
        a->require_evidence == b->require_evidence &&
        a->allow_degraded_optional_capabilities == b->allow_degraded_optional_capabilities;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiApplicationProductionAcceptanceRuleTransferTails(UmiApplicationProductionAcceptanceRule *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiApplicationProductionAcceptanceRuleTransferMalformed(const UmiApplicationProductionAcceptanceRule *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiApplicationProductionAcceptanceRuleTransferCases, UmiApplicationProductionAcceptanceRule,
    umi_application_production_acceptance_rule_archive_encode, umi_application_production_acceptance_rule_archive_decode,
    UmiApplicationProductionAcceptanceRuleTransferEqual, UmiApplicationProductionAcceptanceRuleTransferTails, UmiApplicationProductionAcceptanceRuleTransferMalformed)

int main(void) {
    UmiApplicationProductionAcceptanceRule rule = umi_application_production_acceptance_rule_default();
    assert(umi_application_production_acceptance_rule_validate(&rule) == UMI_STATUS_OK);
    if (UmiApplicationProductionAcceptanceRuleTransferCases(&rule) != 0) return 1;

    assert(rule.require_manifest && rule.require_evidence);
    return 0;
}

