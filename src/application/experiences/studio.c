/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/studio.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom Studio IDE.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/studio.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "resource-explorer", "Resource Explorer",
      "Project, folders and resources", "umicom.workspace", "development.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "editor", "Editor",
      "Source and document editing", "umicom.editor", "development.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "problems", "Problems",
      "Diagnostics and issues", "umicom.diagnostics", "development.blue", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "output", "Output",
      "Build, test and runtime output", "umicom.diagnostics", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE },
    { sizeof(UmiExperiencePanelDefinition), "terminal", "Terminal",
      "Framework terminal and process sessions", "umicom.terminal", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "source-control", "Source Control",
      "VCS status, history and changes", "umicom.vcs", "development.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "test-explorer", "Test Explorer",
      "Tests and coverage", "umicom.testing", "development.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "debug", "Debug",
      "Breakpoints, stack, variables and watches", "umicom.debug", "development.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "ai-assistant", "AI Assistant",
      "Coding and research assistant", "umicom.ai", "ai.violet", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "knowledge", "Knowledge",
      "RAG sources and project knowledge", "umicom.ai.retrieval", "ai.violet", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "context-inspector", "Context Inspector",
      "Selection and linked context", "umicom.application.federation", "development.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "quality", "Quality Centre",
      "Architecture, duplicate and quality evidence", "umicom.quality", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE }
};

static const char *const LAYOUT_DEVELOPMENT[] = {"resource-explorer", "editor", "problems", "output", "terminal", "source-control", "test-explorer", "debug"};
static const char *const LAYOUT_REVIEW[] = {"resource-explorer", "editor", "source-control", "problems", "quality", "context-inspector"};
static const char *const LAYOUT_AI_ASSISTED[] = {"resource-explorer", "editor", "ai-assistant", "knowledge", "context-inspector", "output"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "development", "Development",
      "Coding, build, test and debug workbench", LAYOUT_DEVELOPMENT, COUNT_OF(LAYOUT_DEVELOPMENT), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "review", "Review",
      "Source review, diff, quality and diagnostics", LAYOUT_REVIEW, COUNT_OF(LAYOUT_REVIEW), UMI_EXPERIENCE_LAYOUT_LOCKABLE },
    { sizeof(UmiExperienceLayoutDefinition), "ai-assisted", "AI Assisted",
      "Editor, knowledge and AI collaboration", LAYOUT_AI_ASSISTED, COUNT_OF(LAYOUT_AI_ASSISTED), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "studio.workspace", "Workspace/project lifecycle",
      "Open, persist and recover projects", UMI_EXPERIENCE_FEATURE_VERIFIED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.workspace" },
    { sizeof(UmiExperienceFeatureDefinition), "studio.editor", "Professional editor platform",
      "Split editor, navigation and document state", UMI_EXPERIENCE_FEATURE_VERIFIED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.editor" },
    { sizeof(UmiExperienceFeatureDefinition), "studio.build-test-debug", "Build/test/debug workflow",
      "Unified developer pipeline", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.build" },
    { sizeof(UmiExperienceFeatureDefinition), "studio.vcs", "Source control",
      "Diff, history and repository operations", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.vcs" },
    { sizeof(UmiExperienceFeatureDefinition), "studio.visual-designer", "Visual application designer",
      "Declarative form/page authoring", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.designer" },
    { sizeof(UmiExperienceFeatureDefinition), "studio.ai", "AI/RAG coding assistant",
      "Provider-neutral AI and knowledge tools", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai" },
    { sizeof(UmiExperienceFeatureDefinition), "studio.languages", "Polyglot language parity",
      "C23, C++, ASM, CPython and later languages", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.developer.languages" },
    { sizeof(UmiExperienceFeatureDefinition), "studio.multi-monitor", "Multi-monitor workbench recovery",
      "Persist and recover complex layouts", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ui" },
    { sizeof(UmiExperienceFeatureDefinition), "studio.frontend-parity", "Qt/Wt frontend parity",
      "Alternative frontend adapters", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P4,
      UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER, "umicom.frontend" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.studio",
    "Umicom Studio IDE",
    "development",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_studio(void)
{
    return &DEFINITION;
}
