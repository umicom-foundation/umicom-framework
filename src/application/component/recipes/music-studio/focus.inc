/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/music-studio/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Music Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot MUSIC_STUDIO_FOCUS_SLOTS[] = {
    {"umicom.music.arranger", "umicom.music.arranger", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U,
     1, 1},
    {"umicom.music.piano-roll", "umicom.music.piano-roll", UMI_APPLICATION_COMPONENT_REGION_PRIMARY,
     3U, 1, 0},
    {"umicom.music.stems", "umicom.music.stems", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 2U, 1, 0},
};

static const UmiApplicationComponentRecipe MUSIC_STUDIO_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.music-studio.focus",
    "org.umicom.music-studio",
    "Umicom Music Studio Focus",
    "A distraction-reduced Umicom Music Studio layout centred on the main task and its closest "
    "context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    MUSIC_STUDIO_FOCUS_SLOTS,
    sizeof(MUSIC_STUDIO_FOCUS_SLOTS) / sizeof(MUSIC_STUDIO_FOCUS_SLOTS[0])};
