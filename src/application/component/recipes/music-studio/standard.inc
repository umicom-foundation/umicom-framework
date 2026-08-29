/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/music-studio/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Music Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot MUSIC_STUDIO_STANDARD_SLOTS[] = {
    {"umicom.music.arranger", "umicom.music.arranger", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 5U,
     1, 1},
    {"umicom.music.prompt", "umicom.music.prompt", UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.music.generations", "umicom.music.generations", UMI_APPLICATION_COMPONENT_REGION_LEFT,
     1U, 1, 0},
    {"umicom.music.piano-roll", "umicom.music.piano-roll", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     3U, 1, 0},
    {"umicom.music.stems", "umicom.music.stems", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 2U, 1, 0},
    {"umicom.ai.tools", "umicom.ai.tools", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
};

static const UmiApplicationComponentRecipe MUSIC_STUDIO_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.music-studio.standard",
    "org.umicom.music-studio",
    "Umicom Music Studio Standard",
    "The balanced everyday Umicom Music Studio workspace with core creation, inspection and "
    "supporting tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    MUSIC_STUDIO_STANDARD_SLOTS,
    sizeof(MUSIC_STUDIO_STANDARD_SLOTS) / sizeof(MUSIC_STUDIO_STANDARD_SLOTS[0]),
    "org.umicom.experience.music-studio.standard"};
