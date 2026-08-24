/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/views/selection.c
 *
 * PURPOSE:
 *   Project the current cross-domain Studio selection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/views/selection.h"

#include <stdio.h>

UmiStatus umi_studio_selection_view_create(
    const char *view_id,
    UmiStudioRuntimePlatform *platform,
    UmiUiViewModel **out_view)
{
    UmiStudioRuntimeSnapshot snapshot;
    char location[512];
    UmiStatus status;

    if (platform == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_studio_runtime_platform_snapshot(platform, &snapshot);
    if (status != UMI_STATUS_OK) return status;

    status = umi_studio_view_create_base(
        view_id,
        "studio-selection",
        "Studio Selection",
        "Current Problems/Test/VCS/Debug/Symbol/Editor/AI selection routed to commands.",
        out_view);
    if (status != UMI_STATUS_OK) return status;

    (void)snprintf(
        location,
        sizeof(location),
        "%.330s:%u:%u",
        snapshot.selection.path,
        snapshot.selection.line,
        snapshot.selection.column);

    status = umi_studio_view_set_string(
        *out_view, "studio.selection.kind",
        umi_studio_runtime_selection_kind_text(snapshot.selection.kind));
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_string(
            *out_view, "studio.selection.subject",
            snapshot.selection.subject_id);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_string(
            *out_view, "studio.selection.label",
            snapshot.selection.label);
    if (status == UMI_STATUS_OK)
        status = umi_studio_view_set_string(
            *out_view, "studio.selection.location", location);

    return status;
}
