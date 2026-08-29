/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/music-studio/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Music Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot MUSIC_STUDIO_LEARNING_SLOTS[] = {
    {"umicom.music.arranger", "umicom.music.arranger", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U,
     1, 1},
    {"umicom.music.prompt", "umicom.music.prompt", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.music.stems", "umicom.music.stems", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe MUSIC_STUDIO_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.music-studio.learning",
    "org.umicom.music-studio",
    "Umicom Music Studio Learning",
    "A calm starting layout for learning Umicom Music Studio with only the essential surfaces "
    "visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    MUSIC_STUDIO_LEARNING_SLOTS,
    sizeof(MUSIC_STUDIO_LEARNING_SLOTS) / sizeof(MUSIC_STUDIO_LEARNING_SLOTS[0]),
    "org.umicom.experience.music-studio.learning"};
