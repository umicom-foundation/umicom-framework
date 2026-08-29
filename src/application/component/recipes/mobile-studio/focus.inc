/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/mobile-studio/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Mobile Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot MOBILE_STUDIO_FOCUS_SLOTS[] = {
    {"umicom.development.designer", "umicom.development.designer",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U, 1, 1},
    {"umicom.web.preview", "umicom.web.preview", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 3U, 1, 0},
    {"umicom.development.build", "umicom.development.build",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe MOBILE_STUDIO_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.mobile-studio.focus",
    "org.umicom.mobile-studio",
    "Umicom Mobile Studio Focus",
    "A distraction-reduced Umicom Mobile Studio layout centred on the main task and its closest "
    "context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    MOBILE_STUDIO_FOCUS_SLOTS,
    sizeof(MOBILE_STUDIO_FOCUS_SLOTS) / sizeof(MOBILE_STUDIO_FOCUS_SLOTS[0]),
    "org.umicom.experience.mobile-studio.focus"};
