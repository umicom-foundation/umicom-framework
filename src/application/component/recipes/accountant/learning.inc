/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/accountant/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Accountant using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot ACCOUNTANT_LEARNING_SLOTS[] = {
    {"umicom.shared.accounting", "umicom.shared.accounting",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U, 1, 1},
    {"umicom.shared.payments", "umicom.shared.payments", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U,
     1, 0},
    {"umicom.shared.reconciliation", "umicom.shared.reconciliation",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe ACCOUNTANT_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.accountant.learning",
    "org.umicom.accountant",
    "Umicom Accountant Learning",
    "A calm starting layout for learning Umicom Accountant with only the essential surfaces "
    "visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    ACCOUNTANT_LEARNING_SLOTS,
    sizeof(ACCOUNTANT_LEARNING_SLOTS) / sizeof(ACCOUNTANT_LEARNING_SLOTS[0]),
    "org.umicom.experience.accountant.learning"};
