/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/bank/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Bank using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot BANK_LEARNING_SLOTS[] = {
    {"umicom.shared.banking", "umicom.shared.banking", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U,
     1, 1},
    {"umicom.shared.payments", "umicom.shared.payments", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U,
     1, 0},
    {"umicom.shared.positions", "umicom.shared.positions", UMI_APPLICATION_COMPONENT_REGION_RIGHT,
     1U, 1, 0},
};

static const UmiApplicationComponentRecipe BANK_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.bank.learning",
    "org.umicom.bank",
    "Umicom Bank Learning",
    "A calm starting layout for learning Umicom Bank with only the essential surfaces visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    BANK_LEARNING_SLOTS,
    sizeof(BANK_LEARNING_SLOTS) / sizeof(BANK_LEARNING_SLOTS[0])};
