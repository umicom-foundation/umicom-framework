/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/views/status.c
 *
 * PURPOSE:
 *   Project the Framework Studio status model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/views/status.h"

#include <stdio.h>

UmiStatus umi_studio_status_view_create(
    const char *view_id,
    UmiStudioRuntimePlatform *platform,
    UmiUiViewModel **out_view)
{
    const UmiStudioRuntimeStatusModel *model;
    size_t index;
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    model = umi_studio_runtime_platform_status(platform);
    if (model == NULL) return UMI_STATUS_INVALID_STATE;

    status = umi_studio_view_create_base(
        view_id,
        "studio-status",
        "Studio Status",
        "Workspace, VCS, diagnostics, tests, workflow, debug, AI and editor status.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_set_integer(
        *out_view, "studio.status.count", (int64_t)model->item_count);

    for (index = 0U;
         status == UMI_STATUS_OK && index < model->item_count;
         ++index) {
        const UmiStudioRuntimeStatusItem *item = &model->items[index];
        char key[96];
        char row[512];

        (void)snprintf(key, sizeof(key), "studio.status.row.%zu", index);
        (void)snprintf(
            row, sizeof(row),
            "%s | %.330s | badge=%u%s",
            umi_studio_runtime_status_kind_text(item->kind),
            item->text,
            item->badge_count,
            item->important ? " | important" : "");

        status = umi_studio_view_set_string(*out_view, key, row);
    }

    return status;
}
