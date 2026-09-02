/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/orchestrator.c
 * PURPOSE: Implement controlled autonomous development orchestration.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/orchestrator.h"

#include <stdio.h>
#include <string.h>

/*
 * Provide the helix orchestrator config default operation used by this module and its
 * client applications.
 */
UmiHelixOrchestratorConfig umi_helix_orchestrator_config_default(void)
{
    UmiHelixOrchestratorConfig config;
    config.maximum_attempts = 3U;
    config.minimum_fitness = 0.80;
    config.require_human_approval = 1;
    return config;
}

/*
 * Initialise helix orchestrator from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_helix_orchestrator_init(
    UmiHelixOrchestrator *orchestrator,
    const UmiHelixOrchestratorConfig *config)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestrator == NULL || config == NULL || config->maximum_attempts == 0U ||
        config->minimum_fitness < 0.0 || config->minimum_fitness > 1.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(orchestrator, 0, sizeof(*orchestrator));
    orchestrator->config = *config;
    umi_helix_approval_gate_init(&orchestrator->approvals);
    umi_helix_journal_init(&orchestrator->journal);
    umi_helix_candidate_set_init(&orchestrator->candidates);
    umi_helix_compensation_plan_init(&orchestrator->rollback);
    status = umi_helix_retry_loop_init(&orchestrator->retry,
                                          config->maximum_attempts);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_helix_agent_runtime_register_defaults(
            &orchestrator->agents);
    }
    return status;
}

/*
 * Provide the helix orchestrator set adapter operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_orchestrator_set_adapter(
    UmiHelixOrchestrator *orchestrator,
    const UmiHelixExecutionAdapter *adapter)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestrator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_helix_execution_adapter_validate(adapter);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        orchestrator->adapter = *adapter;
        orchestrator->adapter_configured = 1;
    }
    return status;
}

/*
 * Provide the helix orchestrator plan operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_orchestrator_plan(
    UmiHelixOrchestrator *orchestrator,
    const char *operation_id,
    const char *objective,
    const char *plan_hash)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestrator == NULL || orchestrator->operation_ready) {
        return orchestrator == NULL ? UMI_STATUS_INVALID_ARGUMENT
                                    : UMI_STATUS_INVALID_STATE;
    }
    status = umi_helix_operation_init(&orchestrator->operation,
                                         operation_id, objective, plan_hash);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_helix_operation_transition(
        &orchestrator->operation, UMI_HELIX_OPERATION_PLANNED);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_helix_journal_append(
        &orchestrator->journal, UMI_HELIX_JOURNAL_PLAN, operation_id, objective);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) orchestrator->operation_ready = 1;
    return status;
}

/*
 * Provide the helix orchestrator prepare action operation used by this module and its
 * client applications.
 */
UmiStatus umi_helix_orchestrator_prepare_action(
    UmiHelixOrchestrator *orchestrator,
    const UmiHelixAction *action)
{
    UmiStatus status;
    int requires_approval;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestrator == NULL || action == NULL || !orchestrator->operation_ready ||
        orchestrator->action_ready ||
        orchestrator->operation.state != UMI_HELIX_OPERATION_PLANNED) {
        return UMI_STATUS_INVALID_STATE;
    }
    orchestrator->current_action = *action;
    orchestrator->current_action.maximum_attempts =
        orchestrator->config.maximum_attempts;
    requires_approval = orchestrator->config.require_human_approval ||
        umi_helix_action_requires_approval(action);
    /* Apply this branch only when its contract condition is satisfied. */
    if (requires_approval) {
        status = umi_helix_approval_gate_request(
            &orchestrator->approvals, orchestrator->operation.operation_id,
            action->action_id, orchestrator->operation.plan_hash);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) status = umi_helix_operation_transition(
            &orchestrator->operation, UMI_HELIX_OPERATION_AWAITING_APPROVAL);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        status = umi_helix_operation_transition(
            &orchestrator->operation, UMI_HELIX_OPERATION_APPROVED);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) orchestrator->action_ready = 1;
    return status;
}

