/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/media.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Media Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/media.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

/*
 * Media follows editing, direction and delivery stages of a production.
 * Source, timeline, effects, audio and export panels remain reusable media
 * components suitable for customised and multi-monitor workspaces. */

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "media-browser", "Media Browser",
      "Project assets and bins", "umicom.media", "media.orange", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "source-monitor", "Source Monitor",
      "Source review and marking", "umicom.media", "media.orange", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "program-monitor", "Program Monitor",
      "Timeline output preview", "umicom.media", "media.orange", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR },
    { sizeof(UmiExperiencePanelDefinition), "timeline", "Edit Timeline",
      "Video/audio tracks and edits", "umicom.media", "media.orange", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "inspector", "Inspector",
      "Clip/effect/generation properties", "umicom.ui", "media.orange", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "audio", "Audio Mixer",
      "Track/bus audio controls", "umicom.media", "media.orange", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "film-director", "AI Film Director",
      "Prompt/reference/camera/shot generation", "umicom.ai.media-generation", "creator.orange", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "storyboard", "Storyboard",
      "Scenes, shots and continuity", "umicom.media", "creator.orange", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "references", "Reference Library",
      "Cast/location/style references", "umicom.media", "creator.orange", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "candidates", "Generated Takes",
      "Candidate takes and versions", "umicom.ai.media-generation", "creator.orange", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "continuity", "Continuity",
      "Character/location/style consistency", "umicom.media", "creator.orange", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "colour", "Colour",
      "Colour management and grading", "umicom.media", "media.orange", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "subtitles", "Subtitles",
      "Captions, localisation and timing", "umicom.media", "media.orange", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "render-queue", "Render Queue",
      "Exports and delivery jobs", "umicom.delivery", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "provenance", "Provenance",
      "Generation/source/consent evidence", "umicom.audit", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE }
};

static const char *const LAYOUT_EDIT[] = {"media-browser", "source-monitor", "program-monitor", "timeline", "inspector", "audio"};
static const char *const LAYOUT_DIRECTOR[] = {"film-director", "storyboard", "references", "candidates", "continuity"};
static const char *const LAYOUT_DELIVER[] = {"colour", "audio", "subtitles", "render-queue", "provenance"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "edit", "Edit",
      "Professional timeline and media editing", LAYOUT_EDIT, COUNT_OF(LAYOUT_EDIT), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "director", "Director",
      "AI film generation and storyboard", LAYOUT_DIRECTOR, COUNT_OF(LAYOUT_DIRECTOR), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "deliver", "Deliver",
      "Colour, audio, QC and render delivery", LAYOUT_DELIVER, COUNT_OF(LAYOUT_DELIVER), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "media.project", "Media project graph",
      "Assets, scenes, timeline and versions", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.media" },
    { sizeof(UmiExperienceFeatureDefinition), "media.timeline", "NLE timeline",
      "Tracks, trims, transitions and keyframes", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.media" },
    { sizeof(UmiExperienceFeatureDefinition), "media.render", "Render pipeline",
      "Decode/encode, render queue and delivery", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.media" },
    { sizeof(UmiExperienceFeatureDefinition), "media.ai-video", "AI video generation",
      "Provider-neutral video generation", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai.media-generation" },
    { sizeof(UmiExperienceFeatureDefinition), "media.storyboard", "Storyboard/continuity",
      "Multi-shot production planning", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.media" },
    { sizeof(UmiExperienceFeatureDefinition), "media.avatar", "Avatar/tutorial/podcast",
      "Consented digital identity/voice workflows", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ai.media-generation" },
    { sizeof(UmiExperienceFeatureDefinition), "media.vfx", "VFX/compositing",
      "Node graph, masks and tracking", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.media" },
    { sizeof(UmiExperienceFeatureDefinition), "media.ui", "Professional UI",
      "GTK multi-monitor editor/director/deliver workspaces", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ui" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.media-studio",
    "Umicom Media Studio",
    "edit",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_media(void)
{
    return &DEFINITION;
}
