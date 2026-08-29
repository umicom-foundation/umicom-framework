/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/creator/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom AI Creator using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot CREATOR_FOCUS_SLOTS[] = {
    {"umicom.creator.canvas", "umicom.creator.canvas", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U,
     1, 1},
    {"umicom.creator.prompt", "umicom.creator.prompt", UMI_APPLICATION_COMPONENT_REGION_LEFT, 2U, 1,
     0},
    {"umicom.creator.variants", "umicom.creator.variants", UMI_APPLICATION_COMPONENT_REGION_RIGHT,
     1U, 1, 0},
};

static const UmiApplicationComponentRecipe CREATOR_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.creator.focus",
    "org.umicom.creator",
    "Umicom AI Creator Focus",
    "A distraction-reduced Umicom AI Creator layout centred on the main task and its closest "
    "context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    CREATOR_FOCUS_SLOTS,
    sizeof(CREATOR_FOCUS_SLOTS) / sizeof(CREATOR_FOCUS_SLOTS[0]),
    "org.umicom.experience.creator.focus"};
