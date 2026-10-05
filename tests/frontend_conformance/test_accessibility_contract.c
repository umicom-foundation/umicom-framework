/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_accessibility_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for required semantic accessibility roles, names, states and keyboard affordances.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/frontend/conformance/accessibility_contract.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/frontend/conformance/accessibility_contract.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFcAccessibilityContractTransferEqual(const UmiFcAccessibilityContract *a, const UmiFcAccessibilityContract *b)
{
    return a->required_roles == b->required_roles &&
        a->named == b->named &&
        a->keyboard_reachable == b->keyboard_reachable &&
        a->state_exposed == b->state_exposed;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFcAccessibilityContractTransferTails(UmiFcAccessibilityContract *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFcAccessibilityContractTransferMalformed(const UmiFcAccessibilityContract *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFcAccessibilityContractTransferCases, UmiFcAccessibilityContract,
    umi_fc_accessibility_contract_archive_encode, umi_fc_accessibility_contract_archive_decode,
    UmiFcAccessibilityContractTransferEqual, UmiFcAccessibilityContractTransferTails, UmiFcAccessibilityContractTransferMalformed)

int main(void) {
    UmiFcAccessibilityContract x={1U,true,true,true}; CHECK(umi_fc_accessibility_contract_validate(&x));
    if (UmiFcAccessibilityContractTransferCases(&x) != 0) return 1;

    return 0;
}
