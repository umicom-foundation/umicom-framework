/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_keymap_context.c
 *
 * PURPOSE:
 *   Verify represent debugger keymap activation state and command-context precedence.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
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
#include "umicom/debug/workbench/debug_keymap_context.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/debug/workbench/debug_keymap_context.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDebugWorkbenchDebugKeymapContextTransferEqual(const UmiDebugWorkbenchDebugKeymapContext *a, const UmiDebugWorkbenchDebugKeymapContext *b)
{
    return a->enabled_commands == b->enabled_commands &&
        a->visible_commands == b->visible_commands &&
        a->primary_command == b->primary_command &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDebugWorkbenchDebugKeymapContextTransferTails(UmiDebugWorkbenchDebugKeymapContext *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDebugWorkbenchDebugKeymapContextTransferMalformed(const UmiDebugWorkbenchDebugKeymapContext *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDebugWorkbenchDebugKeymapContextTransferCases, UmiDebugWorkbenchDebugKeymapContext,
    umi_debug_workbench_debug_keymap_context_archive_encode, umi_debug_workbench_debug_keymap_context_archive_decode,
    UmiDebugWorkbenchDebugKeymapContextTransferEqual, UmiDebugWorkbenchDebugKeymapContextTransferTails, UmiDebugWorkbenchDebugKeymapContextTransferMalformed)

int main(void)
{
    UmiDebugWorkbenchDebugKeymapContext model;
    umi_debug_workbench_debug_keymap_context_init(&model);
    UMI_TEST_CHECK(umi_debug_workbench_debug_keymap_context_set_enabled(&model, UMI_DEBUG_WORKBENCH_COMMAND_CONTINUE, true) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_keymap_context_is_enabled(&model, UMI_DEBUG_WORKBENCH_COMMAND_CONTINUE));
    UMI_TEST_CHECK(umi_debug_workbench_debug_keymap_context_set_enabled(&model, UMI_DEBUG_WORKBENCH_COMMAND_CONTINUE, false) == UMI_STATUS_OK);
    UMI_TEST_CHECK(!umi_debug_workbench_debug_keymap_context_is_enabled(&model, UMI_DEBUG_WORKBENCH_COMMAND_CONTINUE));
    UMI_TEST_CHECK(umi_debug_workbench_debug_keymap_context_set_primary(&model, UMI_DEBUG_WORKBENCH_COMMAND_STEP_OVER) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_keymap_context_valid(&model));
    if (UmiDebugWorkbenchDebugKeymapContextTransferCases(&model) != 0) return 1;

    return 0;
}
