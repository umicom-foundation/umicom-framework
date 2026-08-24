/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/views/inline_ai.c
 *
 * PURPOSE:
 *   Project the active inline AI completion and controls.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/views/inline_ai.h"

UmiStatus umi_ide_inline_ai_view_create(
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
        "ide-inline-ai",
        "Inline AI",
        "Low-latency provider-backed code completion with revision-checked acceptance.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    status = umi_ide_view_set_boolean(
        *out_view, "ide.inline.enabled", snapshot.inline_ready);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_string(
            *out_view, "ide.inline.state",
            snapshot.inline_ready
                ? umi_ide_inline_state_text(snapshot.inline_suggestion.state)
                : "disabled");
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_string(
            *out_view, "ide.inline.path",
            snapshot.inline_suggestion.path);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_string(
            *out_view, "ide.inline.text",
            snapshot.inline_suggestion.text);

    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 0U, "ide.inline.request",
            "Request Completion", "Generate an inline completion.",
            snapshot.inline_ready &&
            snapshot.inline_suggestion.state != UMI_IDE_INLINE_READY &&
            snapshot.inline_suggestion.state != UMI_IDE_INLINE_REQUESTED);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 1U, "ide.inline.accept",
            "Accept", "Insert the active completion after revision checks.",
            snapshot.inline_ready &&
            snapshot.inline_suggestion.state == UMI_IDE_INLINE_READY);
    if (status == UMI_STATUS_OK)
        status = umi_ide_view_set_action(
            *out_view, 2U, "ide.inline.reject",
            "Reject", "Reject the active completion.",
            snapshot.inline_ready &&
            snapshot.inline_suggestion.state == UMI_IDE_INLINE_READY);

    return status;
}
