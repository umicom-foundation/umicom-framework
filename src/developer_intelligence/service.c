/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_intelligence/service.c
 *
 * PURPOSE:
 *   Provide one discoverable operational catalogue across the canonical
 *   Language, AI, Knowledge and Teacher development capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_intelligence/service.h"

#include <string.h>

#include "umicom/developer_intelligence/language/assembly_profile.h"
#include "umicom/developer_intelligence/language/c23_profile.h"
#include "umicom/developer_intelligence/language/call_hierarchy.h"
#include "umicom/developer_intelligence/language/code_action_rank.h"
#include "umicom/developer_intelligence/language/code_lens.h"
#include "umicom/developer_intelligence/language/compilation_database.h"
#include "umicom/developer_intelligence/language/completion_context.h"
#include "umicom/developer_intelligence/language/completion_prefix.h"
#include "umicom/developer_intelligence/language/completion_rank.h"
#include "umicom/developer_intelligence/language/cpp_profile.h"
#include "umicom/developer_intelligence/language/declaration_route.h"
#include "umicom/developer_intelligence/language/diagnostic_dedupe.h"
#include "umicom/developer_intelligence/language/diagnostic_merge.h"
#include "umicom/developer_intelligence/language/document_link.h"
#include "umicom/developer_intelligence/language/formatting_range.h"
#include "umicom/developer_intelligence/language/hover_merge.h"
#include "umicom/developer_intelligence/language/implementation_route.h"
#include "umicom/developer_intelligence/language/include_graph.h"
#include "umicom/developer_intelligence/language/inlay_hint_rank.h"
#include "umicom/developer_intelligence/language/inline_value.h"
#include "umicom/developer_intelligence/language/linked_editing.h"
#include "umicom/developer_intelligence/language/project_template_score.h"
#include "umicom/developer_intelligence/language/provider_failover.h"
#include "umicom/developer_intelligence/language/provider_selection.h"
#include "umicom/developer_intelligence/language/refactor_preview.h"
#include "umicom/developer_intelligence/language/reference_rank.h"
#include "umicom/developer_intelligence/language/rename_conflict.h"
#include "umicom/developer_intelligence/language/rename_plan.h"
#include "umicom/developer_intelligence/language/request_budget.h"
#include "umicom/developer_intelligence/language/request_priority.h"
#include "umicom/developer_intelligence/language/response_freshness.h"
#include "umicom/developer_intelligence/language/selection_range.h"
#include "umicom/developer_intelligence/language/semantic_token_delta.h"
#include "umicom/developer_intelligence/language/semantic_token_range.h"
#include "umicom/developer_intelligence/language/signature_active_parameter.h"
#include "umicom/developer_intelligence/language/symbol_rank.h"
#include "umicom/developer_intelligence/language/symbol_search.h"
#include "umicom/developer_intelligence/language/type_definition_route.h"
#include "umicom/developer_intelligence/language/type_hierarchy.h"
#include "umicom/developer_intelligence/language/workspace_edit_validation.h"
#include "umicom/developer_intelligence/ai/agent_plan.h"
#include "umicom/developer_intelligence/ai/agent_step.h"
#include "umicom/developer_intelligence/ai/approval_gate.h"
#include "umicom/developer_intelligence/ai/change_review.h"
#include "umicom/developer_intelligence/ai/checkpoint_policy.h"
#include "umicom/developer_intelligence/ai/citation_coverage.h"
#include "umicom/developer_intelligence/ai/citation_rank.h"
#include "umicom/developer_intelligence/ai/code_completion.h"
#include "umicom/developer_intelligence/ai/code_explanation.h"
#include "umicom/developer_intelligence/ai/code_refactor.h"
#include "umicom/developer_intelligence/ai/code_review.h"
#include "umicom/developer_intelligence/ai/code_test_generation.h"
#include "umicom/developer_intelligence/ai/context_budget.h"
#include "umicom/developer_intelligence/ai/context_source_rank.h"
#include "umicom/developer_intelligence/ai/conversation_memory.h"
#include "umicom/developer_intelligence/ai/edit_risk.h"
#include "umicom/developer_intelligence/ai/local_runtime_route.h"
#include "umicom/developer_intelligence/ai/model_health.h"
#include "umicom/developer_intelligence/ai/model_route.h"
#include "umicom/developer_intelligence/ai/privacy_policy.h"
#include "umicom/developer_intelligence/ai/prompt_budget.h"
#include "umicom/developer_intelligence/ai/provider_fallback.h"
#include "umicom/developer_intelligence/ai/rag_collection_rank.h"
#include "umicom/developer_intelligence/ai/rag_source_rank.h"
#include "umicom/developer_intelligence/ai/repository_change.h"
#include "umicom/developer_intelligence/ai/repository_index.h"
#include "umicom/developer_intelligence/ai/rerank_score.h"
#include "umicom/developer_intelligence/ai/retrieval_fusion.h"
#include "umicom/developer_intelligence/ai/retrieval_lexical.h"
#include "umicom/developer_intelligence/ai/retrieval_vector.h"
#include "umicom/developer_intelligence/ai/teacher_assessment.h"
#include "umicom/developer_intelligence/ai/teacher_gap.h"
#include "umicom/developer_intelligence/ai/teacher_hint.h"
#include "umicom/developer_intelligence/ai/teacher_mastery.h"
#include "umicom/developer_intelligence/ai/teacher_progress.h"
#include "umicom/developer_intelligence/ai/teacher_recommendation.h"
#include "umicom/developer_intelligence/ai/teacher_study_plan.h"
#include "umicom/developer_intelligence/ai/tool_policy.h"
#include "umicom/developer_intelligence/ai/tool_risk.h"
#include "umicom/developer_intelligence/ai/workspace_memory.h"

