/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_workbench_snapshot.c
 *
 * PURPOSE:
 *   Verify capture the aggregate debugger workbench selection and visible-state summary.
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
#include "umicom/debug/workbench/debug_workbench_snapshot.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/debug/workbench/debug_workbench_snapshot.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDebugWorkbenchDebugWorkbenchSnapshotTransferEqual(const UmiDebugWorkbenchDebugWorkbenchSnapshot *a, const UmiDebugWorkbenchDebugWorkbenchSnapshot *b)
{
    return strcmp(a->active_session_id, b->active_session_id) == 0 &&
        strcmp(a->active_item_id, b->active_item_id) == 0 &&
        a->session_count == b->session_count &&
        a->breakpoint_count == b->breakpoint_count &&
        a->thread_count == b->thread_count &&
        a->watch_count == b->watch_count &&
        a->generation == b->generation;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDebugWorkbenchDebugWorkbenchSnapshotTransferTails(UmiDebugWorkbenchDebugWorkbenchSnapshot *value)
{
    (void)value;
    {
        size_t used = strlen(value->active_session_id) + 1U;
        memset(value->active_session_id + used, 0xa5, sizeof(value->active_session_id) - used);
    }
    {
        size_t used = strlen(value->active_item_id) + 1U;
        memset(value->active_item_id + used, 0xa5, sizeof(value->active_item_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDebugWorkbenchDebugWorkbenchSnapshotTransferMalformed(const UmiDebugWorkbenchDebugWorkbenchSnapshot *sample)
{
    (void)sample;
    {
        UmiDebugWorkbenchDebugWorkbenchSnapshot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.active_session_id, 'x', sizeof(invalid.active_session_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_debug_workbench_debug_workbench_snapshot_valid(&invalid)) ||
            umi_debug_workbench_debug_workbench_snapshot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated active_session_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDebugWorkbenchDebugWorkbenchSnapshot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.active_item_id, 'x', sizeof(invalid.active_item_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_debug_workbench_debug_workbench_snapshot_valid(&invalid)) ||
            umi_debug_workbench_debug_workbench_snapshot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated active_item_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDebugWorkbenchDebugWorkbenchSnapshotTransferCases, UmiDebugWorkbenchDebugWorkbenchSnapshot,
    umi_debug_workbench_debug_workbench_snapshot_archive_encode, umi_debug_workbench_debug_workbench_snapshot_archive_decode,
    UmiDebugWorkbenchDebugWorkbenchSnapshotTransferEqual, UmiDebugWorkbenchDebugWorkbenchSnapshotTransferTails, UmiDebugWorkbenchDebugWorkbenchSnapshotTransferMalformed)

int main(void)
{
    UmiDebugWorkbenchDebugWorkbenchSnapshot model;
    umi_debug_workbench_debug_workbench_snapshot_init(&model);
    UMI_TEST_CHECK(umi_debug_workbench_debug_workbench_snapshot_capture(&model, "session-a", "frame-2", 2U, 5U, 3U, 4U) == UMI_STATUS_OK);
    UMI_TEST_CHECK(model.session_count == 2U && model.breakpoint_count == 5U);
    UMI_TEST_CHECK(model.thread_count == 3U && model.watch_count == 4U);
    UMI_TEST_CHECK(umi_debug_workbench_debug_workbench_snapshot_valid(&model));
    if (UmiDebugWorkbenchDebugWorkbenchSnapshotTransferCases(&model) != 0) return 1;

    return 0;
}
