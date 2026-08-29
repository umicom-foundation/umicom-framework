/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/web-studio/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Web Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot WEB_STUDIO_STANDARD_SLOTS[] = {
    {"umicom.web.page-canvas", "umicom.web.page-canvas", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     5U, 1, 1},
    {"umicom.web.preview", "umicom.web.preview", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U, 1,
     0},
    {"umicom.web.dom-inspector", "umicom.web.dom-inspector", UMI_APPLICATION_COMPONENT_REGION_RIGHT,
     2U, 1, 0},
    {"umicom.shared.web", "umicom.shared.web", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.development.explorer", "umicom.development.explorer",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.development.editor", "umicom.development.editor",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 3U, 1, 0},
    {"umicom.development.build", "umicom.development.build",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe WEB_STUDIO_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.web-studio.standard",
    "org.umicom.web-studio",
    "Umicom Web Studio Standard",
    "The balanced everyday Umicom Web Studio workspace with core creation, inspection and "
    "supporting tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    WEB_STUDIO_STANDARD_SLOTS,
    sizeof(WEB_STUDIO_STANDARD_SLOTS) / sizeof(WEB_STUDIO_STANDARD_SLOTS[0]),
    "org.umicom.experience.web-studio.standard"};
