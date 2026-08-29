/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/accountant/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Accountant using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot ACCOUNTANT_STANDARD_SLOTS[] = {
    {"umicom.shared.accounting", "umicom.shared.accounting",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 5U, 1, 1},
    {"umicom.shared.payments", "umicom.shared.payments", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U,
     1, 0},
    {"umicom.shared.payroll", "umicom.shared.payroll", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U,
     1, 0},
    {"umicom.shared.hr", "umicom.shared.hr", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.shared.reconciliation", "umicom.shared.reconciliation",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 2U, 1, 0},
    {"umicom.shared.audit", "umicom.shared.audit", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1,
     0},
};

static const UmiApplicationComponentRecipe ACCOUNTANT_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.accountant.standard",
    "org.umicom.accountant",
    "Umicom Accountant Standard",
    "The balanced everyday Umicom Accountant workspace with core creation, inspection and "
    "supporting tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    ACCOUNTANT_STANDARD_SLOTS,
    sizeof(ACCOUNTANT_STANDARD_SLOTS) / sizeof(ACCOUNTANT_STANDARD_SLOTS[0]),
    "org.umicom.experience.accountant.standard"};
