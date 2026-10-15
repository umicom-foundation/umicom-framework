/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/healthcare/standard.inc
 * PURPOSE: Define a portable coordination layout using shared worklist components.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* These layouts expose the common workspace shell. The dedicated access widget
 * owns the synthetic exercise; these generic panels do not claim clinical data. */
static const UmiApplicationComponentRecipeSlot HEALTHCARE_STANDARD_SLOTS[] = {
    {"umicom.shared.tasks", "umicom.shared.tasks",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U, 1, 1},
    {"umicom.shared.audit", "umicom.shared.audit",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1, 0},
};
static const UmiApplicationComponentRecipe HEALTHCARE_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe), UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.healthcare.standard", "org.umicom.healthcare",
    "Umicom Healthcare Standard",
    "Synthetic coordination workspace with shared task and review surfaces.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    HEALTHCARE_STANDARD_SLOTS,
    sizeof(HEALTHCARE_STANDARD_SLOTS) / sizeof(HEALTHCARE_STANDARD_SLOTS[0]),
    "org.umicom.experience.healthcare.standard"};
