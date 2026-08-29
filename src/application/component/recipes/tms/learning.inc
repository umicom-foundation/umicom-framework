/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/tms/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom TMS using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot TMS_LEARNING_SLOTS[] = {
    {"umicom.treasury.trade-blotter", "umicom.treasury.trade-blotter",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U, 1, 1},
    {"umicom.treasury.workflow", "umicom.treasury.workflow", UMI_APPLICATION_COMPONENT_REGION_LEFT,
     1U, 1, 0},
    {"umicom.treasury.risk", "umicom.treasury.risk", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1,
     0},
};

static const UmiApplicationComponentRecipe TMS_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.tms.learning",
    "org.umicom.tms",
    "Umicom TMS Learning",
    "A calm starting layout for learning Umicom TMS with only the essential surfaces visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    TMS_LEARNING_SLOTS,
    sizeof(TMS_LEARNING_SLOTS) / sizeof(TMS_LEARNING_SLOTS[0]),
    "org.umicom.experience.tms.learning"};
