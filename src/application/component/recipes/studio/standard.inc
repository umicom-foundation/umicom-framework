/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/studio/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Studio IDE using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot STUDIO_STANDARD_SLOTS[] = {
    {"umicom.development.editor", "umicom.development.editor",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 5U, 1, 1},
    {"umicom.development.explorer", "umicom.development.explorer",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.development.designer", "umicom.development.designer",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 3U, 1, 0},
    {"umicom.development.build", "umicom.development.build",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
    {"umicom.development.debug", "umicom.development.debug", UMI_APPLICATION_COMPONENT_REGION_RIGHT,
     1U, 1, 0},
    {"umicom.development.testing", "umicom.development.testing",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.development.source-control", "umicom.development.source-control",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.development.terminal", "umicom.development.terminal",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
    {"umicom.ai.chat", "umicom.ai.chat", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.shared.diagnostics", "umicom.shared.diagnostics",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe STUDIO_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.studio.standard",
    "org.umicom.studio",
    "Umicom Studio IDE Standard",
    "The balanced everyday Umicom Studio IDE workspace with core creation, inspection and "
    "supporting tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    STUDIO_STANDARD_SLOTS,
    sizeof(STUDIO_STANDARD_SLOTS) / sizeof(STUDIO_STANDARD_SLOTS[0])};
