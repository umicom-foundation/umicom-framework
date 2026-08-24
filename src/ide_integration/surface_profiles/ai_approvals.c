/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface_profiles/ai_approvals.c
 *
 * PURPOSE:
 *   Define the AI Approvals logical IDE surface without physical docking geometry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface_profiles/ai_approvals.h"

const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_ai_approvals(void)
{
    static const UmiIdeSurfaceDescriptor descriptor = {
        .surface_id = "umicom.ai-developer.approvals",
        .title = "AI Approvals",
        .description = "Sensitive tool and patch approvals.",
        .command_id = "ai.developer.open-approvals",
        .required_capability = "ai",
        .default_region = UMI_IDE_SURFACE_REGION_SECONDARY_SIDEBAR,
        .default_visible = 0,
        .closable = 1,
        .movable = 1,
        .multi_instance = 0,
        .revision = 1U
    };
    return &descriptor;
}
