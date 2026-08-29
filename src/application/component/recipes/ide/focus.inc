/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/ide/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom IDE using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot IDE_FOCUS_SLOTS[] = {
    {"umicom.development.editor", "umicom.development.editor",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U, 1, 1},
    {"umicom.development.explorer", "umicom.development.explorer",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.shared.diagnostics", "umicom.shared.diagnostics",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe IDE_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.ide.focus",
    "org.umicom.ide",
    "Umicom IDE Focus",
    "A distraction-reduced Umicom IDE layout centred on the main task and its closest context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    IDE_FOCUS_SLOTS,
    sizeof(IDE_FOCUS_SLOTS) / sizeof(IDE_FOCUS_SLOTS[0]),
    "org.umicom.experience.ide.focus"};
