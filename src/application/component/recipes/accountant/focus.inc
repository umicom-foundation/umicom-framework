/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/accountant/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Accountant using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot ACCOUNTANT_FOCUS_SLOTS[] = {
    {"umicom.shared.accounting", "umicom.shared.accounting",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U, 1, 1},
    {"umicom.shared.reconciliation", "umicom.shared.reconciliation",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 2U, 1, 0},
    {"umicom.shared.audit", "umicom.shared.audit", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1,
     0},
};

static const UmiApplicationComponentRecipe ACCOUNTANT_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.accountant.focus",
    "org.umicom.accountant",
    "Umicom Accountant Focus",
    "A distraction-reduced Umicom Accountant layout centred on the main task and its closest "
    "context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    ACCOUNTANT_FOCUS_SLOTS,
    sizeof(ACCOUNTANT_FOCUS_SLOTS) / sizeof(ACCOUNTANT_FOCUS_SLOTS[0]),
    "org.umicom.experience.accountant.focus"};
