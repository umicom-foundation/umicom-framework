/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/desktop/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Desk using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot DESKTOP_LEARNING_SLOTS[] = {
    {"umicom.shell.dock-canvas", "umicom.shell.dock-canvas",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U, 1, 1},
    {"umicom.shell.activity-bar", "umicom.shell.activity-bar",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.shell.notifications", "umicom.shell.notifications",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe DESKTOP_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.desktop.learning",
    "org.umicom.desktop",
    "Umicom Desk Learning",
    "A calm starting layout for learning Umicom Desk with only the essential surfaces visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    DESKTOP_LEARNING_SLOTS,
    sizeof(DESKTOP_LEARNING_SLOTS) / sizeof(DESKTOP_LEARNING_SLOTS[0]),
    "org.umicom.experience.desktop.learning"};
