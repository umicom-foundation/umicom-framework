/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/runtime.c
 *
 * PURPOSE:
 *   Refresh one internally consistent productisation snapshot after catalogue
 *   or evidence changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/runtime.h"

#include <stdlib.h>
#include <string.h>

/*
 * Initialise productisation runtime from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_productisation_runtime_create(
    UmiProductisationRuntime **out_runtime)
{
    UmiProductisationRuntime *runtime;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_runtime = NULL;
    runtime = malloc(sizeof(*runtime));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_productisation_runtime_init(runtime);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        free(runtime);
        return status;
    }
    *out_runtime = runtime;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by productisation runtime so the same storage can be reused
 * safely.
 */
void umi_productisation_runtime_destroy(UmiProductisationRuntime *runtime)
{
    free(runtime);
}

/*
 * Provide the productisation runtime refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_productisation_runtime_refresh(
    UmiProductisationRuntime *runtime)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_product_portfolio_inventory_build(&runtime->inventory);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_product_capability_matrix_build(&runtime->capability_matrix);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_product_portfolio_coverage_build(
        &runtime->evidence, &runtime->coverage);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_productisation_gap_analysis_run(
        &runtime->capability_matrix, &runtime->evidence, &runtime->gaps);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_productisation_completion_plan_build(
        &runtime->gaps, &runtime->completion_plan);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_productisation_release_gate_evaluate(
        &runtime->inventory, &runtime->coverage, &runtime->gaps,
        &runtime->completion_plan, &runtime->evidence,
        &runtime->release_gate);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    runtime->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Initialise productisation runtime from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_productisation_runtime_init(UmiProductisationRuntime *runtime)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(runtime, 0, sizeof(*runtime));
    umi_productisation_evidence_ledger_init(&runtime->evidence);
    return umi_productisation_runtime_refresh(runtime);
}

/*
 * Provide the productisation runtime record evidence operation used by this module and its
 * client applications.
 */
UmiStatus umi_productisation_runtime_record_evidence(
    UmiProductisationRuntime *runtime,
    const char *application_id,
    UmiProductisationAssetKind asset_kind,
    const char *asset_id,
    const char *frontend_id,
    UmiProductisationEvidenceState state,
    const char *reference)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_productisation_evidence_record(
        &runtime->evidence, application_id, asset_kind, asset_id,
        frontend_id, state, reference);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    return umi_productisation_runtime_refresh(runtime);
}
