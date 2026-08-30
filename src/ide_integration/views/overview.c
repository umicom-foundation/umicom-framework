/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/views/overview.c
 *
 * PURPOSE:
 *   Project high-level IDE integration readiness and primary actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/views/overview.h"

UmiStatus umi_ide_overview_view_create(
    const char *view_id,
    UmiIdeIntegrationPlatform *platform,
    UmiUiViewModel **out_view)
{
    UmiIdeIntegrationPlatformSnapshot snapshot;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_ide_integration_platform_snapshot(platform, &snapshot);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ide_view_create_base(
        view_id,
        "ide-integration-overview",
        "IDE Integration",
        "Framework service composition, cross-navigation, inline AI and workflow readiness.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ide_view_set_boolean(
        *out_view, "ide.workflow-ready", snapshot.workflow.ready);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_integer(
            *out_view, "ide.surface-count", (int64_t)snapshot.surface_count);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_integer(
            *out_view, "ide.navigation-count",
            (int64_t)snapshot.navigation_history_count);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_boolean(
            *out_view, "ide.inline-ready", snapshot.inline_ready);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_boolean(
            *out_view, "ide.document-active", snapshot.context.has_document);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_integer(
            *out_view, "ide.problem-count",
            snapshot.context.has_problems
                ? (int64_t)snapshot.context.problems.count
                : 0);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_integer(
            *out_view, "ide.pending-ai-approvals",
            snapshot.context.has_ai
                ? (int64_t)snapshot.context.ai.pending_approval_count
                : 0);

    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 0U, "ide.workflow.refresh",
            "Refresh", "Refresh all Framework IDE state.", 1);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 1U, "ide.self-host.verify",
            "Self-Host Check", "Evaluate strict self-host readiness.", 1);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 2U, "ide.ai.open-overview",
            "AI Developer", "Open the AI Developer overview.",
            snapshot.context.has_ai);

    return status;
}
