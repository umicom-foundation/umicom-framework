/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/music.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Music Studio.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/music.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "ai-create", "AI Create",
      "Prompt/lyrics/song generation", "umicom.ai.media-generation", "music.violet", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "timeline", "Arrangement Timeline",
      "Multitrack audio/MIDI arrangement", "umicom.media", "music.violet", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "ideas", "Ideas / Session",
      "Non-linear scene and clip grid", "umicom.media", "music.violet", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "lyrics", "Lyrics & Vocals",
      "Lyrics, takes and voice profiles", "umicom.media", "music.violet", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "piano-roll", "Piano Roll",
      "MIDI notes, chords and expression", "umicom.media", "music.violet", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "stem-lab", "Stem Lab",
      "Separation, reconstruction and alternates", "umicom.ai.media-generation", "music.violet", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "mixer", "Mixer",
      "Channels, buses, sends and devices", "umicom.media", "music.violet", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR },
    { sizeof(UmiExperiencePanelDefinition), "mastering", "Mastering",
      "Loudness, reference and master chain", "umicom.media", "music.violet", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "automation", "Automation",
      "Parameter and mix automation", "umicom.media", "music.violet", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "browser", "Media Browser",
      "Samples, instruments and assets", "umicom.media", "music.violet", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE },
    { sizeof(UmiExperiencePanelDefinition), "inspector", "Inspector",
      "Track/clip/generation properties", "umicom.ui", "music.violet", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "export", "Export",
      "Mix, stems, MIDI and project bundle", "umicom.delivery", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE }
};

static const char *const LAYOUT_COMPOSE[] = {"ai-create", "timeline", "lyrics", "piano-roll", "browser"};
static const char *const LAYOUT_ARRANGE[] = {"timeline", "ideas", "stem-lab", "browser", "inspector"};
static const char *const LAYOUT_MASTER[] = {"mixer", "mastering", "automation", "browser", "export"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "compose", "Compose",
      "AI create, lyrics, MIDI and arrangement", LAYOUT_COMPOSE, COUNT_OF(LAYOUT_COMPOSE), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "arrange", "Arrange",
      "Timeline, ideas/session grid and stems", LAYOUT_ARRANGE, COUNT_OF(LAYOUT_ARRANGE), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "master", "Master",
      "Mixer, automation and delivery", LAYOUT_MASTER, COUNT_OF(LAYOUT_MASTER), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "music.project", "Music project graph",
      "Tracks, clips, assets and undo", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.media" },
    { sizeof(UmiExperienceFeatureDefinition), "music.audio", "Audio engine",
      "Device, transport and real-time graph", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.media.audio" },
    { sizeof(UmiExperienceFeatureDefinition), "music.midi", "MIDI",
      "Devices, clips and Piano Roll", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.media.midi" },
    { sizeof(UmiExperienceFeatureDefinition), "music.mixer", "Mixer/plugins",
      "Mixer, plugin graph and automation", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.media" },
    { sizeof(UmiExperienceFeatureDefinition), "music.ai", "Generative audio",
      "Song/stem/audio-to-audio providers", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai.media-generation" },
    { sizeof(UmiExperienceFeatureDefinition), "music.stems", "Stem workflows",
      "Separation, replace/extend and alternates", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai.media-generation" },
    { sizeof(UmiExperienceFeatureDefinition), "music.session", "Ideas/session grid",
      "Non-linear performance/arrangement", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.media" },
    { sizeof(UmiExperienceFeatureDefinition), "music.ui", "Production UI",
      "GTK/web layouts and multi-monitor workspaces", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ui" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.music-studio",
    "Umicom Music Studio",
    "compose",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_music(void)
{
    return &DEFINITION;
}
