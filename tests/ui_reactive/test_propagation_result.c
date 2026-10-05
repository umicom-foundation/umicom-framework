/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_propagation_result.c
 *
 * PURPOSE:
 *   Exercise the propagation result reactive UI contract.
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
#include "umicom/ui/reactive/propagation_result.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/propagation_result.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactivePropagationResultTransferEqual(const UmiUiReactivePropagationResult *a, const UmiUiReactivePropagationResult *b)
{
    return a->propagated == b->propagated &&
        a->skipped == b->skipped &&
        a->failed == b->failed &&
        a->generation == b->generation;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactivePropagationResultTransferTails(UmiUiReactivePropagationResult *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactivePropagationResultTransferMalformed(const UmiUiReactivePropagationResult *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactivePropagationResultTransferCases, UmiUiReactivePropagationResult,
    umi_ui_reactive_propagation_result_archive_encode, umi_ui_reactive_propagation_result_archive_decode,
    UmiUiReactivePropagationResultTransferEqual, UmiUiReactivePropagationResultTransferTails, UmiUiReactivePropagationResultTransferMalformed)

int main(void) { UmiUiReactivePropagationResult item; umi_ui_reactive_propagation_result_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactivePropagationResult populated = item;
    populated.propagated = (size_t)2;
    populated.skipped = (size_t)3;
    populated.failed = (size_t)4;
    populated.generation = (uint64_t)5;
    if (UmiUiReactivePropagationResultTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_propagation_result_valid(&item) ? 0 : 1; }
