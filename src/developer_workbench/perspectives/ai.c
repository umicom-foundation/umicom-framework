/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/perspectives/ai.c
 *
 * PURPOSE:
 *   Define the Framework-owned Ai perspective surface set.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/perspectives/ai.h"

static const char *const SURFACES[] = {
    "umicom.ai.chat",
    "umicom.ai.context",
    "umicom.development.editor",
    "umicom.development.explorer"
};

const UmiDeveloperWorkbenchPerspectiveDefinition *umi_developer_workbench_perspective_ai(void)
{
    static const UmiDeveloperWorkbenchPerspectiveDefinition perspective = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperWorkbenchPerspectiveDefinition),
        .api_version = UMI_DEVELOPER_WORKBENCH_API_VERSION,
        .perspective_id = "developer.perspective.ai",
        .title = "Ai",
        .description =
            "Task-oriented Ai tool selection owned by Umicom Framework.",
        .surface_ids = SURFACES,
        .surface_count = sizeof(SURFACES) / sizeof(SURFACES[0]),
        .default_focus_surface_id = "umicom.ai.chat"
    };

    return &perspective;
}