typedef struct UmiDeveloperIntelligenceCatalogueEntry {
    const char *id;
    UmiDeveloperIntelligenceEvaluator evaluator;
} UmiDeveloperIntelligenceCatalogueEntry;

static const UmiDeveloperIntelligenceCatalogueEntry CATALOGUE[] = {
    {"language.assembly-profile", UmiDeveloperIntelligenceEvaluateLanguageAssemblyProfile},
    {"language.c23-profile", UmiDeveloperIntelligenceEvaluateLanguageC23Profile},
    {"language.call-hierarchy", UmiDeveloperIntelligenceEvaluateLanguageCallHierarchy},
    {"language.code-action-rank", UmiDeveloperIntelligenceEvaluateLanguageCodeActionRank},
    {"language.code-lens", UmiDeveloperIntelligenceEvaluateLanguageCodeLens},
    {"language.compilation-database", UmiDeveloperIntelligenceEvaluateLanguageCompilationDatabase},
    {"language.completion-context", UmiDeveloperIntelligenceEvaluateLanguageCompletionContext},
    {"language.completion-prefix", UmiDeveloperIntelligenceEvaluateLanguageCompletionPrefix},
    {"language.completion-rank", UmiDeveloperIntelligenceEvaluateLanguageCompletionRank},
    {"language.cpp-profile", UmiDeveloperIntelligenceEvaluateLanguageCppProfile},
    {"language.declaration-route", UmiDeveloperIntelligenceEvaluateLanguageDeclarationRoute},
    {"language.diagnostic-dedupe", UmiDeveloperIntelligenceEvaluateLanguageDiagnosticDedupe},
    {"language.diagnostic-merge", UmiDeveloperIntelligenceEvaluateLanguageDiagnosticMerge},
    {"language.document-link", UmiDeveloperIntelligenceEvaluateLanguageDocumentLink},
    {"language.formatting-range", UmiDeveloperIntelligenceEvaluateLanguageFormattingRange},
    {"language.hover-merge", UmiDeveloperIntelligenceEvaluateLanguageHoverMerge},
    {"language.implementation-route", UmiDeveloperIntelligenceEvaluateLanguageImplementationRoute},
    {"language.include-graph", UmiDeveloperIntelligenceEvaluateLanguageIncludeGraph},
    {"language.inlay-hint-rank", UmiDeveloperIntelligenceEvaluateLanguageInlayHintRank},
    {"language.inline-value", UmiDeveloperIntelligenceEvaluateLanguageInlineValue},
    {"language.linked-editing", UmiDeveloperIntelligenceEvaluateLanguageLinkedEditing},
    {"language.project-template-score", UmiDeveloperIntelligenceEvaluateLanguageProjectTemplateScore},
    {"language.provider-failover", UmiDeveloperIntelligenceEvaluateLanguageProviderFailover},
    {"language.provider-selection", UmiDeveloperIntelligenceEvaluateLanguageProviderSelection},
    {"language.refactor-preview", UmiDeveloperIntelligenceEvaluateLanguageRefactorPreview},
    {"language.reference-rank", UmiDeveloperIntelligenceEvaluateLanguageReferenceRank},
    {"language.rename-conflict", UmiDeveloperIntelligenceEvaluateLanguageRenameConflict},
    {"language.rename-plan", UmiDeveloperIntelligenceEvaluateLanguageRenamePlan},
    {"language.request-budget", UmiDeveloperIntelligenceEvaluateLanguageRequestBudget},
    {"language.request-priority", UmiDeveloperIntelligenceEvaluateLanguageRequestPriority},
    {"language.response-freshness", UmiDeveloperIntelligenceEvaluateLanguageResponseFreshness},
    {"language.selection-range", UmiDeveloperIntelligenceEvaluateLanguageSelectionRange},
    {"language.semantic-token-delta", UmiDeveloperIntelligenceEvaluateLanguageSemanticTokenDelta},
    {"language.semantic-token-range", UmiDeveloperIntelligenceEvaluateLanguageSemanticTokenRange},
    {"language.signature-active-parameter", UmiDeveloperIntelligenceEvaluateLanguageSignatureActiveParameter},
    {"language.symbol-rank", UmiDeveloperIntelligenceEvaluateLanguageSymbolRank},
    {"language.symbol-search", UmiDeveloperIntelligenceEvaluateLanguageSymbolSearch},
    {"language.type-definition-route", UmiDeveloperIntelligenceEvaluateLanguageTypeDefinitionRoute},
    {"language.type-hierarchy", UmiDeveloperIntelligenceEvaluateLanguageTypeHierarchy},
    {"language.workspace-edit-validation", UmiDeveloperIntelligenceEvaluateLanguageWorkspaceEditValidation},
    {"ai.agent-plan", UmiDeveloperIntelligenceEvaluateAiAgentPlan},
    {"ai.agent-step", UmiDeveloperIntelligenceEvaluateAiAgentStep},
    {"ai.approval-gate", UmiDeveloperIntelligenceEvaluateAiApprovalGate},
    {"ai.change-review", UmiDeveloperIntelligenceEvaluateAiChangeReview},
    {"ai.checkpoint-policy", UmiDeveloperIntelligenceEvaluateAiCheckpointPolicy},
    {"ai.citation-coverage", UmiDeveloperIntelligenceEvaluateAiCitationCoverage},
    {"ai.citation-rank", UmiDeveloperIntelligenceEvaluateAiCitationRank},
    {"ai.code-completion", UmiDeveloperIntelligenceEvaluateAiCodeCompletion},
    {"ai.code-explanation", UmiDeveloperIntelligenceEvaluateAiCodeExplanation},
    {"ai.code-refactor", UmiDeveloperIntelligenceEvaluateAiCodeRefactor},
    {"ai.code-review", UmiDeveloperIntelligenceEvaluateAiCodeReview},
    {"ai.code-test-generation", UmiDeveloperIntelligenceEvaluateAiCodeTestGeneration},
    {"ai.context-budget", UmiDeveloperIntelligenceEvaluateAiContextBudget},
    {"ai.context-source-rank", UmiDeveloperIntelligenceEvaluateAiContextSourceRank},
    {"ai.conversation-memory", UmiDeveloperIntelligenceEvaluateAiConversationMemory},
    {"ai.edit-risk", UmiDeveloperIntelligenceEvaluateAiEditRisk},
    {"ai.local-runtime-route", UmiDeveloperIntelligenceEvaluateAiLocalRuntimeRoute},
    {"ai.model-health", UmiDeveloperIntelligenceEvaluateAiModelHealth},
    {"ai.model-route", UmiDeveloperIntelligenceEvaluateAiModelRoute},
    {"ai.privacy-policy", UmiDeveloperIntelligenceEvaluateAiPrivacyPolicy},
    {"ai.prompt-budget", UmiDeveloperIntelligenceEvaluateAiPromptBudget},
    {"ai.provider-fallback", UmiDeveloperIntelligenceEvaluateAiProviderFallback},
    {"ai.rag-collection-rank", UmiDeveloperIntelligenceEvaluateAiRagCollectionRank},
    {"ai.rag-source-rank", UmiDeveloperIntelligenceEvaluateAiRagSourceRank},
    {"ai.repository-change", UmiDeveloperIntelligenceEvaluateAiRepositoryChange},
    {"ai.repository-index", UmiDeveloperIntelligenceEvaluateAiRepositoryIndex},
    {"ai.rerank-score", UmiDeveloperIntelligenceEvaluateAiRerankScore},
    {"ai.retrieval-fusion", UmiDeveloperIntelligenceEvaluateAiRetrievalFusion},
    {"ai.retrieval-lexical", UmiDeveloperIntelligenceEvaluateAiRetrievalLexical},
    {"ai.retrieval-vector", UmiDeveloperIntelligenceEvaluateAiRetrievalVector},
    {"ai.teacher-assessment", UmiDeveloperIntelligenceEvaluateAiTeacherAssessment},
    {"ai.teacher-gap", UmiDeveloperIntelligenceEvaluateAiTeacherGap},
    {"ai.teacher-hint", UmiDeveloperIntelligenceEvaluateAiTeacherHint},
    {"ai.teacher-mastery", UmiDeveloperIntelligenceEvaluateAiTeacherMastery},
    {"ai.teacher-progress", UmiDeveloperIntelligenceEvaluateAiTeacherProgress},
    {"ai.teacher-recommendation", UmiDeveloperIntelligenceEvaluateAiTeacherRecommendation},
    {"ai.teacher-study-plan", UmiDeveloperIntelligenceEvaluateAiTeacherStudyPlan},
    {"ai.tool-policy", UmiDeveloperIntelligenceEvaluateAiToolPolicy},
    {"ai.tool-risk", UmiDeveloperIntelligenceEvaluateAiToolRisk},
    {"ai.workspace-memory", UmiDeveloperIntelligenceEvaluateAiWorkspaceMemory},
};

size_t UmiDeveloperIntelligenceServiceCount(void)
{
    return sizeof(CATALOGUE) / sizeof(CATALOGUE[0]);
}

UmiStatus UmiDeveloperIntelligenceServiceIdAt(
    size_t index,
    const char **outId)
{
    if (outId == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outId = NULL;
    if (index >= UmiDeveloperIntelligenceServiceCount()) {
        return UMI_STATUS_NOT_FOUND;
    }
    *outId = CATALOGUE[index].id;
    return UMI_STATUS_OK;
}

UmiStatus UmiDeveloperIntelligenceServiceEvaluate(
    const char *id,
    const UmiDeveloperIntelligenceInput *input,
    UmiDeveloperIntelligenceSnapshot *outSnapshot)
{
    size_t index;

    if (id == NULL || id[0] == '\0' || input == NULL ||
        outSnapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < UmiDeveloperIntelligenceServiceCount(); ++index) {
        if (strcmp(CATALOGUE[index].id, id) == 0) {
            return CATALOGUE[index].evaluator(input, outSnapshot);
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
