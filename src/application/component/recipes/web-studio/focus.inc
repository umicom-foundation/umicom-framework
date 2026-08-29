/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/web-studio/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Web Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot WEB_STUDIO_FOCUS_SLOTS[] = {
    {"umicom.web.page-canvas", "umicom.web.page-canvas", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     6U, 1, 1},
    {"umicom.web.preview", "umicom.web.preview", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 3U, 1, 0},
    {"umicom.web.dom-inspector", "umicom.web.dom-inspector",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe WEB_STUDIO_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.web-studio.focus",
    "org.umicom.web-studio",
    "Umicom Web Studio Focus",
    "A distraction-reduced Umicom Web Studio layout centred on the main task and its closest "
    "context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    WEB_STUDIO_FOCUS_SLOTS,
    sizeof(WEB_STUDIO_FOCUS_SLOTS) / sizeof(WEB_STUDIO_FOCUS_SLOTS[0])};
