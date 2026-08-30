/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/runtime.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/runtime.h"

#include <string.h>

static UmiApplicationProductionEvidenceState evidence_state_for(
    const UmiApplicationProductionRuntime *runtime,
    const UmiApplicationProductionEvidenceRequirement *requirement)
{
    switch (requirement->kind) {
    case UMI_APPLICATION_PRODUCTION_EVIDENCE_MANIFEST:
        return runtime->manifest_drift.compatible
            ? UMI_APPLICATION_PRODUCTION_EVIDENCE_ACCEPTED
            : UMI_APPLICATION_PRODUCTION_EVIDENCE_REJECTED;
    case UMI_APPLICATION_PRODUCTION_EVIDENCE_LAYOUT:
        return runtime->readiness.layout_ready
            ? UMI_APPLICATION_PRODUCTION_EVIDENCE_ACCEPTED
            : UMI_APPLICATION_PRODUCTION_EVIDENCE_REJECTED;
    case UMI_APPLICATION_PRODUCTION_EVIDENCE_CAPABILITY:
        return umi_application_production_capability_readiness_launchable(
                   &runtime->readiness.capabilities)
            ? UMI_APPLICATION_PRODUCTION_EVIDENCE_ACCEPTED
            : UMI_APPLICATION_PRODUCTION_EVIDENCE_PENDING;
    case UMI_APPLICATION_PRODUCTION_EVIDENCE_TEST:
        return runtime->readiness.tests_ready
            ? UMI_APPLICATION_PRODUCTION_EVIDENCE_ACCEPTED
            : UMI_APPLICATION_PRODUCTION_EVIDENCE_PENDING;
    case UMI_APPLICATION_PRODUCTION_EVIDENCE_ACCEPTANCE:
        return runtime->binding.adoption_snapshot.acceptance_ready
            ? UMI_APPLICATION_PRODUCTION_EVIDENCE_ACCEPTED
            : UMI_APPLICATION_PRODUCTION_EVIDENCE_PENDING;
    default:
        return UMI_APPLICATION_PRODUCTION_EVIDENCE_MISSING;
    }
}

UmiStatus umi_application_production_runtime_refresh_acceptance(
    UmiApplicationProductionRuntime *runtime)
{
    UmiApplicationProductionAcceptanceRule rule;
    UmiStatus status;
    if (runtime == NULL || !runtime->initialised)
        return UMI_STATUS_INVALID_ARGUMENT;
    rule = umi_application_production_acceptance_rule_default();
    status = umi_application_production_acceptance_report_evaluate(
        &rule, &runtime->binding, &runtime->readiness,
        &runtime->manifest_drift, &runtime->evidence_requirements,
        &runtime->evidence, &runtime->acceptance);
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

UmiStatus umi_application_production_runtime_init(
    const UmiProductApplicationAdoption *adoption,
    UmiApplicationCapabilityProbe probe, void *probe_context,
    UmiApplicationProductionRuntime *out_runtime)
{
    UmiProductSurfacePortfolio surfaces;
    size_t index;
    UmiStatus status;
    if (adoption == NULL || out_runtime == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_runtime, 0, sizeof(*out_runtime));
    status = umi_application_production_binding_init(
        adoption, &out_runtime->binding);
    if (status != UMI_STATUS_OK) return status;
    status = umi_product_surface_portfolio_build(&surfaces);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_panel_bindings_build(
        &out_runtime->binding, &surfaces, &out_runtime->panels);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_layout_bindings_build(
        &out_runtime->binding, &out_runtime->layouts);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_feature_bindings_build(
        &out_runtime->binding, &out_runtime->features);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_command_bindings_build(
        &out_runtime->binding, &out_runtime->commands);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_capability_requirements_build(
        &out_runtime->binding, &out_runtime->requirements);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_capability_probe_run(
        &out_runtime->requirements, probe, probe_context,
        &out_runtime->probe_results);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_capability_readiness_evaluate(
        &out_runtime->probe_results, &out_runtime->readiness.capabilities);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_readiness_report_build(
        &out_runtime->binding, &out_runtime->readiness.capabilities,
        &out_runtime->readiness);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_manifest_contract_build(
        &out_runtime->binding, &out_runtime->manifest_contract);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_manifest_snapshot_capture(
        &out_runtime->binding, &out_runtime->manifest_snapshot);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_manifest_drift_compare(
        &out_runtime->manifest_contract, &out_runtime->manifest_snapshot,
        &out_runtime->manifest_drift);
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_evidence_requirements_build(
        &out_runtime->binding, &out_runtime->evidence_requirements);
    if (status != UMI_STATUS_OK) return status;
    umi_application_production_evidence_registry_init(&out_runtime->evidence);
    status = umi_application_workspace_runtime_init(
        out_runtime->binding.experience, &out_runtime->workspace);
    if (status != UMI_STATUS_OK) return status;
    umi_application_production_checkpoint_store_init(
        &out_runtime->checkpoints);
    out_runtime->recovery.status = UMI_STATUS_OK;
    status = umi_application_production_launch_plan_build(
        &out_runtime->launch_plan);
    if (status != UMI_STATUS_OK) return status;
    out_runtime->revision = 1U;
    out_runtime->initialised = 1;
    for (index = 0U; index < out_runtime->evidence_requirements.count;
         ++index) {
        const UmiApplicationProductionEvidenceRequirement *requirement =
            &out_runtime->evidence_requirements.entries[index];
        status = umi_application_production_evidence_registry_record(
            &out_runtime->evidence, requirement->evidence_id,
            requirement->kind,
            evidence_state_for(out_runtime, requirement),
            "framework:auto-observed");
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_application_production_runtime_refresh_acceptance(out_runtime);
}

UmiStatus umi_application_production_runtime_checkpoint(
    UmiApplicationProductionRuntime *runtime, const char *reason,
    int clean_shutdown)
{
    UmiStatus status;
    if (runtime == NULL || !runtime->initialised)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_checkpoint_store_put(
        &runtime->checkpoints, &runtime->workspace, reason, clean_shutdown);
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

