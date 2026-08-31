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
    { sizeof(UmiExperiencePanelDefinition), "ai-chat", "Assistant Chat",
      "Contextual conversations and approved agent tasks", "umicom.ai", "ai.violet", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "ai-model-comparison", "Model Comparison",
      "Side-by-side local and remote model answers", "umicom.ai", "ai.violet", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "knowledge", "Knowledge",
      "RAG sources and project knowledge", "umicom.ai.retrieval", "ai.violet", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "context-inspector", "Context Inspector",
      "Selection and linked context", "umicom.application.federation", "development.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "quality", "Quality Centre",
      "Architecture, duplicate and quality evidence", "umicom.quality", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    /* The visual designer remains a separate dockable tool so source and design
     * views can be shown together or placed on different monitors. */
    { sizeof(UmiExperiencePanelDefinition), "visual-designer", "Visual Designer",
      "Components, canvas, hierarchy and property editing", "umicom.designer", "development.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    /* The profiler panel projects Framework sampling evidence without owning a
     * profiler process or embedding provider-specific records. */
    { sizeof(UmiExperiencePanelDefinition), "profiler", "Performance Profiler",
      "CPU, memory, call-tree, hotspot and regression evidence", "umicom.observability.performance", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    /* Database tools use Framework connection profiles and connector adapters,
     * keeping credentials and database drivers outside the panel definition. */
    { sizeof(UmiExperiencePanelDefinition), "database-explorer", "Database Explorer",
      "Connections, schemas, data, queries and migration plans", "umicom.data", "data.teal", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    /* Extension management remains isolated from the editor so untrusted
     * packages can pass trust and permission checks before activation. */
    { sizeof(UmiExperiencePanelDefinition), "extensions", "Extensions",
      "Discover, verify, install, update and quarantine extensions", "umicom.plugin", "operations.green", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    /* Remote development projects terminal and workspace contracts while the
     * chosen connection adapter retains ownership of transport credentials. */
    { sizeof(UmiExperiencePanelDefinition), "remote-development", "Remote Development",
      "Remote hosts, workspaces, toolchains, tunnels and health", "umicom.terminal.remote", "operations.green", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    /* Notebook state belongs to the language runtime and is rendered as a
     * normal movable document surface rather than a special fixed window. */
    { sizeof(UmiExperiencePanelDefinition), "notebook", "Notebook",
      "Executable cells, outputs, diagnostics and document context", "umicom.language.notebook", "development.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    /* Deployment planning is separate from execution, giving users a reviewable
     * target, package and environment plan before any external change occurs. */
    { sizeof(UmiExperiencePanelDefinition), "deployment", "Deployment Centre",
      "Targets, packages, environment checks and deployment evidence", "umicom.delivery", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    /* Collaboration projects presence and review state without granting another
     * participant permission to execute local commands or agent operations. */
    { sizeof(UmiExperiencePanelDefinition), "collaboration", "Collaboration",
      "Presence, shared review context, comments and session permissions", "umicom.workbench.collaboration", "development.blue", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    /* Engine Explorer uses the Framework engine catalogue as one truthful list
     * of reusable project building blocks and their current maturity. */
    { sizeof(UmiExperiencePanelDefinition), "engine-explorer", "Engine Explorer",
      "Reusable game, 3D, chart, document, media, trading, social and data engines", "umicom.engine", "development.blue", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED }
};

static const char *const LAYOUT_DEVELOPMENT[] = {"resource-explorer", "editor", "problems", "output", "terminal", "source-control", "test-explorer", "debug"};
static const char *const LAYOUT_REVIEW[] = {"resource-explorer", "editor", "source-control", "problems", "quality", "context-inspector"};
static const char *const LAYOUT_AI_ASSISTED[] = {"resource-explorer", "editor", "ai-chat", "ai-assistant", "ai-model-comparison", "knowledge", "context-inspector", "output"};
/* Engine development keeps reusable components, design and profiling visible. */
static const char *const LAYOUT_ENGINE_DEVELOPMENT[] = {"resource-explorer", "engine-explorer", "editor", "visual-designer", "profiler", "output"};
/* Data development groups connections, code, terminal and diagnostics. */
static const char *const LAYOUT_DATA_DEVELOPMENT[] = {"database-explorer", "editor", "terminal", "problems", "output"};
/* Remote development makes transport health and normal development tools peers. */
static const char *const LAYOUT_REMOTE_DEVELOPMENT[] = {"remote-development", "resource-explorer", "editor", "terminal", "problems", "output"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "development", "Development",
      "Coding, build, test and debug workbench", LAYOUT_DEVELOPMENT, COUNT_OF(LAYOUT_DEVELOPMENT), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "review", "Review",
      "Source review, diff, quality and diagnostics", LAYOUT_REVIEW, COUNT_OF(LAYOUT_REVIEW), UMI_EXPERIENCE_LAYOUT_LOCKABLE },
    { sizeof(UmiExperienceLayoutDefinition), "ai-assisted", "AI Assisted",
      "Editor, knowledge and AI collaboration", LAYOUT_AI_ASSISTED, COUNT_OF(LAYOUT_AI_ASSISTED), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED },
    /* This layout is a reusable starting point, never a locked hard-coded UI. */
    { sizeof(UmiExperienceLayoutDefinition), "engine-development", "Engine Development",
      "Reusable engine, visual design, source and performance work", LAYOUT_ENGINE_DEVELOPMENT, COUNT_OF(LAYOUT_ENGINE_DEVELOPMENT), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR | UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED },
    /* Database work remains a named layout that users may clone and customise. */
    { sizeof(UmiExperienceLayoutDefinition), "data-development", "Data Development",
      "Database connections, queries, source, diagnostics and terminal", LAYOUT_DATA_DEVELOPMENT, COUNT_OF(LAYOUT_DATA_DEVELOPMENT), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED },
    /* Remote tools use the same docking model rather than a separate IDE shell. */
    { sizeof(UmiExperienceLayoutDefinition), "remote-development", "Remote Development",
      "Remote workspace, source, terminal, diagnostics and output", LAYOUT_REMOTE_DEVELOPMENT, COUNT_OF(LAYOUT_REMOTE_DEVELOPMENT), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR | UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED }
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
      UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER, "umicom.frontend" },
    /* Contracts and Studio surfaces exist; native detached windows still need
     * complete interaction and monitor recovery verification. */
    { sizeof(UmiExperienceFeatureDefinition), "studio.layout-customisation", "Complete layout customisation",
      "Dock, float, tab, group, link, lock, persist and recover every tool", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P0,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.workbench-layout" },
    /* Remote terminal and workspace contracts are mature foundations; full
     * welcome-screen connection and end-to-end GUI flows remain product work. */
    { sizeof(UmiExperienceFeatureDefinition), "studio.remote-development", "Remote development",
      "Remote hosts, workspaces, toolchains, tunnels and reconnection", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.terminal.remote" },
    /* Profiling models exist and now have a canonical panel, while live capture
     * adapters and complete native visualisation remain to be integrated. */
    { sizeof(UmiExperienceFeatureDefinition), "studio.performance-tools", "Performance tools",
      "CPU, memory, coverage, call-tree and regression analysis", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.observability.performance" },
    /* Database contracts and Studio centres exist; production connector drivers
     * must be supplied independently for each supported database product. */
    { sizeof(UmiExperienceFeatureDefinition), "studio.database-tools", "Database development",
      "Connections, schemas, query editing, data editing and migrations", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.data" },
    /* Extension trust, lifecycle and package contracts exist; full marketplace
     * download, restart and recovery flows remain integration work. */
    { sizeof(UmiExperienceFeatureDefinition), "studio.extension-lifecycle", "Extension lifecycle",
      "Trusted discovery, install, update, activation and quarantine", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.plugin" },
    /* Notebook document contracts exist but require a complete native cell and
     * output presentation surface before the feature is implemented. */
    { sizeof(UmiExperienceFeatureDefinition), "studio.notebooks", "Executable notebooks",
      "Language-aware cells, outputs, diagnostics and saved document state", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.language.notebook" },
    /* Presence and collaborative designer contracts exist; secure transport and
     * end-to-end permission UX remain unfinished. */
    { sizeof(UmiExperienceFeatureDefinition), "studio.collaboration", "Collaborative development",
      "Presence, shared context, review comments and permission boundaries", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.workbench.collaboration" },
    /* No localisation authoring implementation was found, so this is recorded
     * honestly as planned rather than inferred from general document support. */
    { sizeof(UmiExperienceFeatureDefinition), "studio.localisation", "Localisation tools",
      "Resource extraction, translation editing, previews and validation", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P3,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.localisation" },
    /* Framework now measures source guidance and Studio can review one selected
     * file; a complete native workspace dashboard remains future presentation work. */
    { sizeof(UmiExperienceFeatureDefinition),
      "studio.documentation-guidance",
      "Source guidance review",
      "Explain function and decision documentation coverage for learners",
      UMI_EXPERIENCE_FEATURE_FOUNDATION,
      UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.quality" },
    /* Engine Explorer is backed by the new shared catalogue and must remain a
     * thin projection rather than a second Studio-owned capability registry. */
    { sizeof(UmiExperienceFeatureDefinition), "studio.engine-explorer", "Engine Explorer",
      "Discover reusable Framework engines and verify project requirements", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.engine" }
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
