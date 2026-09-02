/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/games.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Games.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/games.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

/*
 * Games divides world building, gameplay authoring and debugging into layouts.
 * Scene, asset, behaviour and profiler panels come from reusable engine
 * contracts and may be moved across monitors like any other Framework panel. */

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "world", "World Editor",
      "Scene/world authoring", "umicom.games", "games.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "assets", "Assets",
      "Game assets and resources", "umicom.media", "games.green", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "inspector", "Inspector",
      "Entity/component properties", "umicom.games", "games.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "hierarchy", "Hierarchy",
      "World/entity hierarchy", "umicom.games", "games.green", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "gameplay", "Gameplay",
      "Gameplay systems and rules", "umicom.games", "games.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "game-view", "Game View",
      "Play-test output", "umicom.games", "games.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR },
    { sizeof(UmiExperiencePanelDefinition), "debug", "Debug",
      "Runtime debug/evidence", "umicom.debug", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "profiler", "Profiler",
      "Frame/resource performance", "umicom.metrics", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "output", "Output",
      "Build/runtime output", "umicom.diagnostics", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE }
};

static const char *const LAYOUT_WORLD[] = {"world", "assets", "inspector", "hierarchy"};
static const char *const LAYOUT_GAMEPLAY[] = {"gameplay", "world", "output", "inspector"};
static const char *const LAYOUT_DEBUG[] = {"game-view", "debug", "profiler", "output"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "world", "World",
      "World/scene and asset authoring", LAYOUT_WORLD, COUNT_OF(LAYOUT_WORLD), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "gameplay", "Gameplay",
      "Gameplay systems and simulation", LAYOUT_GAMEPLAY, COUNT_OF(LAYOUT_GAMEPLAY), UMI_EXPERIENCE_LAYOUT_LOCKABLE },
    { sizeof(UmiExperienceLayoutDefinition), "debug", "Debug",
      "Play-test and runtime diagnostics", LAYOUT_DEBUG, COUNT_OF(LAYOUT_DEBUG), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "games.world", "World model",
      "Scenes, entities and components", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.games" },
    { sizeof(UmiExperienceFeatureDefinition), "games.render", "Rendering",
      "Renderer provider boundary", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P3,
      UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER, "umicom.media" },
    { sizeof(UmiExperienceFeatureDefinition), "games.simulation", "Simulation",
      "Game loop/physics provider contracts", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P3,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.games" },
    { sizeof(UmiExperienceFeatureDefinition), "games.tools", "Editor/debug",
      "World editor, play test and debug", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ui" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.games",
    "Umicom Games",
    "world",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

/*
 * Provide the application experience games operation used by this module and its client
 * applications.
 */
const UmiApplicationExperienceDefinition *umi_application_experience_games(void)
{
    return &DEFINITION;
}
