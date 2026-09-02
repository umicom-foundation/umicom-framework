/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/orchestration.c
 *
 * PURPOSE:
 *   Implement the Framework-owned diagnostics workflow from provider findings
 *   through severity policy, auditable suppression, baseline classification,
 *   the canonical Problems model and optional immutable diagnostic mirroring.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/orchestration.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDiagnosticOrchestration {
    UmiDiagnosticOrchestrationConfig config;
    UmiDiagnosticProviderRegistry *providers;
    UmiDiagnosticSeverityPolicy *severity_policy;
    UmiDiagnosticSuppressionSet *suppressions;
    UmiDiagnosticBaseline *baseline;
    UmiDiagnosticProblemModel *problems;
    UmiDiagnosticProviderBatch *last_batch;
    UmiDiagnosticModel *diagnostic_model;
    UmiDiagnosticOrchestrationState state;
    char active_provider_id[UMI_DIAGNOSTIC_PROVIDER_ID_CAPACITY];
    uint64_t active_run_id;
    size_t ingested_count;
    size_t accepted_count;
    size_t suppressed_count;
    size_t disabled_count;
    size_t new_count;
    size_t updated_count;
    size_t resolved_count;
    uint64_t revision;
};

/* Provide the next revision operation used by this module and its client applications. */
static uint64_t next_revision(uint64_t value)
{
    return value == UINT64_MAX ? 1U : value + 1U;
}

/*
 * Provide the reset run counters operation used by this module and its client
 * applications.
 */
static void reset_run_counters(UmiDiagnosticOrchestration *orchestration)
{
    orchestration->ingested_count = 0U;
    orchestration->accepted_count = 0U;
    orchestration->suppressed_count = 0U;
    orchestration->disabled_count = 0U;
    orchestration->new_count = 0U;
    orchestration->updated_count = 0U;
    orchestration->resolved_count = 0U;
}

/*
 * Provide the initialize provider suppression operation used by this module and its client
 * applications.
 */
static void initialize_provider_suppression(
    const UmiDiagnosticProviderFinding *finding,
    UmiDiagnosticSuppressionDecision *decision)
{
    (void)memset(decision, 0, sizeof(*decision));
    decision->struct_size = (uint32_t)sizeof(*decision);
    decision->api_version = UMI_DIAGNOSTIC_SUPPRESSION_API_VERSION;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!finding->suppressed) return;
    decision->matched = 1;
    decision->suppressed = 1;
    decision->kind = finding->suppression_kind ==
            UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_IN_SOURCE
        ? UMI_DIAGNOSTIC_SUPPRESSION_IN_SOURCE
        : UMI_DIAGNOSTIC_SUPPRESSION_EXTERNAL;
    /* Select the behaviour associated with the requested command or state value. */
    switch (finding->suppression_status) {
        case UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_UNDER_REVIEW:
            decision->status = UMI_DIAGNOSTIC_SUPPRESSION_UNDER_REVIEW;
            break;
        case UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_REJECTED:
            decision->status = UMI_DIAGNOSTIC_SUPPRESSION_REJECTED;
            decision->suppressed = 0;
            break;
        case UMI_DIAGNOSTIC_PROVIDER_SUPPRESSION_ACCEPTED:
        default:
            decision->status = UMI_DIAGNOSTIC_SUPPRESSION_ACCEPTED;
            break;
    }
    (void)snprintf(decision->reason, sizeof(decision->reason), "%s",
                   finding->suppression_justification[0] != '\0'
                       ? finding->suppression_justification
                       : "provider supplied suppression");
    (void)snprintf(decision->owner, sizeof(decision->owner), "%s",
                   finding->diagnostic.owner[0] != '\0'
                       ? finding->diagnostic.owner : finding->provider_id);
}

