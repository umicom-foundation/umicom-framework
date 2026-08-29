/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/desktop/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Desk using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot DESKTOP_STANDARD_SLOTS[] = {
    {"umicom.shell.dock-canvas", "umicom.shell.dock-canvas",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 5U, 1, 1},
    {"umicom.shell.activity-bar", "umicom.shell.activity-bar",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.shell.layout-tabs", "umicom.shell.layout-tabs",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 1U, 1, 0},
    {"umicom.shell.workspace-profiles", "umicom.shell.workspace-profiles",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.shell.multi-monitor", "umicom.shell.multi-monitor",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.shell.context-links", "umicom.shell.context-links",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
    {"umicom.shell.notifications", "umicom.shell.notifications",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.shell.command-palette", "umicom.shell.command-palette",
     UMI_APPLICATION_COMPONENT_REGION_FLOATING, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe DESKTOP_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.desktop.standard",
    "org.umicom.desktop",
    "Umicom Desk Standard",
    "The balanced everyday Umicom Desk workspace with core creation, inspection and supporting "
    "tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    DESKTOP_STANDARD_SLOTS,
    sizeof(DESKTOP_STANDARD_SLOTS) / sizeof(DESKTOP_STANDARD_SLOTS[0])};
