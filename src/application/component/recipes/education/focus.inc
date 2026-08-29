/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/education/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Education Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot EDUCATION_FOCUS_SLOTS[] = {
    {"umicom.education.course", "umicom.education.course", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     7U, 1, 1},
    {"umicom.author.outline", "umicom.author.outline", UMI_APPLICATION_COMPONENT_REGION_LEFT, 2U, 1,
     0},
    {"umicom.education.assessment", "umicom.education.assessment",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1, 0},
};

static const UmiApplicationComponentRecipe EDUCATION_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.education.focus",
    "org.umicom.education",
    "Umicom Education Studio Focus",
    "A distraction-reduced Umicom Education Studio layout centred on the main task and its closest "
    "context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    EDUCATION_FOCUS_SLOTS,
    sizeof(EDUCATION_FOCUS_SLOTS) / sizeof(EDUCATION_FOCUS_SLOTS[0]),
    "org.umicom.experience.education.focus"};
