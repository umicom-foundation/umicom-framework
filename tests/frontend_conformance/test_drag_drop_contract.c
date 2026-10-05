/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_drag_drop_contract.c
 *
 * PURPOSE:
 *   Focused regression coverage for semantic drag/drop operation, keyboard alternative and docking affordance requirements.
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
#include "umicom/frontend/conformance/drag_drop_contract.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/frontend/conformance/drag_drop_contract.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiFcDragDropContractTransferEqual(const UmiFcDragDropContract *a, const UmiFcDragDropContract *b)
{
    return a->required_ops == b->required_ops &&
        a->keyboard_alternative == b->keyboard_alternative &&
        a->visual_preview == b->visual_preview;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiFcDragDropContractTransferTails(UmiFcDragDropContract *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiFcDragDropContractTransferMalformed(const UmiFcDragDropContract *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiFcDragDropContractTransferCases, UmiFcDragDropContract,
    umi_fc_drag_drop_contract_archive_encode, umi_fc_drag_drop_contract_archive_decode,
    UmiFcDragDropContractTransferEqual, UmiFcDragDropContractTransferTails, UmiFcDragDropContractTransferMalformed)

int main(void) {
    UmiFcDragDropContract x={3U,true,true}; CHECK(umi_fc_drag_drop_contract_validate(&x));
    if (UmiFcDragDropContractTransferCases(&x) != 0) return 1;

    return 0;
}
