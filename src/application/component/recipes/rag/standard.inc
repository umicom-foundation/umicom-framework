/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/rag/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom RAG using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot RAG_STANDARD_SLOTS[] = {
    {"umicom.rag.search", "umicom.rag.search", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 5U, 1, 1},
    {"umicom.rag.collections", "umicom.rag.collections", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U,
     1, 0},
    {"umicom.rag.sources", "umicom.rag.sources", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.rag.pipeline", "umicom.rag.pipeline", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 2U, 1,
     0},
    {"umicom.rag.source-reader", "umicom.rag.source-reader", UMI_APPLICATION_COMPONENT_REGION_RIGHT,
     2U, 1, 0},
    {"umicom.ai.context", "umicom.ai.context", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.ai.evaluation", "umicom.ai.evaluation", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1,
     0},
};

static const UmiApplicationComponentRecipe RAG_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.rag.standard",
    "org.umicom.rag",
    "Umicom RAG Standard",
    "The balanced everyday Umicom RAG workspace with core creation, inspection and supporting "
    "tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    RAG_STANDARD_SLOTS,
    sizeof(RAG_STANDARD_SLOTS) / sizeof(RAG_STANDARD_SLOTS[0]),
    "org.umicom.experience.rag.standard"};
