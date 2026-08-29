/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/llm/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom LLM using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot LLM_STANDARD_SLOTS[] = {
    {"umicom.llm.chat", "umicom.llm.chat", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 5U, 1, 1},
    {"umicom.llm.runtime", "umicom.llm.runtime", UMI_APPLICATION_COMPONENT_REGION_LEFT, 2U, 1, 0},
    {"umicom.llm.model-lab", "umicom.llm.model-lab", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1,
     0},
    {"umicom.ai.context", "umicom.ai.context", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.ai.tools", "umicom.ai.tools", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.ai.evaluation", "umicom.ai.evaluation", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 2U, 1,
     0},
};

static const UmiApplicationComponentRecipe LLM_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.llm.standard",
    "org.umicom.llm",
    "Umicom LLM Standard",
    "The balanced everyday Umicom LLM workspace with core creation, inspection and supporting "
    "tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    LLM_STANDARD_SLOTS,
    sizeof(LLM_STANDARD_SLOTS) / sizeof(LLM_STANDARD_SLOTS[0]),
    "org.umicom.experience.llm.standard"};
