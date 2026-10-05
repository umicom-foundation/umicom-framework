/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_status_model.c
 *
 * PURPOSE:
 *   Verify aggregate active session, stop reason and inspection-count status for workbench chrome.
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
#include "umicom/debug/workbench/debug_status_model.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/debug/workbench/debug_status_model.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDebugWorkbenchDebugStatusModelTransferEqual(const UmiDebugWorkbenchDebugStatusModel *a, const UmiDebugWorkbenchDebugStatusModel *b)
{
    return strcmp(a->session_id, b->session_id) == 0 &&
        strcmp(a->stop_reason, b->stop_reason) == 0 &&
        a->phase == b->phase &&
        a->thread_count == b->thread_count &&
        a->frame_count == b->frame_count &&
        a->variable_count == b->variable_count &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDebugWorkbenchDebugStatusModelTransferTails(UmiDebugWorkbenchDebugStatusModel *value)
{
    (void)value;
    {
        size_t used = strlen(value->session_id) + 1U;
        memset(value->session_id + used, 0xa5, sizeof(value->session_id) - used);
    }
    {
        size_t used = strlen(value->stop_reason) + 1U;
        memset(value->stop_reason + used, 0xa5, sizeof(value->stop_reason) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDebugWorkbenchDebugStatusModelTransferMalformed(const UmiDebugWorkbenchDebugStatusModel *sample)
{
    (void)sample;
    {
        UmiDebugWorkbenchDebugStatusModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.session_id, 'x', sizeof(invalid.session_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_debug_workbench_debug_status_model_valid(&invalid)) ||
            umi_debug_workbench_debug_status_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated session_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDebugWorkbenchDebugStatusModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.stop_reason, 'x', sizeof(invalid.stop_reason));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_debug_workbench_debug_status_model_valid(&invalid)) ||
            umi_debug_workbench_debug_status_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated stop_reason was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDebugWorkbenchDebugStatusModelTransferCases, UmiDebugWorkbenchDebugStatusModel,
    umi_debug_workbench_debug_status_model_archive_encode, umi_debug_workbench_debug_status_model_archive_decode,
    UmiDebugWorkbenchDebugStatusModelTransferEqual, UmiDebugWorkbenchDebugStatusModelTransferTails, UmiDebugWorkbenchDebugStatusModelTransferMalformed)

int main(void)
{
    UmiDebugWorkbenchDebugStatusModel model;
    UMI_TEST_CHECK(umi_debug_workbench_debug_status_model_init(&model, "session-status") == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_status_model_update(&model, UMI_DEBUG_WORKBENCH_SESSION_PAUSED, "breakpoint", 4U, 8U, 21U) == UMI_STATUS_OK);
    UMI_TEST_CHECK(model.thread_count == 4U && model.frame_count == 8U && model.variable_count == 21U);
    UMI_TEST_CHECK(strcmp(model.stop_reason, "breakpoint") == 0);
    UMI_TEST_CHECK(umi_debug_workbench_debug_status_model_valid(&model));
    if (UmiDebugWorkbenchDebugStatusModelTransferCases(&model) != 0) return 1;

    return 0;
}
