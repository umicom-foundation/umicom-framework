/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/perspective.c
 *
 * PURPOSE:
 *   Define the task-oriented AI Developer perspective while keeping physical
 *   window placement separate, as required by Developer Workbench architecture.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/perspective.h"

static const char *const SURFACES[] = {
    "umicom.ai-developer.overview",
    "umicom.ai-developer.chat",
    "umicom.ai-developer.tasks",
    "umicom.ai-developer.approvals",
    "umicom.ai-developer.patch-review",
    "umicom.ai-developer.diff",
    "umicom.ai-developer.tool-activity",
    "umicom.ai-developer.validation",
    "umicom.ai-developer.context",
    "umicom.ai-developer.history",
    "umicom.ai-developer.policy",
    "umicom.ai-developer.checkpoints"
};

const UmiDeveloperWorkbenchPerspectiveDefinition *
umi_ai_developer_perspective_definition(void)
{
    static const UmiDeveloperWorkbenchPerspectiveDefinition perspective = {
        .structure_size = (uint32_t)sizeof(perspective),
        .api_version = 1U,
        .perspective_id = "umicom.perspective.ai-developer",
        .title = "AI Developer",
        .description =
            "Repository-aware coding, controlled tools, approvals, patch review and validation.",
        .surface_ids = SURFACES,
        .surface_count = sizeof(SURFACES) / sizeof(SURFACES[0]),
        .default_focus_surface_id = "umicom.ai-developer.chat"
    };

    return &perspective;
}
