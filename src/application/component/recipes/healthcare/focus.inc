/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/healthcare/focus.inc
 * PURPOSE: Define a portable coordination layout using shared worklist components.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* These layouts expose the common workspace shell. The dedicated access widget
 * owns the synthetic exercise; these generic panels do not claim clinical data. */
static const UmiApplicationComponentRecipeSlot HEALTHCARE_FOCUS_SLOTS[] = {
    {"umicom.shared.tasks", "umicom.shared.tasks",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U, 1, 1},
};
static const UmiApplicationComponentRecipe HEALTHCARE_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe), UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.healthcare.focus", "org.umicom.healthcare",
    "Umicom Healthcare Focus",
    "Synthetic coordination workspace with shared task and review surfaces.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    HEALTHCARE_FOCUS_SLOTS,
    sizeof(HEALTHCARE_FOCUS_SLOTS) / sizeof(HEALTHCARE_FOCUS_SLOTS[0]),
    "org.umicom.experience.healthcare.focus"};
