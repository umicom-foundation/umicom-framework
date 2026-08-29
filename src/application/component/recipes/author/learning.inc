/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/author/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Author using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot AUTHOR_LEARNING_SLOTS[] = {
    {"umicom.author.editor", "umicom.author.editor", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U,
     1, 1},
    {"umicom.author.outline", "umicom.author.outline", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1,
     0},
    {"umicom.author.research", "umicom.author.research", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U,
     1, 0},
};

static const UmiApplicationComponentRecipe AUTHOR_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.author.learning",
    "org.umicom.author",
    "Umicom Author Learning",
    "A calm starting layout for learning Umicom Author with only the essential surfaces visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    AUTHOR_LEARNING_SLOTS,
    sizeof(AUTHOR_LEARNING_SLOTS) / sizeof(AUTHOR_LEARNING_SLOTS[0])};
