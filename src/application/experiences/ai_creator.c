/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/ai_creator.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom AI Creator.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/ai_creator.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "prompt", "Prompt Studio",
      "Prompt, constraints and generation intent", "umicom.ai.media-generation", "creator.orange", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "model-router", "Model Router",
      "Provider/model selection and policy", "umicom.ai", "ai.violet", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "references", "Reference Library",
      "Images, audio, video and style references", "umicom.media", "creator.orange", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "candidates", "Candidate Gallery",
      "Generated candidate assets and versions", "umicom.ai.media-generation", "creator.orange", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "storyboard", "Storyboard",
      "Scenes, shots and continuity", "umicom.media", "creator.orange", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "compare", "Compare",
      "A/B and side-by-side generated output", "umicom.media", "creator.orange", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "provenance", "Provenance",
      "Model, source, consent and generation evidence", "umicom.audit", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE },
    { sizeof(UmiExperiencePanelDefinition), "export", "Export",
      "Asset delivery and handoff", "umicom.delivery", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE }
};

static const char *const LAYOUT_CREATE[] = {"prompt", "model-router", "references", "candidates", "provenance"};
static const char *const LAYOUT_STORYBOARD[] = {"storyboard", "references", "candidates", "provenance"};
static const char *const LAYOUT_REVIEW[] = {"candidates", "compare", "provenance", "export"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "create", "Create",
      "Prompt, references and generation candidates", LAYOUT_CREATE, COUNT_OF(LAYOUT_CREATE), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED },
    { sizeof(UmiExperienceLayoutDefinition), "storyboard", "Storyboard",
      "Storyboard and shot/scene composition", LAYOUT_STORYBOARD, COUNT_OF(LAYOUT_STORYBOARD), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "review", "Review",
      "Compare generations and approve outputs", LAYOUT_REVIEW, COUNT_OF(LAYOUT_REVIEW), UMI_EXPERIENCE_LAYOUT_LOCKABLE }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "creator.image", "Image generation",
      "Provider-neutral image generation", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai.media-generation" },
    { sizeof(UmiExperienceFeatureDefinition), "creator.video", "Video generation",
      "Provider-neutral video generation", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai.media-generation" },
    { sizeof(UmiExperienceFeatureDefinition), "creator.storyboard", "Storyboard generation",
      "Structured multi-shot planning", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.media" },
    { sizeof(UmiExperienceFeatureDefinition), "creator.references", "Reference conditioning",
      "Reusable reference assets and policy", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.media" },
    { sizeof(UmiExperienceFeatureDefinition), "creator.provenance", "Generation provenance",
      "Model/version/source/consent evidence", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.audit" },
    { sizeof(UmiExperienceFeatureDefinition), "creator.providers", "Provider routing",
      "Local/cloud provider registry and routing", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai" },
    { sizeof(UmiExperienceFeatureDefinition), "creator.review", "Candidate review workflow",
      "Non-destructive review and approval", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.workflow" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.creator",
    "Umicom AI Creator",
    "create",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_ai_creator(void)
{
    return &DEFINITION;
}
