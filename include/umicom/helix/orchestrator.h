/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/helix/orchestrator.h
 * PURPOSE: Coordinate governed Helix v2 plans, approvals, execution and undo.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* This orchestrator owns state and evidence, while host adapters
 * own side effects and AuthorEngine-compatible providers own model inference. */
#ifndef INCLUDE_UMICOM_HELIX_ORCHESTRATOR_H
#define INCLUDE_UMICOM_HELIX_ORCHESTRATOR_H

#include "umicom/helix/agent_runtime.h"
#include "umicom/helix/approval_gate.h"
#include "umicom/helix/candidate_set.h"
#include "umicom/helix/execution_adapter.h"
#include "umicom/helix/journal.h"
#include "umicom/helix/operation.h"
#include "umicom/helix/retry_loop.h"
#include "umicom/helix/compensation_plan.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the helix orchestrator config data shared with callers of this public
 * contract.
 */
typedef struct UmiHelixOrchestratorConfig {
    uint32_t maximum_attempts;
    double minimum_fitness;
    int require_human_approval;
} UmiHelixOrchestratorConfig;

/**
 * Represent the helix orchestrator data shared with callers of this public contract.
 */
typedef struct UmiHelixOrchestrator {
    UmiHelixOrchestratorConfig config;
    UmiHelixAgentRuntime agents;
    UmiHelixApprovalGate approvals;
    UmiHelixJournal journal;
    UmiHelixRetryLoop retry;
    UmiHelixCandidateSet candidates;
    UmiHelixCompensationPlan rollback;
    UmiHelixExecutionAdapter adapter;
    UmiHelixOperation operation;
    UmiHelixAction current_action;
    int adapter_configured;
    int operation_ready;
    int action_ready;
} UmiHelixOrchestrator;

/**
 * Provide the helix orchestrator config default operation used by this module and its
 * client applications.
 */
UmiHelixOrchestratorConfig umi_helix_orchestrator_config_default(void);
/**
 * Initialise helix orchestrator from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_helix_orchestrator_init(
    UmiHelixOrchestrator *orchestrator,
    const UmiHelixOrchestratorConfig *config);
/**
 * Provide the helix orchestrator set adapter operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_orchestrator_set_adapter(
    UmiHelixOrchestrator *orchestrator,
    const UmiHelixExecutionAdapter *adapter);
/**
 * Provide the helix orchestrator plan operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_orchestrator_plan(
    UmiHelixOrchestrator *orchestrator,
    const char *operation_id,
    const char *objective,
    const char *plan_hash);
/**
 * Provide the helix orchestrator prepare action operation used by this module and its
 * client applications.
 */
UmiStatus umi_helix_orchestrator_prepare_action(
    UmiHelixOrchestrator *orchestrator,
    const UmiHelixAction *action);
/**
 * Provide the helix orchestrator decide operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_orchestrator_decide(
    UmiHelixOrchestrator *orchestrator,
    UmiHelixApprovalDecision decision,
    const char *approver,
    const char *reason);
/**
 * Perform helix orchestrator through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_helix_orchestrator_execute(
    UmiHelixOrchestrator *orchestrator,
    char *out_evidence,
    size_t evidence_capacity);
/**
 * Provide the helix orchestrator begin rollback operation used by this module and its
 * client applications.
 */
UmiStatus umi_helix_orchestrator_begin_rollback(
    UmiHelixOrchestrator *orchestrator,
    int approved);
/**
 * Provide the helix orchestrator execute rollback operation used by this module and its
 * client applications.
 */
UmiStatus umi_helix_orchestrator_execute_rollback(
    UmiHelixOrchestrator *orchestrator,
    char *out_evidence,
    size_t evidence_capacity);

#ifdef __cplusplus
}
#endif

#endif
