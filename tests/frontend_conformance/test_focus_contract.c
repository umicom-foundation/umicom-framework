/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_focus_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for focusable-element ordering and focus-trap requirements for interactive surfaces.
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
#include "umicom/frontend/conformance/focus_contract.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/frontend/conformance/focus_contract.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFcFocusContractTransferEqual(const UmiFcFocusContract *a, const UmiFcFocusContract *b)
{
    return a->focusable_count == b->focusable_count &&
        a->traversal_count == b->traversal_count &&
        a->modal_trap_required == b->modal_trap_required;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFcFocusContractTransferTails(UmiFcFocusContract *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFcFocusContractTransferMalformed(const UmiFcFocusContract *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFcFocusContractTransferCases, UmiFcFocusContract,
    umi_fc_focus_contract_archive_encode, umi_fc_focus_contract_archive_decode,
    UmiFcFocusContractTransferEqual, UmiFcFocusContractTransferTails, UmiFcFocusContractTransferMalformed)

int main(void) {
    UmiFcFocusContract x={4U,4U,true}; CHECK(umi_fc_focus_contract_validate(&x));
    if (UmiFcFocusContractTransferCases(&x) != 0) return 1;

    return 0;
}