/* Provide the mirror problem operation used by this module and its client applications. */
static UmiStatus mirror_problem(UmiDiagnosticOrchestration *orchestration,
                                const UmiDiagnosticProblem *problem)
{
    UmiDiagnosticSnapshot snapshot;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!orchestration->config.mirror_to_diagnostic_model ||
        orchestration->diagnostic_model == NULL) {
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (problem->policy_disabled &&
        !orchestration->config.retain_disabled_in_problem_model) {
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (problem->suppression.suppressed &&
        !orchestration->config.retain_suppressed_in_diagnostic_model) {
        return UMI_STATUS_OK;
    }
    snapshot = problem->finding.diagnostic;
    snapshot.severity = problem->effective_severity;
    snapshot.resolved = !problem->active;
    return umi_diagnostic_model_upsert(orchestration->diagnostic_model,
                                       &snapshot);
}

/* Provide the process finding operation used by this module and its client applications. */
static UmiStatus process_finding(UmiDiagnosticOrchestration *orchestration,
                                 const UmiDiagnosticProviderFinding *source,
                                 uint64_t timestamp_ns)
{
    UmiDiagnosticProviderFinding finding;
    UmiDiagnosticSeverityDecision severity;
    UmiDiagnosticSuppressionDecision suppression;
    UmiDiagnosticSuppressionDecision rule_suppression;
    UmiDiagnosticBaselineDecision baseline;
    UmiDiagnosticProblem problem;
    UmiStatus status;

    finding = *source;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (finding.run_id == 0U) finding.run_id = orchestration->active_run_id;
    /* Apply this branch only when its contract condition is satisfied. */
    if (finding.diagnostic.timestamp_ns == 0U) {
        finding.diagnostic.timestamp_ns = timestamp_ns;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (finding.fingerprint == 0U) {
        finding.fingerprint = umi_diagnostic_provider_fingerprint(&finding);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (finding.content_fingerprint == 0U) {
        finding.content_fingerprint =
            umi_diagnostic_provider_content_fingerprint(&finding);
    }
    status = umi_diagnostic_provider_finding_validate(&finding);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_diagnostic_severity_policy_evaluate(
        orchestration->severity_policy, &finding, &severity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    initialize_provider_suppression(&finding, &suppression);
    status = umi_diagnostic_suppression_set_evaluate(
        orchestration->suppressions, &finding, timestamp_ns,
        &rule_suppression);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (rule_suppression.matched) suppression = rule_suppression;

    status = umi_diagnostic_baseline_compare(orchestration->baseline,
                                             &finding, &baseline);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    (void)memset(&problem, 0, sizeof(problem));
    problem.struct_size = (uint32_t)sizeof(problem);
    problem.api_version = UMI_DIAGNOSTIC_PROBLEM_MODEL_API_VERSION;
    problem.finding = finding;
    problem.original_severity = finding.diagnostic.severity;
    problem.effective_severity = severity.effective_severity;
    problem.finding.diagnostic.severity = severity.effective_severity;
    problem.baseline_state = baseline.state;
    problem.suppression = suppression;
    (void)snprintf(problem.severity_rule_id,
                   sizeof(problem.severity_rule_id), "%s",
                   severity.rule_id);
    problem.last_seen_run_id = orchestration->active_run_id;
    problem.active = !finding.diagnostic.resolved;
    problem.policy_disabled = severity.disabled;
    problem.fixable = finding.has_fix;

    /* Apply this branch only when its contract condition is satisfied. */
    if (severity.disabled &&
        !orchestration->config.retain_disabled_in_problem_model) {
        ++orchestration->disabled_count;
        return UMI_STATUS_OK;
    }
    status = umi_diagnostic_problem_model_upsert(orchestration->problems,
                                                  &problem);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = mirror_problem(orchestration, &problem);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    ++orchestration->accepted_count;
    /* Apply this branch only when its contract condition is satisfied. */
    if (severity.disabled) ++orchestration->disabled_count;
    /* Apply this branch only when its contract condition is satisfied. */
    if (suppression.suppressed) ++orchestration->suppressed_count;
    /* Apply this branch only when its contract condition is satisfied. */
    if (baseline.state == UMI_DIAGNOSTIC_BASELINE_NEW) {
        ++orchestration->new_count;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (baseline.state == UMI_DIAGNOSTIC_BASELINE_UPDATED) {
        ++orchestration->updated_count;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the mirror resolved provider findings operation used by this module and its
 * client applications.
 */
static UmiStatus mirror_resolved_provider_findings(
    UmiDiagnosticOrchestration *orchestration)
{
    size_t position;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!orchestration->config.mirror_to_diagnostic_model ||
        orchestration->diagnostic_model == NULL) return UMI_STATUS_OK;
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U;
         position < umi_diagnostic_problem_model_count(orchestration->problems);
         ++position) {
        UmiDiagnosticProblem problem;
        UmiStatus status = umi_diagnostic_problem_model_at(
            orchestration->problems, position, &problem);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(problem.finding.provider_id,
                   orchestration->active_provider_id) != 0 ||
            problem.active) continue;
        status = mirror_problem(orchestration, &problem);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic orchestration config default operation used by this module and
 * its client applications.
 */
UmiDiagnosticOrchestrationConfig umi_diagnostic_orchestration_config_default(void)
{
    UmiDiagnosticOrchestrationConfig config;
    (void)memset(&config, 0, sizeof(config));
    config.mirror_to_diagnostic_model = 1;
    config.retain_disabled_in_problem_model = 1;
    config.resolve_missing_provider_findings = 1;
    return config;
}

/*
 * Initialise diagnostic orchestration from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_diagnostic_orchestration_create(
    const UmiDiagnosticOrchestrationConfig *config,
    UmiDiagnosticOrchestration **out_orchestration)
{
    UmiDiagnosticOrchestration *orchestration;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_orchestration = NULL;
    orchestration = (UmiDiagnosticOrchestration *)calloc(
        1U, sizeof(*orchestration));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    orchestration->config = config != NULL
        ? *config : umi_diagnostic_orchestration_config_default();
    orchestration->config.mirror_to_diagnostic_model =
        orchestration->config.mirror_to_diagnostic_model != 0;
    orchestration->config.retain_suppressed_in_diagnostic_model =
        orchestration->config.retain_suppressed_in_diagnostic_model != 0;
    orchestration->config.retain_disabled_in_problem_model =
        orchestration->config.retain_disabled_in_problem_model != 0;
    orchestration->config.resolve_missing_provider_findings =
        orchestration->config.resolve_missing_provider_findings != 0;
    status = umi_diagnostic_provider_registry_create(&orchestration->providers);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_diagnostic_severity_policy_create(
            &orchestration->severity_policy);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_diagnostic_suppression_set_create(
            &orchestration->suppressions);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_diagnostic_baseline_create(&orchestration->baseline);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_diagnostic_problem_model_create(&orchestration->problems);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_diagnostic_provider_batch_create(&orchestration->last_batch);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_diagnostic_orchestration_destroy(orchestration);
        return status;
    }
    orchestration->state = UMI_DIAGNOSTIC_ORCHESTRATION_IDLE;
    orchestration->revision = 1U;
    *out_orchestration = orchestration;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by diagnostic orchestration so the same storage can be
 * reused safely.
 */
void umi_diagnostic_orchestration_destroy(
    UmiDiagnosticOrchestration *orchestration)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return;
    umi_diagnostic_provider_batch_destroy(orchestration->last_batch);
    umi_diagnostic_problem_model_destroy(orchestration->problems);
    umi_diagnostic_baseline_destroy(orchestration->baseline);
    umi_diagnostic_suppression_set_destroy(orchestration->suppressions);
    umi_diagnostic_severity_policy_destroy(orchestration->severity_policy);
    umi_diagnostic_provider_registry_destroy(orchestration->providers);
    free(orchestration);
}

/*
 * Provide the diagnostic orchestration bind model operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_orchestration_bind_model(
    UmiDiagnosticOrchestration *orchestration,
    UmiDiagnosticModel *diagnostic_model)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (orchestration->state == UMI_DIAGNOSTIC_ORCHESTRATION_RUNNING) {
        return UMI_STATUS_BUSY;
    }
    orchestration->diagnostic_model = diagnostic_model;
    orchestration->revision = next_revision(orchestration->revision);
    return UMI_STATUS_OK;
}

/*
 * Perform diagnostic orchestration begin provider through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_diagnostic_orchestration_begin_provider_run(
    UmiDiagnosticOrchestration *orchestration,
    const char *provider_id,
    uint64_t run_id)
{
    UmiDiagnosticProviderDescriptor provider;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL || provider_id == NULL || provider_id[0] == '\0' ||
        run_id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (orchestration->state == UMI_DIAGNOSTIC_ORCHESTRATION_RUNNING) {
        return UMI_STATUS_BUSY;
    }
    status = umi_diagnostic_provider_registry_find(orchestration->providers,
                                                    provider_id, &provider);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!provider.enabled) return UMI_STATUS_UNAVAILABLE;
    status = umi_diagnostic_problem_model_begin_provider_run(
        orchestration->problems, provider_id, run_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_diagnostic_baseline_begin_comparison(
            orchestration->baseline);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_diagnostic_provider_batch_clear(orchestration->last_batch);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(orchestration->active_provider_id,
                   sizeof(orchestration->active_provider_id), "%s",
                   provider_id);
    orchestration->active_run_id = run_id;
    reset_run_counters(orchestration);
    orchestration->state = UMI_DIAGNOSTIC_ORCHESTRATION_RUNNING;
    orchestration->revision = next_revision(orchestration->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic orchestration ingest batch operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_orchestration_ingest_batch(
    UmiDiagnosticOrchestration *orchestration,
    const UmiDiagnosticProviderBatch *batch,
    uint64_t timestamp_ns)
{
    size_t position;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL || batch == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (orchestration->state != UMI_DIAGNOSTIC_ORCHESTRATION_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U; position < umi_diagnostic_provider_batch_count(batch);
         ++position) {
        UmiDiagnosticProviderFinding finding;
        UmiStatus status = umi_diagnostic_provider_batch_at(batch, position,
                                                             &finding);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(finding.provider_id,
                   orchestration->active_provider_id) != 0) {
            return UMI_STATUS_INVALID_ARGUMENT;
        }
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (finding.run_id == 0U) finding.run_id = orchestration->active_run_id;
        status = umi_diagnostic_provider_batch_upsert(orchestration->last_batch,
                                                       &finding);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        ++orchestration->ingested_count;
        status = process_finding(orchestration, &finding, timestamp_ns);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            orchestration->state = UMI_DIAGNOSTIC_ORCHESTRATION_FAILED;
            orchestration->revision = next_revision(orchestration->revision);
            return status;
        }
    }
    orchestration->revision = next_revision(orchestration->revision);
    return UMI_STATUS_OK;
}

/*
 * Perform diagnostic orchestration finish provider through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_diagnostic_orchestration_finish_provider_run(
    UmiDiagnosticOrchestration *orchestration)
{
    UmiDiagnosticBaselineDiff diff;
    size_t resolved = 0U;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (orchestration->state != UMI_DIAGNOSTIC_ORCHESTRATION_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_diagnostic_baseline_finish_comparison(
        orchestration->baseline, &diff);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK &&
        orchestration->config.resolve_missing_provider_findings) {
        status = umi_diagnostic_problem_model_finish_provider_run(
            orchestration->problems,
            orchestration->active_provider_id,
            orchestration->active_run_id,
            &resolved);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        orchestration->resolved_count = resolved;
        status = mirror_resolved_provider_findings(orchestration);
    }
    orchestration->state = status == UMI_STATUS_OK
        ? UMI_DIAGNOSTIC_ORCHESTRATION_COMPLETED
        : UMI_DIAGNOSTIC_ORCHESTRATION_FAILED;
    orchestration->revision = next_revision(orchestration->revision);
    return status;
}

/*
 * Provide the diagnostic orchestration cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_diagnostic_orchestration_cancel(
    UmiDiagnosticOrchestration *orchestration)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (orchestration->state != UMI_DIAGNOSTIC_ORCHESTRATION_RUNNING) {
        return UMI_STATUS_INVALID_STATE;
    }
    orchestration->state = UMI_DIAGNOSTIC_ORCHESTRATION_CANCELLED;
    orchestration->revision = next_revision(orchestration->revision);
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic orchestration reapply operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_orchestration_reapply(
    UmiDiagnosticOrchestration *orchestration,
    uint64_t timestamp_ns)
{
    size_t position;
    UmiDiagnosticBaselineDiff diff;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (orchestration->state == UMI_DIAGNOSTIC_ORCHESTRATION_RUNNING ||
        orchestration->active_provider_id[0] == '\0' ||
        orchestration->active_run_id == 0U) {
        return UMI_STATUS_INVALID_STATE;
    }
    reset_run_counters(orchestration);
    status = umi_diagnostic_baseline_begin_comparison(orchestration->baseline);
    /* Visit each bounded item once so every record receives the same rule. */
    for (position = 0U;
         status == UMI_STATUS_OK &&
         position < umi_diagnostic_provider_batch_count(orchestration->last_batch);
         ++position) {
        UmiDiagnosticProviderFinding finding;
        status = umi_diagnostic_provider_batch_at(orchestration->last_batch,
                                                   position, &finding);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) {
            ++orchestration->ingested_count;
            status = process_finding(orchestration, &finding, timestamp_ns);
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_diagnostic_baseline_finish_comparison(
            orchestration->baseline, &diff);
    }
    orchestration->state = status == UMI_STATUS_OK
        ? UMI_DIAGNOSTIC_ORCHESTRATION_COMPLETED
        : UMI_DIAGNOSTIC_ORCHESTRATION_FAILED;
    orchestration->revision = next_revision(orchestration->revision);
    return status;
}

/*
 * Provide the diagnostic orchestration capture baseline operation used by this module and
 * its client applications.
 */
UmiStatus umi_diagnostic_orchestration_capture_baseline(
    UmiDiagnosticOrchestration *orchestration)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (orchestration->state == UMI_DIAGNOSTIC_ORCHESTRATION_RUNNING) {
        return UMI_STATUS_BUSY;
    }
    status = umi_diagnostic_baseline_capture_batch(orchestration->baseline,
                                                    orchestration->last_batch);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        orchestration->revision = next_revision(orchestration->revision);
    }
    return status;
}

/*
 * Provide the diagnostic orchestration import sarif operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_orchestration_import_sarif(
    UmiDiagnosticOrchestration *orchestration,
    const char *json,
    size_t json_size,
    const char *provider_id,
    uint64_t timestamp_ns,
    UmiDiagnosticSarifImportSummary *out_summary)
{
    UmiDiagnosticProviderDescriptor provider;
    UmiDiagnosticProviderBatch *batch = NULL;
    UmiDiagnosticSarifImportSummary summary;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_diagnostic_provider_batch_create(&batch);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_diagnostic_sarif_import(json, json_size, provider_id,
                                             &provider, batch, &summary);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_diagnostic_provider_registry_upsert(
            orchestration->providers, &provider);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        uint64_t run_id = summary.run_id != 0U ? summary.run_id : 1U;
        status = umi_diagnostic_orchestration_begin_provider_run(
            orchestration, provider_id, run_id);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_diagnostic_orchestration_ingest_batch(
            orchestration, batch, timestamp_ns);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_diagnostic_orchestration_finish_provider_run(orchestration);
    }
    umi_diagnostic_provider_batch_destroy(batch);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && out_summary != NULL) *out_summary = summary;
    return status;
}

/*
 * Provide the diagnostic orchestration export sarif operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_orchestration_export_sarif(
    const UmiDiagnosticOrchestration *orchestration,
    const UmiDiagnosticSarifRunDescriptor *run,
    const UmiDiagnosticSarifExportOptions *options,
    char **out_json,
    size_t *out_size)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_diagnostic_sarif_export(orchestration->problems, run, options,
                                       out_json, out_size);
}

/*
 * Provide the diagnostic orchestration snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_diagnostic_orchestration_snapshot(
    const UmiDiagnosticOrchestration *orchestration,
    UmiDiagnosticOrchestrationSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (orchestration == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = UMI_DIAGNOSTIC_ORCHESTRATION_API_VERSION;
    out_snapshot->state = orchestration->state;
    (void)snprintf(out_snapshot->active_provider_id,
                   sizeof(out_snapshot->active_provider_id), "%s",
                   orchestration->active_provider_id);
    out_snapshot->active_run_id = orchestration->active_run_id;
    out_snapshot->ingested_count = orchestration->ingested_count;
    out_snapshot->accepted_count = orchestration->accepted_count;
    out_snapshot->suppressed_count = orchestration->suppressed_count;
    out_snapshot->disabled_count = orchestration->disabled_count;
    out_snapshot->new_count = orchestration->new_count;
    out_snapshot->updated_count = orchestration->updated_count;
    out_snapshot->resolved_count = orchestration->resolved_count;
    out_snapshot->revision = orchestration->revision;
    return UMI_STATUS_OK;
}

/*
 * Provide the diagnostic orchestration providers operation used by this module and its
 * client applications.
 */
UmiDiagnosticProviderRegistry *umi_diagnostic_orchestration_providers(
    UmiDiagnosticOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->providers : NULL;
}

/*
 * Provide the diagnostic orchestration severity policy operation used by this module and
 * its client applications.
 */
UmiDiagnosticSeverityPolicy *umi_diagnostic_orchestration_severity_policy(
    UmiDiagnosticOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->severity_policy : NULL;
}

/*
 * Provide the diagnostic orchestration suppressions operation used by this module and its
 * client applications.
 */
UmiDiagnosticSuppressionSet *umi_diagnostic_orchestration_suppressions(
    UmiDiagnosticOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->suppressions : NULL;
}

/*
 * Provide the diagnostic orchestration baseline operation used by this module and its
 * client applications.
 */
UmiDiagnosticBaseline *umi_diagnostic_orchestration_baseline(
    UmiDiagnosticOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->baseline : NULL;
}

/*
 * Provide the diagnostic orchestration problems operation used by this module and its
 * client applications.
 */
UmiDiagnosticProblemModel *umi_diagnostic_orchestration_problems(
    UmiDiagnosticOrchestration *orchestration)
{
    return orchestration != NULL ? orchestration->problems : NULL;
}
