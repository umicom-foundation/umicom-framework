/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_selection_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for single, multiple and range selection semantics for list, tree, grid and editor surfaces.
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
#include "umicom/frontend/conformance/selection_contract.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/frontend/conformance/selection_contract.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFcSelectionContractTransferEqual(const UmiFcSelectionContract *a, const UmiFcSelectionContract *b)
{
    return a->required_modes == b->required_modes &&
        a->keyboard_extend == b->keyboard_extend &&
        a->preserve_on_refresh == b->preserve_on_refresh;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFcSelectionContractTransferTails(UmiFcSelectionContract *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFcSelectionContractTransferMalformed(const UmiFcSelectionContract *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFcSelectionContractTransferCases, UmiFcSelectionContract,
    umi_fc_selection_contract_archive_encode, umi_fc_selection_contract_archive_decode,
    UmiFcSelectionContractTransferEqual, UmiFcSelectionContractTransferTails, UmiFcSelectionContractTransferMalformed)

int main(void) {
    UmiFcSelectionContract x={3U,true,true}; CHECK(umi_fc_selection_contract_validate(&x));
    if (UmiFcSelectionContractTransferCases(&x) != 0) return 1;

    return 0;
}
