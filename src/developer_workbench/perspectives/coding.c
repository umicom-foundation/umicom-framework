/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/perspectives/coding.c
 *
 * PURPOSE:
 *   Define the Framework-owned Coding perspective surface set.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/perspectives/coding.h"

static const char *const SURFACES[] = {
    "umicom.development.explorer",
    "umicom.development.editor",
    "umicom.development.build",
    "umicom.development.terminal",
    "umicom.ai.chat"
};

const UmiDeveloperWorkbenchPerspectiveDefinition *umi_developer_workbench_perspective_coding(void)
{
    static const UmiDeveloperWorkbenchPerspectiveDefinition perspective = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperWorkbenchPerspectiveDefinition),
        .api_version = UMI_DEVELOPER_WORKBENCH_API_VERSION,
        .perspective_id = "developer.perspective.coding",
        .title = "Coding",
        .description =
            "Task-oriented Coding tool selection owned by Umicom Framework.",
        .surface_ids = SURFACES,
        .surface_count = sizeof(SURFACES) / sizeof(SURFACES[0]),
        .default_focus_surface_id = "umicom.development.explorer"
    };

    return &perspective;
}
