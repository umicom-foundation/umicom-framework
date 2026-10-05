/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_types.c
 *
 * PURPOSE:
 *   Verify define stable debugger-workbench identifiers, source locations, phases, commands and shared value types.
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
#include "umicom/debug/workbench/types.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/debug/workbench/types.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDebugWorkbenchEntryTransferEqual(const UmiDebugWorkbenchEntry *a, const UmiDebugWorkbenchEntry *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->label, b->label) == 0 &&
        strcmp(a->detail, b->detail) == 0 &&
        strcmp(a->location.path, b->location.path) == 0 &&
        a->location.range.start.line == b->location.range.start.line &&
        a->location.range.start.column == b->location.range.start.column &&
        a->location.range.end.line == b->location.range.end.line &&
        a->location.range.end.column == b->location.range.end.column &&
        a->state == b->state &&
        a->flags == b->flags &&
        a->value == b->value &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDebugWorkbenchEntryTransferTails(UmiDebugWorkbenchEntry *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->label) + 1U;
        memset(value->label + used, 0xa5, sizeof(value->label) - used);
    }
    {
        size_t used = strlen(value->detail) + 1U;
        memset(value->detail + used, 0xa5, sizeof(value->detail) - used);
    }
    {
        size_t used = strlen(value->location.path) + 1U;
        memset(value->location.path + used, 0xa5, sizeof(value->location.path) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDebugWorkbenchEntryTransferMalformed(const UmiDebugWorkbenchEntry *sample)
{
    (void)sample;
    {
        UmiDebugWorkbenchEntry invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_debug_workbench_entry_valid(&invalid)) ||
            umi_debug_workbench_entry_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDebugWorkbenchEntry invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.label, 'x', sizeof(invalid.label));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_debug_workbench_entry_valid(&invalid)) ||
            umi_debug_workbench_entry_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated label was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDebugWorkbenchEntry invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.detail, 'x', sizeof(invalid.detail));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_debug_workbench_entry_valid(&invalid)) ||
            umi_debug_workbench_entry_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated detail was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDebugWorkbenchEntry invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.location.path, 'x', sizeof(invalid.location.path));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_debug_workbench_entry_valid(&invalid)) ||
            umi_debug_workbench_entry_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated location.path was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDebugWorkbenchEntryTransferCases, UmiDebugWorkbenchEntry,
    umi_debug_workbench_entry_archive_encode, umi_debug_workbench_entry_archive_decode,
    UmiDebugWorkbenchEntryTransferEqual, UmiDebugWorkbenchEntryTransferTails, UmiDebugWorkbenchEntryTransferMalformed)

int main(void)
{
    UmiDebugWorkbenchEntry entry;
    UmiDebugWorkbenchRange range = {{9U, 7U}, {3U, 2U}};
    UMI_TEST_CHECK(umi_debug_workbench_entry_init(&entry, "session-1", "Session 1", "gdb-dap", "src/main.c", range) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_entry_valid(&entry));
    if (UmiDebugWorkbenchEntryTransferCases(&entry) != 0) return 1;

    UMI_TEST_CHECK(entry.location.range.start.line == 3U);
    UMI_TEST_CHECK(strcmp(entry.location.path, "src/main.c") == 0);
    UMI_TEST_CHECK(umi_debug_workbench_session_transition_allowed(UMI_DEBUG_WORKBENCH_SESSION_RUNNING, UMI_DEBUG_WORKBENCH_SESSION_PAUSED));
    UMI_TEST_CHECK(!umi_debug_workbench_session_transition_allowed(UMI_DEBUG_WORKBENCH_SESSION_RUNNING, UMI_DEBUG_WORKBENCH_SESSION_INITIALIZING));
    UMI_TEST_CHECK(umi_debug_workbench_command_bit(UMI_DEBUG_WORKBENCH_COMMAND_STEP_IN) != 0U);
    return 0;
}
