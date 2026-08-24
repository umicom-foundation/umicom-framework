/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/studio_runtime/activation_profiles/editor.c
 *
 * PURPOSE:
 *   Define Code Editor command activation behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio_runtime/activation_profiles/editor.h"

static const UmiStudioRuntimeActivationRule RULES[] = {
    { .command_pattern = "ide.open.", .surface = UMI_STUDIO_SURFACE_EDITOR, .prefix_match = 1, .make_visible = 1, .focus = 1 },
    { .command_pattern = "document.", .surface = UMI_STUDIO_SURFACE_EDITOR, .prefix_match = 1, .make_visible = 1, .focus = 1 },
    { .command_pattern = "editor.", .surface = UMI_STUDIO_SURFACE_EDITOR, .prefix_match = 1, .make_visible = 1, .focus = 1 },
    { .command_pattern = "navigate.file", .surface = UMI_STUDIO_SURFACE_EDITOR, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "navigate.definition", .surface = UMI_STUDIO_SURFACE_EDITOR, .prefix_match = 0, .make_visible = 1, .focus = 1 },
    { .command_pattern = "navigate.references", .surface = UMI_STUDIO_SURFACE_EDITOR, .prefix_match = 0, .make_visible = 1, .focus = 1 }
};

const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_editor(void)
{
    static const UmiStudioRuntimeActivationProfile profile = {
        .profile_id = "umicom.studio.activation.editor",
        .rules = RULES,
        .rule_count = sizeof(RULES) / sizeof(RULES[0])
    };
    return &profile;
}
