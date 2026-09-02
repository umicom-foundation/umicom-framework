/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/close_guard.c
 *
 * PURPOSE:
 *   Implement close confirmation policy without toolkit dialog ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/close_guard.h"

#include <stdio.h>
#include <string.h>

/* Provide the ai state running operation used by this module and its client applications. */
static int ai_state_running(UmiAiCodingRuntimeState state)
{
    return state == UMI_AI_CODING_RUNTIME_PREPARING ||
        state == UMI_AI_CODING_RUNTIME_MATERIALISING_CONTEXT ||
        state == UMI_AI_CODING_RUNTIME_GENERATING ||
        state == UMI_AI_CODING_RUNTIME_APPLYING ||
        state == UMI_AI_CODING_RUNTIME_VALIDATING ||
        state == UMI_AI_CODING_RUNTIME_REPAIRING;
}

/*
 * Provide the studio close guard evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_close_guard_evaluate(
    UmiDocumentCoordinator *documents,
    const UmiIdeActiveContext *context,
    UmiStudioRuntimeCloseReport *out_report)
{
    size_t index;
    int running;
    int written;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (documents == NULL || context == NULL || out_report == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_report, 0, sizeof(*out_report));

    out_report->open_document_count =
        umi_document_coordinator_count(documents);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < out_report->open_document_count; ++index) {
        UmiDocumentWorkingCopySnapshot document;

        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_document_coordinator_at(
                documents,
                index,
                &document) == UMI_STATUS_OK &&
            document.dirty) {
            out_report->dirty_document_count += 1U;
        }
    }

    out_report->tests_running =
        context->has_tests && context->tests.operation_running;
    out_report->debug_running =
        context->has_debug && context->debug.active;
    out_report->ai_running =
        context->has_ai && ai_state_running(context->ai.coding.agent.state);

    running =
        out_report->tests_running ||
        out_report->debug_running ||
        out_report->ai_running;

    /* Apply this branch only when its contract condition is satisfied. */
    if (out_report->dirty_document_count > 0U && running) {
        out_report->decision =
            UMI_STUDIO_CLOSE_CONFIRM_DIRTY_AND_RUNNING;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (out_report->dirty_document_count > 0U) {
        out_report->decision = UMI_STUDIO_CLOSE_CONFIRM_DIRTY;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (running) {
        out_report->decision = UMI_STUDIO_CLOSE_CONFIRM_RUNNING;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        out_report->decision = UMI_STUDIO_CLOSE_ALLOW;
    }

    written = snprintf(
        out_report->summary,
        sizeof(out_report->summary),
        "openDocuments=%zu dirtyDocuments=%zu testsRunning=%d "
        "debugRunning=%d aiRunning=%d decision=%s",
        out_report->open_document_count,
        out_report->dirty_document_count,
        out_report->tests_running,
        out_report->debug_running,
        out_report->ai_running,
        umi_studio_runtime_close_decision_text(out_report->decision));

    /* Apply this branch only when its contract condition is satisfied. */
    if (written < 0 ||
        (size_t)written >= sizeof(out_report->summary)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    out_report->revision = context->revision + 1U;
    return UMI_STATUS_OK;
}
