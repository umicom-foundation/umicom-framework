/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/tms/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom TMS using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot TMS_FOCUS_SLOTS[] = {
    {"umicom.treasury.pricing", "umicom.treasury.pricing", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     6U, 1, 1},
    {"umicom.treasury.trade-blotter", "umicom.treasury.trade-blotter",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 2U, 1, 0},
    {"umicom.treasury.risk", "umicom.treasury.risk", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1,
     0},
};

static const UmiApplicationComponentRecipe TMS_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.tms.focus",
    "org.umicom.tms",
    "Umicom TMS Focus",
    "A distraction-reduced Umicom TMS layout centred on the main task and its closest context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    TMS_FOCUS_SLOTS,
    sizeof(TMS_FOCUS_SLOTS) / sizeof(TMS_FOCUS_SLOTS[0]),
    "org.umicom.experience.tms.focus"};
