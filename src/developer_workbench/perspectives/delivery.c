/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/perspectives/delivery.c
 *
 * PURPOSE:
 *   Define the Framework-owned Delivery perspective surface set.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/perspectives/delivery.h"

static const char *const SURFACES[] = {
    "umicom.development.build",
    "umicom.development.testing",
    "umicom.development.source-control",
    "umicom.development.terminal"
};

/*
 * Provide the developer workbench perspective delivery operation used by this module and
 * its client applications.
 */
const UmiDeveloperWorkbenchPerspectiveDefinition *umi_developer_workbench_perspective_delivery(void)
{
    static const UmiDeveloperWorkbenchPerspectiveDefinition perspective = {
        .structure_size =
            (uint32_t)sizeof(UmiDeveloperWorkbenchPerspectiveDefinition),
        .api_version = UMI_DEVELOPER_WORKBENCH_API_VERSION,
        .perspective_id = "developer.perspective.delivery",
        .title = "Delivery",
        .description =
            "Task-oriented Delivery tool selection owned by Umicom Framework.",
        .surface_ids = SURFACES,
        .surface_count = sizeof(SURFACES) / sizeof(SURFACES[0]),
        .default_focus_surface_id = "umicom.development.build"
    };

    return &perspective;
}
