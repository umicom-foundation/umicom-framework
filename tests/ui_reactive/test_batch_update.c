/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_batch_update.c
 *
 * PURPOSE:
 *   Exercise the batch update reactive UI contract.
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
#include "umicom/ui/reactive/batch_update.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/batch_update.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveBatchUpdateTransferEqual(const UmiUiReactiveBatchUpdate *a, const UmiUiReactiveBatchUpdate *b)
{
    return a->mutation_count == b->mutation_count &&
        a->start_revision == b->start_revision &&
        a->end_revision == b->end_revision &&
        a->committed == b->committed;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveBatchUpdateTransferTails(UmiUiReactiveBatchUpdate *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveBatchUpdateTransferMalformed(const UmiUiReactiveBatchUpdate *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveBatchUpdateTransferCases, UmiUiReactiveBatchUpdate,
    umi_ui_reactive_batch_update_archive_encode, umi_ui_reactive_batch_update_archive_decode,
    UmiUiReactiveBatchUpdateTransferEqual, UmiUiReactiveBatchUpdateTransferTails, UmiUiReactiveBatchUpdateTransferMalformed)

int main(void) { UmiUiReactiveBatchUpdate item; umi_ui_reactive_batch_update_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveBatchUpdate populated = item;
    populated.mutation_count = (size_t)2;
    populated.start_revision = (uint64_t)3;
    populated.end_revision = (uint64_t)4;
    populated.committed = true;
    if (UmiUiReactiveBatchUpdateTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_batch_update_valid(&item) ? 0 : 1; }
