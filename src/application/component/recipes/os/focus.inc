/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/os/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom OS using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot OS_FOCUS_SLOTS[] = {
    {"umicom.os.launcher", "umicom.os.launcher", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 7U, 1,
     1},
    {"umicom.os.control-centre", "umicom.os.control-centre", UMI_APPLICATION_COMPONENT_REGION_RIGHT,
     2U, 1, 0},
    {"umicom.os.resource-monitor", "umicom.os.resource-monitor",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe OS_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.os.focus",
    "org.umicom.os",
    "Umicom OS Focus",
    "A distraction-reduced Umicom OS layout centred on the main task and its closest context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    OS_FOCUS_SLOTS,
    sizeof(OS_FOCUS_SLOTS) / sizeof(OS_FOCUS_SLOTS[0])};
