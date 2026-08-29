/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/ide/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom IDE using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot IDE_LEARNING_SLOTS[] = {
    {"umicom.development.editor", "umicom.development.editor",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U, 1, 1},
    {"umicom.development.explorer", "umicom.development.explorer",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.development.terminal", "umicom.development.terminal",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe IDE_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.ide.learning",
    "org.umicom.ide",
    "Umicom IDE Learning",
    "A calm starting layout for learning Umicom IDE with only the essential surfaces visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    IDE_LEARNING_SLOTS,
    sizeof(IDE_LEARNING_SLOTS) / sizeof(IDE_LEARNING_SLOTS[0]),
    "org.umicom.experience.ide.learning"};
