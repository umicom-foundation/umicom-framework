/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/component/recipes/media-studio/standard.inc
 *
 * PURPOSE:
 *   Define the standard workspace recipe for Umicom Media Studio using only
 *   reusable component contracts owned by Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static const UmiApplicationComponentRecipeSlot MEDIA_STUDIO_STANDARD_SLOTS[] = {
    {"umicom.media.timeline", "umicom.media.timeline", UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 5U,
     1, 1},
    {"umicom.media.asset-browser", "umicom.media.asset-browser",
     UMI_APPLICATION_COMPONENT_REGION_LEFT, 1U, 1, 0},
    {"umicom.media.source-monitor", "umicom.media.source-monitor",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 2U, 1, 0},
    {"umicom.media.program-monitor", "umicom.media.program-monitor",
     UMI_APPLICATION_COMPONENT_REGION_PRIMARY, 3U, 1, 0},
    {"umicom.media.effects", "umicom.media.effects", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1,
     0},
    {"umicom.media.color", "umicom.media.color", UMI_APPLICATION_COMPONENT_REGION_RIGHT, 1U, 1, 0},
    {"umicom.media.audio-mixer", "umicom.media.audio-mixer",
     UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 2U, 1, 0},
    {"umicom.media.delivery", "umicom.media.delivery", UMI_APPLICATION_COMPONENT_REGION_BOTTOM, 1U,
     1, 0},
};

static const UmiApplicationComponentRecipe MEDIA_STUDIO_STANDARD_RECIPE = {
    sizeof(UmiApplicationComponentRecipe),
    UMI_APPLICATION_COMPONENT_RECIPE_API_VERSION,
    "org.umicom.workspace.media-studio.standard",
    "org.umicom.media-studio",
    "Umicom Media Studio Standard",
    "The balanced everyday Umicom Media Studio workspace with core creation, inspection and "
    "supporting tools.",
    UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
    MEDIA_STUDIO_STANDARD_SLOTS,
    sizeof(MEDIA_STUDIO_STANDARD_SLOTS) / sizeof(MEDIA_STUDIO_STANDARD_SLOTS[0])};
