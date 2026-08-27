/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/runtime.h
 *
 * PURPOSE:
 *   Coordinate inventory, capability coverage, evidence, gaps, completion
 *   planning and release gates for all canonical Umicom applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_RUNTIME_H
#define UMICOM_APPLICATION_PRODUCTISATION_RUNTIME_H

#include "umicom/application/productisation/capability_matrix.h"
#include "umicom/application/productisation/release_gate.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiProductisationRuntime {
    UmiProductisationEvidenceLedger evidence;
    UmiProductPortfolioInventory inventory;
    UmiProductCapabilityMatrix capability_matrix;
    UmiProductPortfolioCoverage coverage;
    UmiProductisationGapReport gaps;
    UmiProductisationCompletionPlan completion_plan;
    UmiProductisationReleaseGate release_gate;
    uint64_t revision;
} UmiProductisationRuntime;

UmiStatus umi_productisation_runtime_create(
    UmiProductisationRuntime **out_runtime);
void umi_productisation_runtime_destroy(UmiProductisationRuntime *runtime);
UmiStatus umi_productisation_runtime_init(UmiProductisationRuntime *runtime);
UmiStatus umi_productisation_runtime_refresh(
    UmiProductisationRuntime *runtime);
UmiStatus umi_productisation_runtime_record_evidence(
    UmiProductisationRuntime *runtime,
    const char *application_id,
    UmiProductisationAssetKind asset_kind,
    const char *asset_id,
    const char *frontend_id,
    UmiProductisationEvidenceState state,
    const char *reference);

#ifdef __cplusplus
}
#endif

#endif
