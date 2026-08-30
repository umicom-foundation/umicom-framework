/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/author.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Author.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/author.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "sources", "Sources",
      "Research source library", "umicom.ai.retrieval", "author.gold", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "research", "Research",
      "Research workspace", "umicom.ai.research", "author.gold", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "notes", "Notes",
      "Research notes", "umicom.knowledge", "author.gold", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "citations", "Citations",
      "Citation/provenance controls", "umicom.audit", "author.gold", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE },
    { sizeof(UmiExperiencePanelDefinition), "outline", "Outline",
      "Document structure", "umicom.ai.authorengine", "author.gold", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "editor", "Editor",
      "Structured authoring surface", "umicom.ai.authorengine", "author.gold", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "assets", "Assets",
      "Images/media/data assets", "umicom.media", "author.gold", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "ai-assistant", "AI Assistant",
      "Writing/research assistant", "umicom.ai", "ai.violet", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "preview", "Preview",
      "Rendered output preview", "umicom.delivery", "author.gold", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "validation", "Validation",
      "Quality/conformance checks", "umicom.quality", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "delivery", "Delivery",
      "Export/publish pipeline", "umicom.delivery", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "metadata", "Metadata",
      "Publication metadata", "umicom.ai.authorengine", "author.gold", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE }
};

static const char *const LAYOUT_RESEARCH[] = {"sources", "research", "notes", "citations"};
static const char *const LAYOUT_AUTHOR[] = {"outline", "editor", "assets", "ai-assistant"};
static const char *const LAYOUT_PUBLISH[] = {"preview", "validation", "delivery", "metadata"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "research", "Research",
      "Sources, notes and evidence", LAYOUT_RESEARCH, COUNT_OF(LAYOUT_RESEARCH), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "author", "Author",
      "Structured document authoring", LAYOUT_AUTHOR, COUNT_OF(LAYOUT_AUTHOR), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "publish", "Publish",
      "Validation, rendering and distribution", LAYOUT_PUBLISH, COUNT_OF(LAYOUT_PUBLISH), UMI_EXPERIENCE_LAYOUT_LOCKABLE }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "author.research", "Research",
      "Source-backed research workflow", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai.research" },
    { sizeof(UmiExperienceFeatureDefinition), "author.document", "Authoring",
      "Structured book/course/site authoring", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai.authorengine" },
    { sizeof(UmiExperienceFeatureDefinition), "author.media", "Media",
      "Image/media integration", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.media" },
    { sizeof(UmiExperienceFeatureDefinition), "author.delivery", "Publishing",
      "Render/export/publish pipeline", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.delivery" },
    { sizeof(UmiExperienceFeatureDefinition), "author.ai", "AI writing",
      "Provider-neutral generation and editing", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.author",
    "Umicom Author",
    "author",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_author(void)
{
    return &DEFINITION;
}
