/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/rag/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom RAG using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot RAG_FOCUS_SLOTS[] = {
    {"umicom.rag.search", "umicom.rag.search", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U, 1, 1},
    {"umicom.rag.source-reader", "umicom.rag.source-reader", UMI_APPLICATION_COMPONENT_REGION_RIGHT,
     2U, 1, 0},
    {"umicom.rag.collections", "umicom.rag.collections", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U,
     1, 0},
};

static const UmiApplicationComponentRecipe RAG_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.rag.focus",
    "org.umicom.rag",
    "Umicom RAG Focus",
    "A distraction-reduced Umicom RAG layout centred on the main task and its closest context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    RAG_FOCUS_SLOTS,
    sizeof(RAG_FOCUS_SLOTS) / sizeof(RAG_FOCUS_SLOTS[0]),
    "org.umicom.experience.rag.focus"};
