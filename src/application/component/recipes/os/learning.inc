/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/os/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom OS using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot OS_LEARNING_SLOTS[] = {
    {"umicom.os.launcher", "umicom.os.launcher", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U, 1,
     1},
    {"umicom.os.control-centre", "umicom.os.control-centre", UMI_APPLICATION_COMPONENT_REGION_RIGHT,
     1U, 1, 0},
    {"umicom.os.resource-monitor", "umicom.os.resource-monitor",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe OS_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.os.learning",
    "org.umicom.os",
    "Umicom OS Learning",
    "A calm starting layout for learning Umicom OS with only the essential surfaces visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    OS_LEARNING_SLOTS,
    sizeof(OS_LEARNING_SLOTS) / sizeof(OS_LEARNING_SLOTS[0]),
    "org.umicom.experience.os.learning"};
