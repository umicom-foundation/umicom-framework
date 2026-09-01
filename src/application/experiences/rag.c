/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experiences/rag.c
 *
 * PURPOSE:
 *   Define the canonical Framework-owned product experience for Umicom RAG.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experiences/rag.h"

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

/*
 * RAG separates knowledge collection from retrieval evaluation.
 * Source readers, indexes, search and evidence panels share one governed
 * retrieval model while layouts present different stages of that workflow. */

static const UmiExperiencePanelDefinition PANELS[] = {
    { sizeof(UmiExperiencePanelDefinition), "collections", "Collections",
      "Knowledge collections", "umicom.ai.retrieval", "ai.violet", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "sources", "Sources",
      "Files, repositories and connected sources", "umicom.knowledge", "ai.violet", "left",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "ingestion", "Ingestion",
      "Parse/chunk/embed/index jobs", "umicom.ai.retrieval", "operations.green", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "retrieval", "Retrieval Test",
      "Hybrid retrieval and reranking", "umicom.ai.retrieval", "ai.violet", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "source-viewer", "Source Viewer",
      "Source provenance and citations", "umicom.knowledge", "ai.violet", "right",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE | UMI_EXPERIENCE_PANEL_CONTEXT_LINKED },
    { sizeof(UmiExperiencePanelDefinition), "evaluation", "Evaluation",
      "Grounding and retrieval metrics", "umicom.ai.retrieval", "research.gold", "centre",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_FLOATABLE },
    { sizeof(UmiExperiencePanelDefinition), "jobs", "Jobs",
      "Large-corpus job health", "umicom.tasks", "operations.green", "bottom",
      UMI_EXPERIENCE_PANEL_DOCKABLE | UMI_EXPERIENCE_PANEL_AUTO_HIDE }
};

static const char *const LAYOUT_KNOWLEDGE[] = {"collections", "sources", "ingestion", "retrieval", "source-viewer"};
static const char *const LAYOUT_EVALUATION[] = {"retrieval", "evaluation", "source-viewer", "jobs"};

static const UmiExperienceLayoutDefinition LAYOUTS[] = {
    { sizeof(UmiExperienceLayoutDefinition), "knowledge", "Knowledge",
      "Collections, sources and retrieval", LAYOUT_KNOWLEDGE, COUNT_OF(LAYOUT_KNOWLEDGE), UMI_EXPERIENCE_LAYOUT_LOCKABLE | UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR },
    { sizeof(UmiExperienceLayoutDefinition), "evaluation", "Evaluation",
      "Grounding, citations and retrieval quality", LAYOUT_EVALUATION, COUNT_OF(LAYOUT_EVALUATION), UMI_EXPERIENCE_LAYOUT_LOCKABLE }
};

static const UmiExperienceFeatureDefinition FEATURES[] = {
    { sizeof(UmiExperienceFeatureDefinition), "rag.ingest", "Ingestion",
      "Source parse/chunk/metadata pipeline", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai.retrieval" },
    { sizeof(UmiExperienceFeatureDefinition), "rag.embed", "Embeddings",
      "Provider-neutral embeddings", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai.retrieval" },
    { sizeof(UmiExperienceFeatureDefinition), "rag.index", "Hybrid index",
      "Vector + lexical retrieval", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai.retrieval" },
    { sizeof(UmiExperienceFeatureDefinition), "rag.rerank", "Reranking",
      "Provider-neutral reranking", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.ai.retrieval" },
    { sizeof(UmiExperienceFeatureDefinition), "rag.citations", "Citations",
      "Source provenance and citation links", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.knowledge" },
    { sizeof(UmiExperienceFeatureDefinition), "rag.acl", "Permissions",
      "ACL filters before retrieval", UMI_EXPERIENCE_FEATURE_FOUNDATION, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.security" },
    { sizeof(UmiExperienceFeatureDefinition), "rag.scale", "Large-corpus scale",
      "Sharding/checkpoints/incremental indexing", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P2,
      UMI_EXPERIENCE_OWNER_FRAMEWORK, "umicom.tasks" },
    { sizeof(UmiExperienceFeatureDefinition), "rag.ui", "Knowledge Studio UI",
      "Collections/jobs/retrieval/evaluation workspaces", UMI_EXPERIENCE_FEATURE_PLANNED, UMI_EXPERIENCE_PRIORITY_P1,
      UMI_EXPERIENCE_OWNER_APPLICATION, "umicom.ui" }
};

static const UmiApplicationExperienceDefinition DEFINITION = {
    sizeof(UmiApplicationExperienceDefinition),
    UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "org.umicom.rag",
    "Umicom RAG",
    "knowledge",
    PANELS,
    COUNT_OF(PANELS),
    LAYOUTS,
    COUNT_OF(LAYOUTS),
    FEATURES,
    COUNT_OF(FEATURES)
};

const UmiApplicationExperienceDefinition *umi_application_experience_rag(void)
{
    return &DEFINITION;
}
