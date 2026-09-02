/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/completion_orchestration.h
 *
 * PURPOSE:
 *   Coordinate provider selection, policy, collection, deterministic query,
 *   resolution, cancellation and the active completion session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * List the named editor completion orchestration state values accepted by this public
 * contract.
 */
typedef enum UmiEditorCompletionOrchestrationState {
    UMI_EDITOR_COMPLETION_ORCHESTRATION_IDLE = 0,
    UMI_EDITOR_COMPLETION_ORCHESTRATION_COLLECTING = 1,
    UMI_EDITOR_COMPLETION_ORCHESTRATION_READY = 2,
    UMI_EDITOR_COMPLETION_ORCHESTRATION_CANCELLED = 3,
    UMI_EDITOR_COMPLETION_ORCHESTRATION_FAILED = 4
} UmiEditorCompletionOrchestrationState;

/**
 * Represent the editor completion policy data shared with callers of this public contract.
 */
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

/**
 * Represent the editor completion provider report data shared with callers of this public
 * contract.
 */
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

/**
 * Represent the editor completion orchestration snapshot data shared with callers of this
 * public contract.
 */
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

/**
 * Represent the editor completion orchestration data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorCompletionOrchestration
    UmiEditorCompletionOrchestration;

/**
 * Provide the editor completion policy default operation used by this module and its
 * client applications.
 */
UmiEditorCompletionPolicy umi_editor_completion_policy_default(void);
/**
 * Initialise editor completion orchestration from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_completion_orchestration_create(
    UmiEditorCompletionProviderRegistry *provider_registry,
    UmiEditorCompletionOrchestration **out_orchestration);
/**
 * Release or reset state held by editor completion orchestration so the same storage can
 * be reused safely.
 */
void umi_editor_completion_orchestration_destroy(
    UmiEditorCompletionOrchestration *orchestration);
/**
 * Provide the editor completion orchestration set policy operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_completion_orchestration_set_policy(
    UmiEditorCompletionOrchestration *orchestration,
    const UmiEditorCompletionPolicy *policy);
/**
 * Provide the editor completion orchestration begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_completion_orchestration_begin(
    UmiEditorCompletionOrchestration *orchestration,
    const UmiEditorCompletionRequest *request,
    const UmiEditorCompletionRankingConfig *ranking);
/**
 * Provide the editor completion orchestration cancel operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_completion_orchestration_cancel(
    UmiEditorCompletionOrchestration *orchestration);
/**
 * Find editor completion orchestration resolve while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_completion_orchestration_resolve_selected(
    UmiEditorCompletionOrchestration *orchestration);
/**
 * Find editor completion orchestration report while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_editor_completion_orchestration_report_at(
    const UmiEditorCompletionOrchestration *orchestration,
    size_t position,
    UmiEditorCompletionProviderReport *out_report);
/**
 * Provide the editor completion orchestration snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_completion_orchestration_snapshot(
    const UmiEditorCompletionOrchestration *orchestration,
    UmiEditorCompletionOrchestrationSnapshot *out_snapshot);
/**
 * Provide the editor completion orchestration session operation used by this module and
 * its client applications.
 */
UmiEditorCompletionSession *umi_editor_completion_orchestration_session(
    UmiEditorCompletionOrchestration *orchestration);
/**
 * Provide the editor completion orchestration result operation used by this module and its
 * client applications.
 */
const UmiEditorCompletionQueryResult *
umi_editor_completion_orchestration_result(
    const UmiEditorCompletionOrchestration *orchestration);
/**
 * Provide the editor completion orchestration revision operation used by this module and
 * its client applications.
 */
uint64_t umi_editor_completion_orchestration_revision(
    const UmiEditorCompletionOrchestration *orchestration);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_EDITOR_COMPLETION_ORCHESTRATION_H */
