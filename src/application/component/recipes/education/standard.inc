/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/education/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Education Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot EDUCATION_STANDARD_SLOTS[] = {
    {"umicom.education.course", "umicom.education.course", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     6U, 1, 1},
    {"umicom.education.assessment", "umicom.education.assessment",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1, 0},
    {"umicom.author.editor", "umicom.author.editor", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 3U,
     1, 0},
    {"umicom.author.outline", "umicom.author.outline", UMI_APPLICATION_COMPONENT_REGION_LEFT, 2U, 1,
     0},
    {"umicom.media.asset-browser", "umicom.media.asset-browser",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.shared.tasks", "umicom.shared.tasks", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1,
     0},
    {"umicom.shared.audit", "umicom.shared.audit", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1,
     0},
};

static const UmiApplicationComponentRecipe EDUCATION_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.education.standard",
    "org.umicom.education",
    "Umicom Education Studio Standard",
    "The balanced everyday Umicom Education Studio workspace with core creation, inspection and "
    "supporting tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    EDUCATION_STANDARD_SLOTS,
    sizeof(EDUCATION_STANDARD_SLOTS) / sizeof(EDUCATION_STANDARD_SLOTS[0]),
    "org.umicom.experience.education.standard"};
