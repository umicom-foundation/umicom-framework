/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/completion_orchestration.h
 *
 * PURPOSE:
 *   Coordinate provider selection, policy, collection, deterministic query,
 *   resolution, cancellation and the active completion session.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_COMPLETION_ORCHESTRATION_H
#define UMICOM_EDITOR_COMPLETION_ORCHESTRATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/editor/completion_session.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_EDITOR_COMPLETION_ORCHESTRATION_API_VERSION 1U

typedef enum UmiEditorCompletionOrchestrationState {
    UMI_EDITOR_COMPLETION_ORCHESTRATION_IDLE = 0,
    UMI_EDITOR_COMPLETION_ORCHESTRATION_COLLECTING = 1,
    UMI_EDITOR_COMPLETION_ORCHESTRATION_READY = 2,
    UMI_EDITOR_COMPLETION_ORCHESTRATION_CANCELLED = 3,
    UMI_EDITOR_COMPLETION_ORCHESTRATION_FAILED = 4
} UmiEditorCompletionOrchestrationState;

typedef struct UmiEditorCompletionPolicy {
    uint32_t struct_size;
    uint32_t api_version;
    size_t maximum_providers;
    size_t maximum_candidates;
    size_t maximum_results;
    int allow_ai;
    int allow_remote;
    int trusted_workspace;
    int continue_on_provider_error;
    int resolve_before_accept;
} UmiEditorCompletionPolicy;

typedef struct UmiEditorCompletionProviderReport {
    uint32_t struct_size;
    uint32_t api_version;
    char provider_id[UMI_EDITOR_COMPLETION_PROVIDER_ID_CAPACITY];
    UmiStatus status;
    size_t emitted_candidate_count;
    uint64_t provider_revision;
    int incomplete;
    int skipped_by_policy;
    int truncated;
} UmiEditorCompletionProviderReport;

typedef struct UmiEditorCompletionOrchestrationSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    UmiEditorCompletionOrchestrationState state;
    size_t eligible_provider_count;
    size_t invoked_provider_count;
    size_t failed_provider_count;
    size_t collected_candidate_count;
    size_t result_count;
    uint64_t request_id;
    uint64_t document_revision;
    uint64_t provider_registry_revision;
    uint64_t revision;
    int truncated;
    int incomplete;
} UmiEditorCompletionOrchestrationSnapshot;

typedef struct UmiEditorCompletionOrchestration
    UmiEditorCompletionOrchestration;

UmiEditorCompletionPolicy umi_editor_completion_policy_default(void);
UmiStatus umi_editor_completion_orchestration_create(
    UmiEditorCompletionProviderRegistry *provider_registry,
    UmiEditorCompletionOrchestration **out_orchestration);
void umi_editor_completion_orchestration_destroy(
    UmiEditorCompletionOrchestration *orchestration);
UmiStatus umi_editor_completion_orchestration_set_policy(
    UmiEditorCompletionOrchestration *orchestration,
    const UmiEditorCompletionPolicy *policy);
UmiStatus umi_editor_completion_orchestration_begin(
    UmiEditorCompletionOrchestration *orchestration,
    const UmiEditorCompletionRequest *request,
    const UmiEditorCompletionRankingConfig *ranking);
UmiStatus umi_editor_completion_orchestration_cancel(
    UmiEditorCompletionOrchestration *orchestration);
UmiStatus umi_editor_completion_orchestration_resolve_selected(
    UmiEditorCompletionOrchestration *orchestration);
UmiStatus umi_editor_completion_orchestration_report_at(
    const UmiEditorCompletionOrchestration *orchestration,
    size_t position,
    UmiEditorCompletionProviderReport *out_report);
UmiStatus umi_editor_completion_orchestration_snapshot(
    const UmiEditorCompletionOrchestration *orchestration,
    UmiEditorCompletionOrchestrationSnapshot *out_snapshot);
UmiEditorCompletionSession *umi_editor_completion_orchestration_session(
    UmiEditorCompletionOrchestration *orchestration);
const UmiEditorCompletionQueryResult *
umi_editor_completion_orchestration_result(
    const UmiEditorCompletionOrchestration *orchestration);
uint64_t umi_editor_completion_orchestration_revision(
    const UmiEditorCompletionOrchestration *orchestration);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_COMPLETION_ORCHESTRATION_H */
