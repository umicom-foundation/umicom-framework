/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/media-studio/focus.inc
 *
 * PURPOSE:
 *   Define the focus workspace recipe for Umicom Media Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot MEDIA_STUDIO_FOCUS_SLOTS[] = {
    {"umicom.media.timeline", "umicom.media.timeline", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 6U,
     1, 1},
    {"umicom.media.program-monitor", "umicom.media.program-monitor",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 3U, 1, 0},
    {"umicom.media.audio-mixer", "umicom.media.audio-mixer",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 2U, 1, 0},
};

static const UmiApplicationComponentRecipe MEDIA_STUDIO_FOCUS_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.media-studio.focus",
    "org.umicom.media-studio",
    "Umicom Media Studio Focus",
    "A distraction-reduced Umicom Media Studio layout centred on the main task and its closest "
    "context.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_FOCUS,
    MEDIA_STUDIO_FOCUS_SLOTS,
    sizeof(MEDIA_STUDIO_FOCUS_SLOTS) / sizeof(MEDIA_STUDIO_FOCUS_SLOTS[0])};