/*
 * Provide the helix orchestrator decide operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_orchestrator_decide(
    UmiHelixOrchestrator *orchestrator,
    UmiHelixApprovalDecision decision,
    const char *approver,
    const char *reason)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestrator == NULL || !orchestrator->action_ready ||
        orchestrator->operation.state != UMI_HELIX_OPERATION_AWAITING_APPROVAL) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_helix_approval_gate_decide(
        &orchestrator->approvals, orchestrator->operation.operation_id,
        orchestrator->current_action.action_id, decision, approver, reason);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_helix_journal_append(
        &orchestrator->journal, UMI_HELIX_JOURNAL_APPROVAL,
        orchestrator->operation.operation_id,
        decision == UMI_HELIX_APPROVAL_APPROVED ? "Action approved"
                                                : "Action declined");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_helix_operation_transition(
        &orchestrator->operation,
        decision == UMI_HELIX_APPROVAL_APPROVED
            ? UMI_HELIX_OPERATION_APPROVED : UMI_HELIX_OPERATION_CANCELLED);
    return status;
}

/*
 * Perform helix orchestrator through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_helix_orchestrator_execute(
    UmiHelixOrchestrator *orchestrator,
    char *out_evidence,
    size_t evidence_capacity)
{
    UmiStatus result;
    int approved;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestrator == NULL || out_evidence == NULL || evidence_capacity == 0U ||
        !orchestrator->adapter_configured || !orchestrator->action_ready ||
        orchestrator->operation.state != UMI_HELIX_OPERATION_APPROVED) {
        return UMI_STATUS_INVALID_STATE;
    }
    approved = !orchestrator->config.require_human_approval &&
               !umi_helix_action_requires_approval(
                   &orchestrator->current_action);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!approved) approved = umi_helix_approval_gate_is_approved(
        &orchestrator->approvals, orchestrator->operation.operation_id,
        orchestrator->current_action.action_id,
        orchestrator->operation.plan_hash);
    result = umi_helix_retry_loop_begin(&orchestrator->retry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;
    orchestrator->current_action.attempt = orchestrator->retry.attempts_started;
    result = umi_helix_operation_transition(
        &orchestrator->operation, UMI_HELIX_OPERATION_RUNNING);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;
    result = umi_helix_execution_adapter_execute(
        &orchestrator->adapter, &orchestrator->current_action, approved,
        out_evidence, evidence_capacity);
    (void)umi_helix_retry_loop_record(&orchestrator->retry, result);
    (void)umi_helix_journal_append(
        &orchestrator->journal, UMI_HELIX_JOURNAL_ACTION,
        orchestrator->operation.operation_id,
        orchestrator->current_action.summary);
    /* Apply this branch only when its contract condition is satisfied. */
    if (out_evidence[0] != '\0') {
        (void)umi_helix_journal_append(
            &orchestrator->journal, UMI_HELIX_JOURNAL_EVIDENCE,
            orchestrator->operation.operation_id, out_evidence);
        orchestrator->operation.evidence_count++;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result == UMI_STATUS_OK) {
        orchestrator->operation.actions_completed++;
        return umi_helix_operation_transition(
            &orchestrator->operation, UMI_HELIX_OPERATION_SUCCEEDED);
    }
    orchestrator->operation.retry_count++;
    (void)umi_helix_operation_transition(
        &orchestrator->operation, UMI_HELIX_OPERATION_FAILED);
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_helix_retry_loop_can_retry(&orchestrator->retry)) {
        (void)umi_helix_operation_transition(
            &orchestrator->operation, UMI_HELIX_OPERATION_APPROVED);
    }
    return result;
}

/*
 * Provide the helix orchestrator begin rollback operation used by this module and its
 * client applications.
 */
UmiStatus umi_helix_orchestrator_begin_rollback(
    UmiHelixOrchestrator *orchestrator,
    int approved)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestrator == NULL || !orchestrator->operation_ready) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_helix_compensation_plan_begin(&orchestrator->rollback, approved);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_helix_operation_transition(
        &orchestrator->operation, UMI_HELIX_OPERATION_ROLLING_BACK);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_helix_journal_append(
        &orchestrator->journal, UMI_HELIX_JOURNAL_ROLLBACK,
        orchestrator->operation.operation_id, "Rollback started");
    return status;
}

/*
 * Provide the helix orchestrator execute rollback operation used by this module and its
 * client applications.
 */
UmiStatus umi_helix_orchestrator_execute_rollback(
    UmiHelixOrchestrator *orchestrator,
    char *out_evidence,
    size_t evidence_capacity)
{
    const UmiHelixAction *action;
    UmiStatus result;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestrator == NULL || !orchestrator->adapter_configured) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    action = umi_helix_compensation_plan_next(&orchestrator->rollback);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (action == NULL) return UMI_STATUS_INVALID_STATE;
    result = umi_helix_execution_adapter_execute(
        &orchestrator->adapter, action, 1, out_evidence, evidence_capacity);
    (void)umi_helix_compensation_plan_record(&orchestrator->rollback, result);
    (void)umi_helix_journal_append(
        &orchestrator->journal, UMI_HELIX_JOURNAL_ROLLBACK,
        orchestrator->operation.operation_id, action->summary);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result == UMI_STATUS_OK &&
        orchestrator->rollback.state == UMI_HELIX_ROLLBACK_COMPLETE) {
        return umi_helix_operation_transition(
            &orchestrator->operation, UMI_HELIX_OPERATION_ROLLED_BACK);
    }
    return result;
}
