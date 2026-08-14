/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/orchestrator_v2.c
 * PURPOSE: Implement controlled autonomous development orchestration.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/orchestrator_v2.h"

#include <stdio.h>
#include <string.h>

UmiHelixOrchestratorConfigV2 umi_helix_orchestrator_v2_config_default(void)
{
    UmiHelixOrchestratorConfigV2 config;
    config.maximum_attempts = 3U;
    config.minimum_fitness = 0.80;
    config.require_human_approval = 1;
    return config;
}

UmiStatus umi_helix_orchestrator_v2_init(
    UmiHelixOrchestratorV2 *orchestrator,
    const UmiHelixOrchestratorConfigV2 *config)
{
    UmiStatus status;
    if (orchestrator == NULL || config == NULL || config->maximum_attempts == 0U ||
        config->minimum_fitness < 0.0 || config->minimum_fitness > 1.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(orchestrator, 0, sizeof(*orchestrator));
    orchestrator->config = *config;
    umi_helix_approval_gate_v2_init(&orchestrator->approvals);
    umi_helix_journal_v2_init(&orchestrator->journal);
    umi_helix_candidate_set_v2_init(&orchestrator->candidates);
    umi_helix_rollback_v2_init(&orchestrator->rollback);
    status = umi_helix_retry_loop_v2_init(&orchestrator->retry,
                                          config->maximum_attempts);
    if (status == UMI_STATUS_OK) {
        status = umi_helix_agent_runtime_v2_register_defaults(
            &orchestrator->agents);
    }
    return status;
}

UmiStatus umi_helix_orchestrator_v2_set_adapter(
    UmiHelixOrchestratorV2 *orchestrator,
    const UmiHelixExecutionAdapterV2 *adapter)
{
    UmiStatus status;
    if (orchestrator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_helix_execution_adapter_v2_validate(adapter);
    if (status == UMI_STATUS_OK) {
        orchestrator->adapter = *adapter;
        orchestrator->adapter_configured = 1;
    }
    return status;
}

UmiStatus umi_helix_orchestrator_v2_plan(
    UmiHelixOrchestratorV2 *orchestrator,
    const char *operation_id,
    const char *objective,
    const char *plan_hash)
{
    UmiStatus status;
    if (orchestrator == NULL || orchestrator->operation_ready) {
        return orchestrator == NULL ? UMI_STATUS_INVALID_ARGUMENT
                                    : UMI_STATUS_INVALID_STATE;
    }
    status = umi_helix_operation_v2_init(&orchestrator->operation,
                                         operation_id, objective, plan_hash);
    if (status == UMI_STATUS_OK) status = umi_helix_operation_v2_transition(
        &orchestrator->operation, UMI_HELIX_OPERATION_PLANNED);
    if (status == UMI_STATUS_OK) status = umi_helix_journal_v2_append(
        &orchestrator->journal, UMI_HELIX_JOURNAL_PLAN, operation_id, objective);
    if (status == UMI_STATUS_OK) orchestrator->operation_ready = 1;
    return status;
}

UmiStatus umi_helix_orchestrator_v2_prepare_action(
    UmiHelixOrchestratorV2 *orchestrator,
    const UmiHelixActionV2 *action)
{
    UmiStatus status;
    int requires_approval;
    if (orchestrator == NULL || action == NULL || !orchestrator->operation_ready ||
        orchestrator->action_ready ||
        orchestrator->operation.state != UMI_HELIX_OPERATION_PLANNED) {
        return UMI_STATUS_INVALID_STATE;
    }
    orchestrator->current_action = *action;
    orchestrator->current_action.maximum_attempts =
        orchestrator->config.maximum_attempts;
    requires_approval = orchestrator->config.require_human_approval ||
        umi_helix_action_v2_requires_approval(action);
    if (requires_approval) {
        status = umi_helix_approval_gate_v2_request(
            &orchestrator->approvals, orchestrator->operation.operation_id,
            action->action_id, orchestrator->operation.plan_hash);
        if (status == UMI_STATUS_OK) status = umi_helix_operation_v2_transition(
            &orchestrator->operation, UMI_HELIX_OPERATION_AWAITING_APPROVAL);
    } else {
        status = umi_helix_operation_v2_transition(
            &orchestrator->operation, UMI_HELIX_OPERATION_APPROVED);
    }
    if (status == UMI_STATUS_OK) orchestrator->action_ready = 1;
    return status;
}

UmiStatus umi_helix_orchestrator_v2_decide(
    UmiHelixOrchestratorV2 *orchestrator,
    UmiHelixApprovalDecisionV2 decision,
    const char *approver,
    const char *reason)
{
    UmiStatus status;
    if (orchestrator == NULL || !orchestrator->action_ready ||
        orchestrator->operation.state != UMI_HELIX_OPERATION_AWAITING_APPROVAL) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_helix_approval_gate_v2_decide(
        &orchestrator->approvals, orchestrator->operation.operation_id,
        orchestrator->current_action.action_id, decision, approver, reason);
    if (status == UMI_STATUS_OK) status = umi_helix_journal_v2_append(
        &orchestrator->journal, UMI_HELIX_JOURNAL_APPROVAL,
        orchestrator->operation.operation_id,
        decision == UMI_HELIX_APPROVAL_APPROVED ? "Action approved"
                                                : "Action declined");
    if (status == UMI_STATUS_OK) status = umi_helix_operation_v2_transition(
        &orchestrator->operation,
        decision == UMI_HELIX_APPROVAL_APPROVED
            ? UMI_HELIX_OPERATION_APPROVED : UMI_HELIX_OPERATION_CANCELLED);
    return status;
}

UmiStatus umi_helix_orchestrator_v2_execute(
    UmiHelixOrchestratorV2 *orchestrator,
    char *out_evidence,
    size_t evidence_capacity)
{
    UmiStatus result;
    int approved;
    if (orchestrator == NULL || out_evidence == NULL || evidence_capacity == 0U ||
        !orchestrator->adapter_configured || !orchestrator->action_ready ||
        orchestrator->operation.state != UMI_HELIX_OPERATION_APPROVED) {
        return UMI_STATUS_INVALID_STATE;
    }
    approved = !orchestrator->config.require_human_approval &&
               !umi_helix_action_v2_requires_approval(
                   &orchestrator->current_action);
    if (!approved) approved = umi_helix_approval_gate_v2_is_approved(
        &orchestrator->approvals, orchestrator->operation.operation_id,
        orchestrator->current_action.action_id,
        orchestrator->operation.plan_hash);
    result = umi_helix_retry_loop_v2_begin(&orchestrator->retry);
    if (result != UMI_STATUS_OK) return result;
    orchestrator->current_action.attempt = orchestrator->retry.attempts_started;
    result = umi_helix_operation_v2_transition(
        &orchestrator->operation, UMI_HELIX_OPERATION_RUNNING);
    if (result != UMI_STATUS_OK) return result;
    result = umi_helix_execution_adapter_v2_execute(
        &orchestrator->adapter, &orchestrator->current_action, approved,
        out_evidence, evidence_capacity);
    (void)umi_helix_retry_loop_v2_record(&orchestrator->retry, result);
    (void)umi_helix_journal_v2_append(
        &orchestrator->journal, UMI_HELIX_JOURNAL_ACTION,
        orchestrator->operation.operation_id,
        orchestrator->current_action.summary);
    if (out_evidence[0] != '\0') {
        (void)umi_helix_journal_v2_append(
            &orchestrator->journal, UMI_HELIX_JOURNAL_EVIDENCE,
            orchestrator->operation.operation_id, out_evidence);
        orchestrator->operation.evidence_count++;
    }
    if (result == UMI_STATUS_OK) {
        orchestrator->operation.actions_completed++;
        return umi_helix_operation_v2_transition(
            &orchestrator->operation, UMI_HELIX_OPERATION_SUCCEEDED);
    }
    orchestrator->operation.retry_count++;
    (void)umi_helix_operation_v2_transition(
        &orchestrator->operation, UMI_HELIX_OPERATION_FAILED);
    if (umi_helix_retry_loop_v2_can_retry(&orchestrator->retry)) {
        (void)umi_helix_operation_v2_transition(
            &orchestrator->operation, UMI_HELIX_OPERATION_APPROVED);
    }
    return result;
}

UmiStatus umi_helix_orchestrator_v2_begin_rollback(
    UmiHelixOrchestratorV2 *orchestrator,
    int approved)
{
    UmiStatus status;
    if (orchestrator == NULL || !orchestrator->operation_ready) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_helix_rollback_v2_begin(&orchestrator->rollback, approved);
    if (status == UMI_STATUS_OK) status = umi_helix_operation_v2_transition(
        &orchestrator->operation, UMI_HELIX_OPERATION_ROLLING_BACK);
    if (status == UMI_STATUS_OK) status = umi_helix_journal_v2_append(
        &orchestrator->journal, UMI_HELIX_JOURNAL_ROLLBACK,
        orchestrator->operation.operation_id, "Rollback started");
    return status;
}

UmiStatus umi_helix_orchestrator_v2_execute_rollback(
    UmiHelixOrchestratorV2 *orchestrator,
    char *out_evidence,
    size_t evidence_capacity)
{
    const UmiHelixActionV2 *action;
    UmiStatus result;
    if (orchestrator == NULL || !orchestrator->adapter_configured) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    action = umi_helix_rollback_v2_next(&orchestrator->rollback);
    if (action == NULL) return UMI_STATUS_INVALID_STATE;
    result = umi_helix_execution_adapter_v2_execute(
        &orchestrator->adapter, action, 1, out_evidence, evidence_capacity);
    (void)umi_helix_rollback_v2_record(&orchestrator->rollback, result);
    (void)umi_helix_journal_v2_append(
        &orchestrator->journal, UMI_HELIX_JOURNAL_ROLLBACK,
        orchestrator->operation.operation_id, action->summary);
    if (result == UMI_STATUS_OK &&
        orchestrator->rollback.state == UMI_HELIX_ROLLBACK_COMPLETE) {
        return umi_helix_operation_v2_transition(
            &orchestrator->operation, UMI_HELIX_OPERATION_ROLLED_BACK);
    }
    return result;
}
