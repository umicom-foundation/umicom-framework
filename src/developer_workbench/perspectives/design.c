/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/perspectives/design.c
 *
 * PURPOSE:
 *   Define the Framework-owned Design perspective surface set.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/perspectives/design.h"

static const char *const SURFACES[] = {
    "umicom.development.designer",
    "umicom.development.explorer",
    "umicom.development.editor"
};

const UmiDeveloperWorkbenchPerspectiveDefinition *umi_developer_workbench_perspective_design(void)
{
    static const UmiDeveloperWorkbenchPerspectiveDefinition perspective = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperWorkbenchPerspectiveDefinition),
        .api_version = UMI_DEVELOPER_WORKBENCH_API_VERSION,
        .perspective_id = "developer.perspective.design",
        .title = "Design",
        .description =
            "Task-oriented Design tool selection owned by Umicom Framework.",
        .surface_ids = SURFACES,
        .surface_count = sizeof(SURFACES) / sizeof(SURFACES[0]),
        .default_focus_surface_id = "umicom.development.designer"
    };

    return &perspective;
}
