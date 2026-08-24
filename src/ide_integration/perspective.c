/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/perspective.c
 *
 * PURPOSE:
 *   Define the Framework integrated-development perspective. Physical docking
 *   remains a layout-engine responsibility.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/perspective.h"

static const char *const SURFACES[] = {
    "umicom.ide.explorer",
    "umicom.ide.editor",
    "umicom.ide.outline",
    "umicom.ide.search",
    "umicom.ide.problems",
    "umicom.ide.output",
    "umicom.ide.terminal",
    "umicom.ide.source-control",
    "umicom.ide.test-explorer",
    "umicom.ide.debug",
    "umicom.ide.build",
    "umicom.ai-developer.chat",
    "umicom.ai-developer.patch-review",
    "umicom.ai-developer.approvals",
    "umicom.ai-developer.tool-activity",
    "umicom.ai-developer.validation"
};

const UmiDeveloperWorkbenchPerspectiveDefinition *
umi_ide_integrated_development_perspective(void)
{
    static const UmiDeveloperWorkbenchPerspectiveDefinition perspective = {
        .structure_size = (uint32_t)sizeof(perspective),
        .api_version = 1U,
        .perspective_id = "umicom.perspective.integrated-development",
        .title = "Integrated Development",
        .description =
            "Editor, Problems, Tests, Source Control, Debug, Terminal, Build and AI Developer.",
        .surface_ids = SURFACES,
        .surface_count = sizeof(SURFACES) / sizeof(SURFACES[0]),
        .default_focus_surface_id = "umicom.ide.editor"
    };

    return &perspective;
}
