/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/media-studio/learning.inc
 *
 * PURPOSE:
 *   Define the learning workspace recipe for Umicom Media Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot MEDIA_STUDIO_LEARNING_SLOTS[] = {
    {"umicom.media.timeline", "umicom.media.timeline", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 4U,
     1, 1},
    {"umicom.media.asset-browser", "umicom.media.asset-browser",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.media.program-monitor", "umicom.media.program-monitor",
     UMI_APPLICATION_COMPONENT_REGION_RIGHT, 2U, 1, 0},
};

static const UmiApplicationComponentRecipe MEDIA_STUDIO_LEARNING_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.media-studio.learning",
    "org.umicom.media-studio",
    "Umicom Media Studio Learning",
    "A calm starting layout for learning Umicom Media Studio with only the essential surfaces "
    "visible.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
    MEDIA_STUDIO_LEARNING_SLOTS,
    sizeof(MEDIA_STUDIO_LEARNING_SLOTS) / sizeof(MEDIA_STUDIO_LEARNING_SLOTS[0]),
    "org.umicom.experience.media-studio.learning"};
