/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/orchestrator_v2.h
 * PURPOSE: Coordinate governed Helix v2 plans, approvals, execution and undo.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
/* BEGINNER NOTE: This orchestrator owns state and evidence, while host adapters
 * own side effects and AuthorEngine-compatible providers own model inference. */
#ifndef INCLUDE_UMICOM_HELIX_ORCHESTRATOR_V2_H
#define INCLUDE_UMICOM_HELIX_ORCHESTRATOR_V2_H

#include "umicom/helix/agent_runtime_v2.h"
#include "umicom/helix/approval_gate_v2.h"
#include "umicom/helix/candidate_set_v2.h"
#include "umicom/helix/execution_adapter_v2.h"
#include "umicom/helix/journal_v2.h"
#include "umicom/helix/operation_v2.h"
#include "umicom/helix/retry_loop_v2.h"
#include "umicom/helix/rollback_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiHelixOrchestratorConfigV2 {
    uint32_t maximum_attempts;
    double minimum_fitness;
    int require_human_approval;
} UmiHelixOrchestratorConfigV2;

typedef struct UmiHelixOrchestratorV2 {
    UmiHelixOrchestratorConfigV2 config;
    UmiHelixAgentRuntimeV2 agents;
    UmiHelixApprovalGateV2 approvals;
    UmiHelixJournalV2 journal;
    UmiHelixRetryLoopV2 retry;
    UmiHelixCandidateSetV2 candidates;
    UmiHelixRollbackPlanV2 rollback;
    UmiHelixExecutionAdapterV2 adapter;
    UmiHelixOperationV2 operation;
    UmiHelixActionV2 current_action;
    int adapter_configured;
    int operation_ready;
    int action_ready;
} UmiHelixOrchestratorV2;

UmiHelixOrchestratorConfigV2 umi_helix_orchestrator_v2_config_default(void);
UmiStatus umi_helix_orchestrator_v2_init(
    UmiHelixOrchestratorV2 *orchestrator,
    const UmiHelixOrchestratorConfigV2 *config);
UmiStatus umi_helix_orchestrator_v2_set_adapter(
    UmiHelixOrchestratorV2 *orchestrator,
    const UmiHelixExecutionAdapterV2 *adapter);
UmiStatus umi_helix_orchestrator_v2_plan(
    UmiHelixOrchestratorV2 *orchestrator,
    const char *operation_id,
    const char *objective,
    const char *plan_hash);
UmiStatus umi_helix_orchestrator_v2_prepare_action(
    UmiHelixOrchestratorV2 *orchestrator,
    const UmiHelixActionV2 *action);
UmiStatus umi_helix_orchestrator_v2_decide(
    UmiHelixOrchestratorV2 *orchestrator,
    UmiHelixApprovalDecisionV2 decision,
    const char *approver,
    const char *reason);
UmiStatus umi_helix_orchestrator_v2_execute(
    UmiHelixOrchestratorV2 *orchestrator,
    char *out_evidence,
    size_t evidence_capacity);
UmiStatus umi_helix_orchestrator_v2_begin_rollback(
    UmiHelixOrchestratorV2 *orchestrator,
    int approved);
UmiStatus umi_helix_orchestrator_v2_execute_rollback(
    UmiHelixOrchestratorV2 *orchestrator,
    char *out_evidence,
    size_t evidence_capacity);

#ifdef __cplusplus
}
#endif

#endif
