/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface_profiles/ai_chat.c
 *
 * PURPOSE:
 *   Define the AI Coding Chat logical IDE surface without physical docking geometry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface_profiles/ai_chat.h"

const UmiIdeSurfaceDescriptor *umi_ide_surface_profile_ai_chat(void)
{
    static const UmiIdeSurfaceDescriptor descriptor = {
        .surface_id = "umicom.ai-developer.chat",
        .title = "AI Coding Chat",
        .description = "Repository-aware coding conversation.",
        .command_id = "ai.developer.open-chat",
        .required_capability = "ai",
        .default_region = UMI_IDE_SURFACE_REGION_SECONDARY_SIDEBAR,
        .default_visible = 0,
        .closable = 1,
        .movable = 1,
        .multi_instance = 1,
        .revision = 1U
    };
    return &descriptor;
}
