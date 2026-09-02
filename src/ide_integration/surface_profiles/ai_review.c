/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface_profiles/ai_review.c
 *
 * PURPOSE:
 *   Define the AI Patch Review logical IDE surface without physical docking geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface_profiles/ai_review.h"

/*
 * Provide the ide surface profile ai review operation used by this module and its client
 * applications.
 */
const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_ai_review(void)
{
    static const UmiIdeSurfaceDescriptor descriptor = {
        .surface_id = "umicom.ai-developer.patch-review",
        .title = "AI Patch Review",
        .description = "Governed complete-file patch review.",
        .command_id = "ai.developer.open-review",
        .required_capability = "ai",
        .default_region = UMI_IDE_SURFACE_REGION_EDITOR,
        .default_visible = 0,
        .closable = 1,
        .movable = 1,
        .multi_instance = 1,
        .revision = 1U
    };
    return &descriptor;
}
