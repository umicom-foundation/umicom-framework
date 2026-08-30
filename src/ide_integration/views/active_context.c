/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/views/active_context.c
 *
 * PURPOSE:
 *   Project current cross-domain IDE context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/views/active_context.h"

#include <stdio.h>

UmiStatus umi_ide_active_context_view_create(
    const char *view_id,
    UmiIdeIntegrationPlatform *platform,
    UmiUiViewModel **out_view)
{
    UmiIdeIntegrationPlatformSnapshot snapshot;
    char text[512];
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ide_integration_platform_snapshot(platform, &snapshot);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ide_view_create_base(
        view_id,
        "ide-active-context",
        "Active IDE Context",
        "Current authoritative document, diagnostics, tests, source control, debugger and AI state.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    (void)snprintf(
        text, sizeof(text),
        "%.280s | %.80s | dirty=%d | revision=%llu",
        snapshot.context.has_document
            ? snapshot.context.document.display_name : "No document",
        snapshot.context.has_document
            ? snapshot.context.document.language_id : "",
        snapshot.context.has_document
            ? snapshot.context.document.dirty : 0,
        (unsigned long long)(snapshot.context.has_document
            ? snapshot.context.document.revision : 0U));

    status = umi_ide_view_set_string(*out_view, "ide.context.document", text);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_integer(
            *out_view, "ide.context.problems",
            snapshot.context.has_problems
                ? (int64_t)snapshot.context.problems.count : 0);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_integer(
            *out_view, "ide.context.tests",
            snapshot.context.has_tests
                ? (int64_t)snapshot.context.tests.item_count : 0);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_string(
            *out_view, "ide.context.branch",
            snapshot.context.has_source_control
                ? snapshot.context.source_control.branch.name : "");
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_boolean(
            *out_view, "ide.context.debug-active",
            snapshot.context.has_debug && snapshot.context.debug.active);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_string(
            *out_view, "ide.context.ai-state",
            snapshot.context.has_ai
                ? umi_ai_coding_runtime_state_text(
                    snapshot.context.ai.coding.agent.state)
                : "unavailable");

    return status;
}
