/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_workspace_profile.c
 *
 * PURPOSE:
 *   Verify persist per-workspace debugger layout and presentation preferences.
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
#include "umicom/debug/workbench/debug_workspace_profile.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/debug/workbench/debug_workspace_profile.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDebugWorkbenchDebugWorkspaceProfileTransferEqual(const UmiDebugWorkbenchDebugWorkspaceProfile *a, const UmiDebugWorkbenchDebugWorkspaceProfile *b)
{
    return strcmp(a->workspace_id, b->workspace_id) == 0 &&
        a->primary_view == b->primary_view &&
        a->visible_views == b->visible_views &&
        a->follow_instruction_pointer == b->follow_instruction_pointer &&
        a->open_console_on_output == b->open_console_on_output &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDebugWorkbenchDebugWorkspaceProfileTransferTails(UmiDebugWorkbenchDebugWorkspaceProfile *value)
{
    (void)value;
    {
        size_t used = strlen(value->workspace_id) + 1U;
        memset(value->workspace_id + used, 0xa5, sizeof(value->workspace_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDebugWorkbenchDebugWorkspaceProfileTransferMalformed(const UmiDebugWorkbenchDebugWorkspaceProfile *sample)
{
    (void)sample;
    {
        UmiDebugWorkbenchDebugWorkspaceProfile invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.workspace_id, 'x', sizeof(invalid.workspace_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_debug_workbench_debug_workspace_profile_valid(&invalid)) ||
            umi_debug_workbench_debug_workspace_profile_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated workspace_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDebugWorkbenchDebugWorkspaceProfileTransferCases, UmiDebugWorkbenchDebugWorkspaceProfile,
    umi_debug_workbench_debug_workspace_profile_archive_encode, umi_debug_workbench_debug_workspace_profile_archive_decode,
    UmiDebugWorkbenchDebugWorkspaceProfileTransferEqual, UmiDebugWorkbenchDebugWorkspaceProfileTransferTails, UmiDebugWorkbenchDebugWorkspaceProfileTransferMalformed)

int main(void)
{
    UmiDebugWorkbenchDebugWorkspaceProfile model;
    UMI_TEST_CHECK(umi_debug_workbench_debug_workspace_profile_init(&model, "workspace-main") == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_workspace_profile_set_view_visible(&model, UMI_DEBUG_WORKBENCH_VIEW_MEMORY, true) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_workspace_profile_view_visible(&model, UMI_DEBUG_WORKBENCH_VIEW_MEMORY));
    UMI_TEST_CHECK(umi_debug_workbench_debug_workspace_profile_set_primary_view(&model, UMI_DEBUG_WORKBENCH_VIEW_CALL_STACK) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_workspace_profile_valid(&model));
    if (UmiDebugWorkbenchDebugWorkspaceProfileTransferCases(&model) != 0) return 1;

    return 0;
}
