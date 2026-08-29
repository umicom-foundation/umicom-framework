/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/author/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Author using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot AUTHOR_FOCUS_SLOTS[] = {
    {"umicom.author.editor", "umicom.author.editor", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 7U,
     1, 1},
    {"umicom.author.outline", "umicom.author.outline", UMI_APPLICATION_COMPONENT_REGION_LEFT, 2U, 1,
     0},
    {"umicom.author.research", "umicom.author.research", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U,
     1, 0},
};

static const UmiApplicationComponentRecipe AUTHOR_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.author.focus",
    "org.umicom.author",
    "Umicom Author Focus",
    "A distraction-reduced Umicom Author layout centred on the main task and its closest context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    AUTHOR_FOCUS_SLOTS,
    sizeof(AUTHOR_FOCUS_SLOTS) / sizeof(AUTHOR_FOCUS_SLOTS[0])};
