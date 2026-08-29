/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/mobile-studio/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Mobile Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot MOBILE_STUDIO_LEARNING_SLOTS[] = {
    {"umicom.development.designer", "umicom.development.designer",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U, 1, 1},
    {"umicom.development.explorer", "umicom.development.explorer",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.web.preview", "umicom.web.preview", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1, 0},
};

static const UmiApplicationComponentRecipe MOBILE_STUDIO_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.mobile-studio.learning",
    "org.umicom.mobile-studio",
    "Umicom Mobile Studio Learning",
    "A calm starting layout for learning Umicom Mobile Studio with only the essential surfaces "
    "visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    MOBILE_STUDIO_LEARNING_SLOTS,
    sizeof(MOBILE_STUDIO_LEARNING_SLOTS) / sizeof(MOBILE_STUDIO_LEARNING_SLOTS[0]),
    "org.umicom.experience.mobile-studio.learning"};
