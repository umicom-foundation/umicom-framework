/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/llm.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom LLM.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/llm.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

/*
 * LLM separates conversation, model management and evaluation workflows.
 * Provider processes and secrets remain behind Framework contracts while the
 * layouts arrange chat, runtime, tools and evidence panels. */

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "chat", "Chat",
      "Conversation and composer", "umicom.ai", "ai.violet", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "projects", "Projects",
      "Project instructions, chats and artifacts", "umicom.ai", "ai.violet", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "knowledge", "Knowledge",
      "RAG collections and retrieval", "umicom.ai.retrieval", "ai.violet", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "context-inspector", "Context Inspector",
      "Model, sources, context and privacy", "umicom.ai", "ai.violet", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "tools", "MCP & Tools",
      "Tool servers, permissions and approvals", "umicom.ai.tools", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "model-library", "Model Host",
      "Installed/local models and runtimes", "umicom.llm", "ai.violet", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "host-health", "Host Health",
      "CPU/RAM/GPU/runtime health", "umicom.metrics", "operations.green", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "provider-router", "Provider Router",
      "Local/cloud routing and policies", "umicom.ai", "ai.violet", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CRITICAL },
    { sizeof(UmiExperiencePanelDefinition), "research", "Deep Research",
      "Plans, sources, citations and report", "umicom.ai.research", "research.gold", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "compare", "Model Compare",
      "Side-by-side evaluation", "umicom.ai", "research.gold", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "artifacts", "Artifacts",
      "Code and document work products", "umicom.ai", "development.blue", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_MULTI_MONITOR },
    { sizeof(UmiExperiencePanelDefinition), "sources", "Source Ledger",
      "Evidence retained/excluded/conflicts", "umicom.ai.retrieval", "research.gold", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "usage", "Usage & Resources",
      "Latency, tokens, cost and hardware", "umicom.metrics", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE }
};

static const char *const LAYOUT_CHAT[] = {"chat", "projects", "knowledge", "context-inspector", "tools"};
static const char *const LAYOUT_MODELS[] = {"model-library", "host-health", "provider-router", "usage"};
static const char *const LAYOUT_EVALUATION[] = {"research", "compare", "artifacts", "sources", "usage"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "chat", "Chat",
      "Conversation, project knowledge and context", LAYOUT_CHAT, COUNT_OF(LAYOUT_CHAT), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_RESPONSIVE },
    { sizeof(UmiExperienceLayoutDefinition), "models", "Models",
      "Local model host and provider operations", LAYOUT_MODELS, COUNT_OF(LAYOUT_MODELS), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "evaluation", "Evaluation",
      "Research, compare and evaluation", LAYOUT_EVALUATION, COUNT_OF(LAYOUT_EVALUATION), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "llm.providers", "Provider registry",
      "Local/remote provider contracts", UMI_EXPERIENCE_FEATURE_IMPLEMENTED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai" },
    { sizeof(UmiExperienceFeatureDefinition), "llm.local-host", "Local model host",
      "llama.cpp/Ollama/LM Studio/local runtime", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.llm" },
    { sizeof(UmiExperienceFeatureDefinition), "llm.cloud", "Cloud providers",
      "OpenAI/Anthropic/compatible endpoints", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai" },
    { sizeof(UmiExperienceFeatureDefinition), "llm.rag", "RAG",
      "Collections, retrieval and citations", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai.retrieval" },
    { sizeof(UmiExperienceFeatureDefinition), "llm.tools", "MCP/tools",
      "Tool registry, permissions and sandbox", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai.tools" },
    { sizeof(UmiExperienceFeatureDefinition), "llm.research", "Deep research",
      "Plan/source/citation/report workflow", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ai.research" },
    { sizeof(UmiExperienceFeatureDefinition), "llm.artifacts", "Artifacts",
      "Code/document generation and review", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ai" },
    { sizeof(UmiExperienceFeatureDefinition), "llm.evaluation", "Model evaluation",
      "Datasets, metrics and compare", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ai" },
    { sizeof(UmiExperienceFeatureDefinition), "llm.web-mobile", "Web/mobile clients",
      "Multi-user server and mobile companion", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P3,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.frontend" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.llm",
    "Umicom LLM",
    "chat",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_llm(void)
{
    return &DEFINITION;
}
