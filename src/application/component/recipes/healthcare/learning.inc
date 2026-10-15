/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/healthcare/learning.inc
 * PURPOSE: Define a portable coordination layout using shared worklist components.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* These layouts expose the common workspace shell. The dedicated access widget
 * owns the synthetic exercise; these generic panels do not claim clinical data. */
static const UmiApplicationComponentRecipeSlot HEALTHCARE_LEARNING_SLOTS[] = {
    {"umicom.shared.tasks", "umicom.shared.tasks",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U, 1, 1},
    {"umicom.shared.audit", "umicom.shared.audit",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1, 0},
};
static const UmiApplicationComponentRecipe HEALTHCARE_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe), UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.healthcare.learning", "org.umicom.healthcare",
    "Umicom Healthcare Learning",
    "Synthetic coordination workspace with shared task and review surfaces.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    HEALTHCARE_LEARNING_SLOTS,
    sizeof(HEALTHCARE_LEARNING_SLOTS) / sizeof(HEALTHCARE_LEARNING_SLOTS[0]),
    "org.umicom.experience.healthcare.learning"};
