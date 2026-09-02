/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/views/layout_browser.c
 *
 * PURPOSE:
 *   Project Framework Studio semantic layout presets and selection commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/views/layout_browser.h"

#include <stdio.h>
#include <string.h>

/*
 * Initialise studio layout browser view from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_studio_layout_browser_view_create(
    const char *view_id,
    UmiStudioRuntimePlatform *platform,
    UmiUiViewModel **out_view)
{
    UmiStudioRuntimeSnapshot snapshot;
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_studio_runtime_platform_snapshot(platform, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_create_base(
        view_id,
        "studio-layout-browser",
        "Layout Browser",
        "Framework-owned semantic layouts for professional workspace switching.",
        out_view);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_set_string(
        *out_view,
        "studio.layouts.active",
        snapshot.active_layout_preset_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_integer(
            *out_view,
            "studio.layouts.count",
            (int64_t)umi_studio_layout_catalogue_count());

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         status == UMI_STATUS_OK &&
         index < umi_studio_layout_catalogue_count();
         ++index) {
        const UmiStudioRuntimeLayoutPresetDefinition *preset =
            umi_studio_layout_catalogue_at(index);
        char key[96];
        char row[512];

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (preset == NULL) continue;

        (void)snprintf(key, sizeof(key), "studio.layouts.row.%zu", index);
        (void)snprintf(
            row,
            sizeof(row),
            "%s%.160s | %.300s",
            strcmp(
                snapshot.active_layout_preset_id,
                preset->preset_id) == 0
                ? "* "
                : "",
            preset->title,
            preset->description);

        status = umi_studio_view_set_string(*out_view, key, row);
    }

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_action(
            *out_view, 0U, "studio.layout.default",
            "Default", "Switch to default development layout.", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_action(
            *out_view, 1U, "studio.layout.debugging",
            "Debugging", "Switch to debugging layout.", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_action(
            *out_view, 2U, "studio.layout.testing",
            "Testing", "Switch to testing layout.", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_action(
            *out_view, 3U, "studio.layout.ai-development",
            "AI Development", "Switch to AI development layout.", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_action(
            *out_view, 4U, "studio.layout.review",
            "Review", "Switch to AI patch-review layout.", 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_action(
            *out_view, 5U, "studio.layout.zen",
            "Zen", "Switch to distraction-free editor layout.", 1);

    return status;
}
