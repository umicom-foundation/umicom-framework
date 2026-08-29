/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/llm/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom LLM using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot LLM_LEARNING_SLOTS[] = {
    {"umicom.llm.chat", "umicom.llm.chat", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U, 1, 1},
    {"umicom.llm.runtime", "umicom.llm.runtime", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.llm.model-lab", "umicom.llm.model-lab", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1,
     0},
};

static const UmiApplicationComponentRecipe LLM_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.llm.learning",
    "org.umicom.llm",
    "Umicom LLM Learning",
    "A calm starting layout for learning Umicom LLM with only the essential surfaces visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    LLM_LEARNING_SLOTS,
    sizeof(LLM_LEARNING_SLOTS) / sizeof(LLM_LEARNING_SLOTS[0])};
