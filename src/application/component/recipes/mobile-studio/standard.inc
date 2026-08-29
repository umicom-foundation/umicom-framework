/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/mobile-studio/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Mobile Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot MOBILE_STUDIO_STANDARD_SLOTS[] = {
    {"umicom.development.designer", "umicom.development.designer",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 5U, 1, 1},
    {"umicom.development.editor", "umicom.development.editor",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U, 1, 0},
    {"umicom.development.explorer", "umicom.development.explorer",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.web.preview", "umicom.web.preview", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1, 0},
    {"umicom.development.build", "umicom.development.build",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
    {"umicom.development.testing", "umicom.development.testing",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.shared.diagnostics", "umicom.shared.diagnostics",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe MOBILE_STUDIO_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.mobile-studio.standard",
    "org.umicom.mobile-studio",
    "Umicom Mobile Studio Standard",
    "The balanced everyday Umicom Mobile Studio workspace with core creation, inspection and "
    "supporting tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    MOBILE_STUDIO_STANDARD_SLOTS,
    sizeof(MOBILE_STUDIO_STANDARD_SLOTS) / sizeof(MOBILE_STUDIO_STANDARD_SLOTS[0]),
    "org.umicom.experience.mobile-studio.standard"};
