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

#include <stdlib.h>
#include <string.h>

/*
 * Provide the evidence state for operation used by this module and its client
 * applications.
 */
static UmiApplicationProductionEvidenceState evidence_state_for(
    const UmiApplicationProductionRuntime *runtime,
    const UmiApplicationProductionEvidenceRequirement *requirement)
{
    /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the application production runtime refresh acceptance operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_production_runtime_refresh_acceptance(
    UmiApplicationProductionRuntime *runtime)
{
    UmiApplicationProductionAcceptanceRule rule;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || !runtime->initialised)
        return UMI_STATUS_INVALID_ARGUMENT;
    rule = umi_application_production_acceptance_rule_default();
    status = umi_application_production_acceptance_report_evaluate(
        &rule, &runtime->binding, &runtime->readiness,
        &runtime->manifest_drift, &runtime->evidence_requirements,
        &runtime->evidence, &runtime->acceptance);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}

/*
 * Initialise application production runtime from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_production_runtime_init(
    const UmiProductApplicationAdoption *adoption,
    UmiApplicationCapabilityProbe probe, void *probe_context,
    UmiApplicationProductionRuntime *out_runtime)
{
    UmiProductSurfacePortfolio *surfaces;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adoption == NULL || out_runtime == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_runtime, 0, sizeof(*out_runtime));
    status = umi_application_production_binding_init(
        adoption, &out_runtime->binding);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* The portfolio contains hundreds of detailed surface records. Keep this
     * temporary build object on the heap so callers do not need a very large
     * native thread stack merely to initialise one thin application. */
    surfaces = (UmiProductSurfacePortfolio *)calloc(1U, sizeof(*surfaces));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surfaces == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_product_surface_portfolio_build(surfaces);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(surfaces);
        return status;
    }
    status = umi_application_production_panel_bindings_build(
        &out_runtime->binding, surfaces, &out_runtime->panels);
    /* Panel bindings copy each surface projection by value. Its component and
     * experience pointers refer to static catalogues, so the temporary
     * whole-suite container is no longer required after this call. */
    free(surfaces);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_layout_bindings_build(
        &out_runtime->binding, &out_runtime->layouts);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_feature_bindings_build(
        &out_runtime->binding, &out_runtime->features);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_command_bindings_build(
        &out_runtime->binding, &out_runtime->commands);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_capability_requirements_build(
        &out_runtime->binding, &out_runtime->requirements);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_capability_probe_run(
        &out_runtime->requirements, probe, probe_context,
        &out_runtime->probe_results);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_capability_readiness_evaluate(
        &out_runtime->probe_results, &out_runtime->readiness.capabilities);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_readiness_report_build(
        &out_runtime->binding, &out_runtime->readiness.capabilities,
        &out_runtime->readiness);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_manifest_contract_build(
        &out_runtime->binding, &out_runtime->manifest_contract);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_manifest_snapshot_capture(
        &out_runtime->binding, &out_runtime->manifest_snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_manifest_drift_compare(
        &out_runtime->manifest_contract, &out_runtime->manifest_snapshot,
        &out_runtime->manifest_drift);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_application_production_evidence_requirements_build(
        &out_runtime->binding, &out_runtime->evidence_requirements);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    umi_application_production_evidence_registry_init(&out_runtime->evidence);
    status = umi_application_workspace_runtime_init(
        out_runtime->binding.experience, &out_runtime->workspace);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    umi_application_production_checkpoint_store_init(
        &out_runtime->checkpoints);
    out_runtime->recovery.status = UMI_STATUS_OK;
    status = umi_application_production_launch_plan_build(
        &out_runtime->launch_plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_runtime->revision = 1U;
    out_runtime->initialised = 1;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < out_runtime->evidence_requirements.count;
         ++index) {
        const UmiApplicationProductionEvidenceRequirement *requirement =
            &out_runtime->evidence_requirements.entries[index];
        status = umi_application_production_evidence_registry_record(
            &out_runtime->evidence, requirement->evidence_id,
            requirement->kind,
            evidence_state_for(out_runtime, requirement),
            "framework:auto-observed");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return umi_application_production_runtime_refresh_acceptance(out_runtime);
}

/*
 * Provide the application production runtime checkpoint operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_production_runtime_checkpoint(
    UmiApplicationProductionRuntime *runtime, const char *reason,
    int clean_shutdown)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL || !runtime->initialised)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_checkpoint_store_put(
        &runtime->checkpoints, &runtime->workspace, reason, clean_shutdown);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) runtime->revision += 1U;
    return status;
}
