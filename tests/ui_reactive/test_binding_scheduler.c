/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_binding_scheduler.c
 *
 * PURPOSE:
 *   Exercise the binding scheduler reactive UI contract.
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
#include "umicom/ui/reactive/binding_scheduler.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/binding_scheduler.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveBindingSchedulerTransferEqual(const UmiUiReactiveBindingScheduler *a, const UmiUiReactiveBindingScheduler *b)
{
    return a->pending == b->pending &&
        a->generation == b->generation &&
        a->suspended == b->suspended;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveBindingSchedulerTransferTails(UmiUiReactiveBindingScheduler *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveBindingSchedulerTransferMalformed(const UmiUiReactiveBindingScheduler *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveBindingSchedulerTransferCases, UmiUiReactiveBindingScheduler,
    umi_ui_reactive_binding_scheduler_archive_encode, umi_ui_reactive_binding_scheduler_archive_decode,
    UmiUiReactiveBindingSchedulerTransferEqual, UmiUiReactiveBindingSchedulerTransferTails, UmiUiReactiveBindingSchedulerTransferMalformed)

int main(void) { UmiUiReactiveBindingScheduler item; umi_ui_reactive_binding_scheduler_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveBindingScheduler populated = item;
    populated.pending = (size_t)2;
    populated.generation = (uint64_t)3;
    populated.suspended = true;
    if (UmiUiReactiveBindingSchedulerTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_binding_scheduler_valid(&item) ? 0 : 1; }
