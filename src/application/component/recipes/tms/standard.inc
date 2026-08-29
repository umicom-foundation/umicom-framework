/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/tms/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom TMS using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot TMS_STANDARD_SLOTS[] = {
    {"umicom.treasury.trade-blotter", "umicom.treasury.trade-blotter",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 5U, 1, 1},
    {"umicom.treasury.trade-capture", "umicom.treasury.trade-capture",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1, 0},
    {"umicom.treasury.market-data", "umicom.treasury.market-data",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.treasury.pricing", "umicom.treasury.pricing", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     3U, 1, 0},
    {"umicom.treasury.risk", "umicom.treasury.risk", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1,
     0},
    {"umicom.treasury.workflow", "umicom.treasury.workflow", UMI_APPLICATION_COMPONENT_REGION_LEFT,
     1U, 1, 0},
    {"umicom.treasury.settlement", "umicom.treasury.settlement",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
    {"umicom.treasury.accounting", "umicom.treasury.accounting",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe TMS_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.tms.standard",
    "org.umicom.tms",
    "Umicom TMS Standard",
    "The balanced everyday Umicom TMS workspace with core creation, inspection and supporting "
    "tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    TMS_STANDARD_SLOTS,
    sizeof(TMS_STANDARD_SLOTS) / sizeof(TMS_STANDARD_SLOTS[0]),
    "org.umicom.experience.tms.standard"};
