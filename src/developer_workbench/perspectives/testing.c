/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/perspectives/testing.c
 *
 * PURPOSE:
 *   Define the Framework-owned Testing perspective surface set.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/perspectives/testing.h"

static const char *const SURFACES[] = {
    "umicom.development.editor",
    "umicom.development.testing",
    "umicom.development.build",
    "umicom.development.terminal"
};

const UmiDeveloperWorkbenchPerspectiveDefinition *umi_developer_workbench_perspective_testing(void)
{
    static const UmiDeveloperWorkbenchPerspectiveDefinition perspective = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperWorkbenchPerspectiveDefinition),
        .api_version = UMI_DEVELOPER_WORKBENCH_API_VERSION,
        .perspective_id = "developer.perspective.testing",
        .title = "Testing",
        .description =
            "Task-oriented Testing tool selection owned by Umicom Framework.",
        .surface_ids = SURFACES,
        .surface_count = sizeof(SURFACES) / sizeof(SURFACES[0]),
        .default_focus_surface_id = "umicom.development.editor"
    };

    return &perspective;
}
