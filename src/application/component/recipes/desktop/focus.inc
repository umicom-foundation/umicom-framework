/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/desktop/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Desk using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot DESKTOP_FOCUS_SLOTS[] = {
    {"umicom.shell.dock-canvas", "umicom.shell.dock-canvas",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U, 1, 1},
    {"umicom.shell.notifications", "umicom.shell.notifications",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.shell.command-palette", "umicom.shell.command-palette",
     UMI_APPLICATION_COMPONENT_REGION_FLOATING, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe DESKTOP_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.desktop.focus",
    "org.umicom.desktop",
    "Umicom Desk Focus",
    "A distraction-reduced Umicom Desk layout centred on the main task and its closest context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    DESKTOP_FOCUS_SLOTS,
    sizeof(DESKTOP_FOCUS_SLOTS) / sizeof(DESKTOP_FOCUS_SLOTS[0])};
