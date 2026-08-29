/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/os/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom OS using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot OS_STANDARD_SLOTS[] = {
    {"umicom.os.launcher", "umicom.os.launcher", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U, 1,
     1},
    {"umicom.os.control-centre", "umicom.os.control-centre", UMI_APPLICATION_COMPONENT_REGION_RIGHT,
     2U, 1, 0},
    {"umicom.os.resource-monitor", "umicom.os.resource-monitor",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 2U, 1, 0},
    {"umicom.shell.multi-monitor", "umicom.shell.multi-monitor",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.shell.notifications", "umicom.shell.notifications",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
    {"umicom.shared.operations", "umicom.shared.operations", UMI_APPLICATION_COMPONENT_REGION_LEFT,
     1U, 1, 0},
    {"umicom.shared.security", "umicom.shared.security", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U,
     1, 0},
};

static const UmiApplicationComponentRecipe OS_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.os.standard",
    "org.umicom.os",
    "Umicom OS Standard",
    "The balanced everyday Umicom OS workspace with core creation, inspection and supporting "
    "tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    OS_STANDARD_SLOTS,
    sizeof(OS_STANDARD_SLOTS) / sizeof(OS_STANDARD_SLOTS[0])};
