/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/author/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Author using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot AUTHOR_STANDARD_SLOTS[] = {
    {"umicom.author.editor", "umicom.author.editor", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U,
     1, 1},
    {"umicom.author.outline", "umicom.author.outline", UMI_APPLICATION_COMPONENT_REGION_LEFT, 2U, 1,
     0},
    {"umicom.author.research", "umicom.author.research", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U,
     1, 0},
    {"umicom.author.publish", "umicom.author.publish", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U,
     1, 0},
    {"umicom.rag.source-reader", "umicom.rag.source-reader", UMI_APPLICATION_COMPONENT_REGION_RIGHT,
     1U, 1, 0},
    {"umicom.ai.chat", "umicom.ai.chat", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe AUTHOR_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.author.standard",
    "org.umicom.author",
    "Umicom Author Standard",
    "The balanced everyday Umicom Author workspace with core creation, inspection and supporting "
    "tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    AUTHOR_STANDARD_SLOTS,
    sizeof(AUTHOR_STANDARD_SLOTS) / sizeof(AUTHOR_STANDARD_SLOTS[0]),
    "org.umicom.experience.author.standard"};
