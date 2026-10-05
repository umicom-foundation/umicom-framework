/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_ui_state_service.c
 *
 * PURPOSE:
 *   Exercise the ui state service reactive UI contract.
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
#include "umicom/ui/reactive/ui_state_service.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/reactive/ui_state_service.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiReactiveUiStateServiceTransferEqual(const UmiUiReactiveUiStateService *a, const UmiUiReactiveUiStateService *b)
{
    return a->bindings_ready == b->bindings_ready &&
        a->validation_ready == b->validation_ready &&
        a->graph_ready == b->graph_ready &&
        a->scheduler_ready == b->scheduler_ready &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiReactiveUiStateServiceTransferTails(UmiUiReactiveUiStateService *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiReactiveUiStateServiceTransferMalformed(const UmiUiReactiveUiStateService *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiReactiveUiStateServiceTransferCases, UmiUiReactiveUiStateService,
    umi_ui_reactive_ui_state_service_archive_encode, umi_ui_reactive_ui_state_service_archive_decode,
    UmiUiReactiveUiStateServiceTransferEqual, UmiUiReactiveUiStateServiceTransferTails, UmiUiReactiveUiStateServiceTransferMalformed)

int main(void) { UmiUiReactiveUiStateService item; umi_ui_reactive_ui_state_service_init(&item);
    /* Nonzero fields expose swapped or omitted values in a saved record.
     * The original initialized fixture remains available for its own checks. */
    UmiUiReactiveUiStateService populated = item;
    populated.bindings_ready = true;
    populated.validation_ready = true;
    populated.graph_ready = true;
    populated.scheduler_ready = true;
    populated.revision = (uint64_t)6;
    if (UmiUiReactiveUiStateServiceTransferCases(&populated) != 0) return 1;
 return umi_ui_reactive_ui_state_service_valid(&item) ? 0 : 1; }
