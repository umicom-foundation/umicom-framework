/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/views/runtime_overview.c
 *
 * PURPOSE:
 *   Project the final Framework Studio runtime composition into one overview.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/views/runtime_overview.h"

UmiStatus umi_studio_runtime_overview_view_create(
    const char *view_id,
    UmiStudioRuntimePlatform *platform,
    UmiUiViewModel **out_view)
{
    UmiStudioRuntimeSnapshot snapshot;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_studio_runtime_platform_snapshot(platform, &snapshot);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_create_base(
        view_id,
        "studio-runtime-overview",
        "Studio Runtime",
        "Framework-owned Studio shell, IDE integration, documents, commands, layouts and status.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_set_string(
        *out_view, "studio.window-title", snapshot.window_title);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_string(
            *out_view, "studio.layout", snapshot.active_layout_preset_id);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_integer(
            *out_view, "studio.open-documents", (int64_t)snapshot.tabs.count);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_integer(
            *out_view, "studio.aliases", (int64_t)snapshot.registered_alias_count);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_integer(
            *out_view, "studio.commands-enabled",
            (int64_t)snapshot.command_sync.enabled_count);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_integer(
            *out_view, "studio.commands-disabled",
            (int64_t)snapshot.command_sync.disabled_count);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_boolean(
            *out_view, "studio.workflow-ready", snapshot.ide.workflow.ready);

    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_action(
            *out_view, 0U, "studio.refresh",
            "Refresh", "Refresh all Framework Studio state.", 1);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_action(
            *out_view, 1U, "studio.contract.check",
            "Check Contract", "Verify thin-Studio runtime closure.", 1);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_action(
            *out_view, 2U, "studio.layout.manager",
            "Layouts", "Open the semantic layout manager.", 1);

    return status;
}
