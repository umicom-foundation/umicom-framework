/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/studio/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Studio IDE using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot STUDIO_FOCUS_SLOTS[] = {
    {"umicom.development.editor", "umicom.development.editor",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U, 1, 1},
    {"umicom.development.explorer", "umicom.development.explorer",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.development.terminal", "umicom.development.terminal",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe STUDIO_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.studio.focus",
    "org.umicom.studio",
    "Umicom Studio IDE Focus",
    "A distraction-reduced Umicom Studio IDE layout centred on the main task and its closest "
    "context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    STUDIO_FOCUS_SLOTS,
    sizeof(STUDIO_FOCUS_SLOTS) / sizeof(STUDIO_FOCUS_SLOTS[0]),
    "org.umicom.experience.studio.focus"};
