/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/web-studio/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Web Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot WEB_STUDIO_LEARNING_SLOTS[] = {
    {"umicom.web.page-canvas", "umicom.web.page-canvas", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     4U, 1, 1},
    {"umicom.development.explorer", "umicom.development.explorer",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.web.preview", "umicom.web.preview", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1, 0},
};

static const UmiApplicationComponentRecipe WEB_STUDIO_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.web-studio.learning",
    "org.umicom.web-studio",
    "Umicom Web Studio Learning",
    "A calm starting layout for learning Umicom Web Studio with only the essential surfaces "
    "visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    WEB_STUDIO_LEARNING_SLOTS,
    sizeof(WEB_STUDIO_LEARNING_SLOTS) / sizeof(WEB_STUDIO_LEARNING_SLOTS[0])};
