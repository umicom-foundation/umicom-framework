/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/creator/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom AI Creator using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot CREATOR_STANDARD_SLOTS[] = {
    {"umicom.creator.canvas", "umicom.creator.canvas", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 5U,
     1, 1},
    {"umicom.creator.prompt", "umicom.creator.prompt", UMI_APPLICATION_COMPONENT_REGION_LEFT, 2U, 1,
     0},
    {"umicom.creator.storyboard", "umicom.creator.storyboard",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 2U, 1, 0},
    {"umicom.creator.variants", "umicom.creator.variants", UMI_APPLICATION_COMPONENT_REGION_RIGHT,
     1U, 1, 0},
    {"umicom.ai.tools", "umicom.ai.tools", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.ai.context", "umicom.ai.context", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe CREATOR_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.creator.standard",
    "org.umicom.creator",
    "Umicom AI Creator Standard",
    "The balanced everyday Umicom AI Creator workspace with core creation, inspection and "
    "supporting tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    CREATOR_STANDARD_SLOTS,
    sizeof(CREATOR_STANDARD_SLOTS) / sizeof(CREATOR_STANDARD_SLOTS[0])};
