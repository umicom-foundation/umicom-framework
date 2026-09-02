/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/views/close_guard.c
 *
 * PURPOSE:
 *   Project dirty-document/running-operation close-safety evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/views/close_guard.h"

/*
 * Initialise studio close guard view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_studio_close_guard_view_create(
    const char *view_id,
    UmiStudioRuntimePlatform *platform,
    UmiUiViewModel **out_view)
{
    UmiStudioRuntimeSnapshot snapshot;
    UmiStudioRuntimeBindings *bindings;
    UmiStudioRuntimeCloseReport report;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    bindings = umi_studio_runtime_platform_bindings(platform);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bindings == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_studio_runtime_platform_snapshot(platform, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_studio_close_guard_evaluate(
            bindings->documents,
            &snapshot.ide.context,
            &report);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_create_base(
        view_id,
        "studio-close-guard",
        "Close Safety",
        "Dirty documents and running test/debug/AI operations before closing Studio.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_set_string(
        *out_view, "studio.close.decision",
        umi_studio_runtime_close_decision_text(report.decision));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_string(
            *out_view, "studio.close.summary", report.summary);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_integer(
            *out_view, "studio.close.dirty",
            (int64_t)report.dirty_document_count);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_boolean(
            *out_view, "studio.close.tests-running", report.tests_running);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_boolean(
            *out_view, "studio.close.debug-running", report.debug_running);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_boolean(
            *out_view, "studio.close.ai-running", report.ai_running);

    return status;
}
