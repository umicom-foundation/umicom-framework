/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/bank/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Bank using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot BANK_FOCUS_SLOTS[] = {
    {"umicom.shared.banking", "umicom.shared.banking", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U,
     1, 1},
    {"umicom.shared.payments", "umicom.shared.payments", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U,
     1, 0},
    {"umicom.shared.audit", "umicom.shared.audit", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1,
     0},
};

static const UmiApplicationComponentRecipe BANK_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.bank.focus",
    "org.umicom.bank",
    "Umicom Bank Focus",
    "A distraction-reduced Umicom Bank layout centred on the main task and its closest context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    BANK_FOCUS_SLOTS,
    sizeof(BANK_FOCUS_SLOTS) / sizeof(BANK_FOCUS_SLOTS[0])};
