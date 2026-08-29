/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/rag/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom RAG using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot RAG_LEARNING_SLOTS[] = {
    {"umicom.rag.search", "umicom.rag.search", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U, 1, 1},
    {"umicom.rag.collections", "umicom.rag.collections", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U,
     1, 0},
    {"umicom.rag.source-reader", "umicom.rag.source-reader", UMI_APPLICATION_COMPONENT_REGION_RIGHT,
     1U, 1, 0},
};

static const UmiApplicationComponentRecipe RAG_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.rag.learning",
    "org.umicom.rag",
    "Umicom RAG Learning",
    "A calm starting layout for learning Umicom RAG with only the essential surfaces visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    RAG_LEARNING_SLOTS,
    sizeof(RAG_LEARNING_SLOTS) / sizeof(RAG_LEARNING_SLOTS[0]),
    "org.umicom.experience.rag.learning"};
