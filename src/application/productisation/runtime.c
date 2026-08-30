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

UmiStatus umi_productisation_runtime_create(
    UmiProductisationRuntime **out_runtime)
{
    UmiProductisationRuntime *runtime;
    UmiStatus status;
    if (out_runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_runtime = NULL;
    runtime = malloc(sizeof(*runtime));
    if (runtime == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_productisation_runtime_init(runtime);
    if (status != UMI_STATUS_OK) {
        free(runtime);
        return status;
    }
    *out_runtime = runtime;
    return UMI_STATUS_OK;
}

void umi_productisation_runtime_destroy(UmiProductisationRuntime *runtime)
{
    free(runtime);
}

UmiStatus umi_productisation_runtime_refresh(
    UmiProductisationRuntime *runtime)
{
    UmiStatus status;
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_product_portfolio_inventory_build(&runtime->inventory);
    if (status != UMI_STATUS_OK) return status;
    status = umi_product_capability_matrix_build(&runtime->capability_matrix);
    if (status != UMI_STATUS_OK) return status;
    status = umi_product_portfolio_coverage_build(
        &runtime->evidence, &runtime->coverage);
    if (status != UMI_STATUS_OK) return status;
    status = umi_productisation_gap_analysis_run(
        &runtime->capability_matrix, &runtime->evidence, &runtime->gaps);
    if (status != UMI_STATUS_OK) return status;
    status = umi_productisation_completion_plan_build(
        &runtime->gaps, &runtime->completion_plan);
    if (status != UMI_STATUS_OK) return status;
    status = umi_productisation_release_gate_evaluate(
        &runtime->inventory, &runtime->coverage, &runtime->gaps,
        &runtime->completion_plan, &runtime->evidence,
        &runtime->release_gate);
    if (status != UMI_STATUS_OK) return status;
    runtime->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_productisation_runtime_init(UmiProductisationRuntime *runtime)
{
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(runtime, 0, sizeof(*runtime));
    umi_productisation_evidence_ledger_init(&runtime->evidence);
    return umi_productisation_runtime_refresh(runtime);
}

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
    if (runtime == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_productisation_evidence_record(
        &runtime->evidence, application_id, asset_kind, asset_id,
        frontend_id, state, reference);
    if (status != UMI_STATUS_OK) return status;
    return umi_productisation_runtime_refresh(runtime);
}
