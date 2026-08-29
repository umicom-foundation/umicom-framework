/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/llm/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom LLM using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot LLM_FOCUS_SLOTS[] = {
    {"umicom.llm.chat", "umicom.llm.chat", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U, 1, 1},
    {"umicom.ai.context", "umicom.ai.context", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.llm.runtime", "umicom.llm.runtime", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe LLM_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.llm.focus",
    "org.umicom.llm",
    "Umicom LLM Focus",
    "A distraction-reduced Umicom LLM layout centred on the main task and its closest context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    LLM_FOCUS_SLOTS,
    sizeof(LLM_FOCUS_SLOTS) / sizeof(LLM_FOCUS_SLOTS[0])};
