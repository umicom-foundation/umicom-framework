/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_keyboard_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for required command and navigation keyboard coverage for workstation surfaces.
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
#include "umicom/frontend/conformance/keyboard_contract.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/frontend/conformance/keyboard_contract.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFcKeyboardContractTransferEqual(const UmiFcKeyboardContract *a, const UmiFcKeyboardContract *b)
{
    return a->command_count == b->command_count &&
        a->navigation_count == b->navigation_count &&
        a->shortcuts_documented == b->shortcuts_documented;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFcKeyboardContractTransferTails(UmiFcKeyboardContract *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFcKeyboardContractTransferMalformed(const UmiFcKeyboardContract *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFcKeyboardContractTransferCases, UmiFcKeyboardContract,
    umi_fc_keyboard_contract_archive_encode, umi_fc_keyboard_contract_archive_decode,
    UmiFcKeyboardContractTransferEqual, UmiFcKeyboardContractTransferTails, UmiFcKeyboardContractTransferMalformed)

int main(void) {
    UmiFcKeyboardContract x={5U,2U,true}; CHECK(umi_fc_keyboard_contract_validate(&x));
    if (UmiFcKeyboardContractTransferCases(&x) != 0) return 1;

    return 0;
}
