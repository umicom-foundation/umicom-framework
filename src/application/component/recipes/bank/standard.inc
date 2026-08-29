/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/bank/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Bank using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot BANK_STANDARD_SLOTS[] = {
    {"umicom.shared.banking", "umicom.shared.banking", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 5U,
     1, 1},
    {"umicom.shared.payments", "umicom.shared.payments", UMI_APPLICATION_COMPONENT_REGION_LEFT, 2U,
     1, 0},
    {"umicom.shared.positions", "umicom.shared.positions", UMI_APPLICATION_COMPONENT_REGION_RIGHT,
     1U, 1, 0},
    {"umicom.shared.digital-assets", "umicom.shared.digital-assets",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 3U, 1, 0},
    {"umicom.shared.risk", "umicom.shared.risk", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.shared.settlement", "umicom.shared.settlement",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
    {"umicom.shared.collateral", "umicom.shared.collateral",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
    {"umicom.shared.audit", "umicom.shared.audit", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1,
     0},
};

static const UmiApplicationComponentRecipe BANK_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.bank.standard",
    "org.umicom.bank",
    "Umicom Bank Standard",
    "The balanced everyday Umicom Bank workspace with core creation, inspection and supporting "
    "tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    BANK_STANDARD_SLOTS,
    sizeof(BANK_STANDARD_SLOTS) / sizeof(BANK_STANDARD_SLOTS[0])};
