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

UmiStatus umi_studio_close_guard_view_create(
    const char *view_id,
    UmiStudioRuntimePlatform *platform,
    UmiUiViewModel **out_view)
{
    UmiStudioRuntimeSnapshot snapshot;
    UmiStudioRuntimeBindings *bindings;
    UmiStudioRuntimeCloseReport report;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    bindings = umi_studio_runtime_platform_bindings(platform);
    if (bindings == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_studio_runtime_platform_snapshot(platform, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = umi_studio_close_guard_evaluate(
            bindings->documents,
            &snapshot.ide.context,
            &report);
    }
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_create_base(
        view_id,
        "studio-close-guard",
        "Close Safety",
        "Dirty documents and running test/debug/AI operations before closing Studio.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_set_string(
        *out_view, "studio.close.decision",
        umi_studio_runtime_close_decision_text(report.decision));
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_string(
            *out_view, "studio.close.summary", report.summary);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_integer(
            *out_view, "studio.close.dirty",
            (int64_t)report.dirty_document_count);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_boolean(
            *out_view, "studio.close.tests-running", report.tests_running);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_boolean(
            *out_view, "studio.close.debug-running", report.debug_running);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_boolean(
            *out_view, "studio.close.ai-running", report.ai_running);

    return status;
}
