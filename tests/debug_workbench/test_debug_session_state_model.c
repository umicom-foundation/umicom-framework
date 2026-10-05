/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_workbench/test_debug_session_state_model.c
 *
 * PURPOSE:
 *   Verify track debugger lifecycle state and enforce legal high-level phase transitions.
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
#include "umicom/debug/workbench/debug_session_state_model.h"
#define UMI_TEST_CHECK(expression) do { if (!(expression)) return 1; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/debug/workbench/debug_session_state_model.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDebugWorkbenchDebugSessionStateModelTransferEqual(const UmiDebugWorkbenchDebugSessionStateModel *a, const UmiDebugWorkbenchDebugSessionStateModel *b)
{
    return strcmp(a->session_id, b->session_id) == 0 &&
        a->phase == b->phase &&
        a->stop_sequence == b->stop_sequence &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDebugWorkbenchDebugSessionStateModelTransferTails(UmiDebugWorkbenchDebugSessionStateModel *value)
{
    (void)value;
    {
        size_t used = strlen(value->session_id) + 1U;
        memset(value->session_id + used, 0xa5, sizeof(value->session_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDebugWorkbenchDebugSessionStateModelTransferMalformed(const UmiDebugWorkbenchDebugSessionStateModel *sample)
{
    (void)sample;
    {
        UmiDebugWorkbenchDebugSessionStateModel invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.session_id, 'x', sizeof(invalid.session_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_debug_workbench_debug_session_state_model_valid(&invalid)) ||
            umi_debug_workbench_debug_session_state_model_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated session_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDebugWorkbenchDebugSessionStateModelTransferCases, UmiDebugWorkbenchDebugSessionStateModel,
    umi_debug_workbench_debug_session_state_model_archive_encode, umi_debug_workbench_debug_session_state_model_archive_decode,
    UmiDebugWorkbenchDebugSessionStateModelTransferEqual, UmiDebugWorkbenchDebugSessionStateModelTransferTails, UmiDebugWorkbenchDebugSessionStateModelTransferMalformed)

int main(void)
{
    UmiDebugWorkbenchDebugSessionStateModel model;
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_state_model_init(&model, "session-main") == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_state_model_transition(&model, UMI_DEBUG_WORKBENCH_SESSION_INITIALIZING) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_state_model_transition(&model, UMI_DEBUG_WORKBENCH_SESSION_RUNNING) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_state_model_transition(&model, UMI_DEBUG_WORKBENCH_SESSION_PAUSED) == UMI_STATUS_OK);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_state_model_record_stop(&model) == UMI_STATUS_OK);
    UMI_TEST_CHECK(model.stop_sequence == 1U);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_state_model_transition(&model, UMI_DEBUG_WORKBENCH_SESSION_INITIALIZING) == UMI_STATUS_INVALID_STATE);
    UMI_TEST_CHECK(umi_debug_workbench_debug_session_state_model_valid(&model));
    if (UmiDebugWorkbenchDebugSessionStateModelTransferCases(&model) != 0) return 1;

    return 0;
}
