/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface_profiles/ai_validation.c
 *
 * PURPOSE:
 *   Define the AI Validation logical IDE surface without physical docking geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface_profiles/ai_validation.h"

/*
 * Provide the ide surface profile ai validation operation used by this module and its
 * client applications.
 */
const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_ai_validation(void)
{
    static const UmiIdeSurfaceDescriptor descriptor = {
        .surface_id = "umicom.ai-developer.validation",
        .title = "AI Validation",
        .description = "Build/test validation evidence for AI changes.",
        .command_id = "ai.developer.open-validation",
        .required_capability = "ai",
        .default_region = UMI_IDE_SURFACE_REGION_BOTTOM_PANEL,
        .default_visible = 0,
        .closable = 1,
        .movable = 1,
        .multi_instance = 0,
        .revision = 1U
    };
    return &descriptor;
}
