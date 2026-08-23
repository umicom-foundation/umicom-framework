/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/perspectives/source_control.c
 *
 * PURPOSE:
 *   Define the Framework-owned Source Control perspective surface set.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/perspectives/source_control.h"

static const char *const SURFACES[] = {
    "umicom.development.editor",
    "umicom.development.source-control",
    "umicom.development.explorer"
};

const UmiDeveloperWorkbenchPerspectiveDefinition *umi_developer_workbench_perspective_source_control(void)
{
    static const UmiDeveloperWorkbenchPerspectiveDefinition perspective = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperWorkbenchPerspectiveDefinition),
        .api_version = UMI_DEVELOPER_WORKBENCH_API_VERSION,
        .perspective_id = "developer.perspective.source-control",
        .title = "Source Control",
        .description =
            "Task-oriented Source Control tool selection owned by Umicom Framework.",
        .surface_ids = SURFACES,
        .surface_count = sizeof(SURFACES) / sizeof(SURFACES[0]),
        .default_focus_surface_id = "umicom.development.editor"
    };

    return &perspective;
}
