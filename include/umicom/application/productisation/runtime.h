/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/runtime.h
 *
 * PURPOSE:
 *   Coordinate inventory, capability coverage, evidence, gaps, completion
 *   planning and release gates for all canonical Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_RUNTIME_H
#define UMICOM_APPLICATION_PRODUCTISATION_RUNTIME_H

#include "umicom/application/productisation/capability_matrix.h"
#include "umicom/application/productisation/release_gate.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the productisation runtime data shared with callers of this public contract.
 */
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

/**
 * Allocate and initialise a productisation runtime.
 *
 * @param out_runtime Receives the new caller-owned runtime.
 * @return `UMI_STATUS_OK` on success or an allocation or validation status.
 */
UmiStatus umi_productisation_runtime_create(
    UmiProductisationRuntime **out_runtime);

/**
 * Release a runtime created by `umi_productisation_runtime_create`.
 *
 * Passing `NULL` is safe. The pointer must not be used after this call.
 *
 * @param runtime Runtime to release.
 */
void umi_productisation_runtime_destroy(UmiProductisationRuntime *runtime);

/**
 * Initialise caller-provided runtime storage and calculate its first snapshot.
 *
 * @param runtime Writable storage retained by the caller.
 * @return `UMI_STATUS_OK` on success or a validation or capacity status.
 */
UmiStatus umi_productisation_runtime_init(UmiProductisationRuntime *runtime);

/**
 * Rebuild all derived portfolio views from the runtime's current evidence.
 *
 * A successful refresh advances the revision. No external actions are run.
 *
 * @param runtime Runtime whose derived state is refreshed.
 * @return `UMI_STATUS_OK` on success or the first calculation failure.
 */
UmiStatus umi_productisation_runtime_refresh(
    UmiProductisationRuntime *runtime);

/**
 * Record evidence and refresh the dependent coverage, gaps and release gate.
 *
 * Strings are copied into bounded ledger storage. A failed refresh is returned
 * to the caller so the application never mistakes stale state for acceptance.
 *
 * @param runtime Runtime that owns the evidence ledger.
 * @param application_id Stable application identifier.
 * @param asset_kind Kind of asset being evidenced.
 * @param asset_id Stable asset identifier.
 * @param frontend_id Frontend identifier or an empty string for shared proof.
 * @param state Evidence state supported by the reference.
 * @param reference Human-readable test, file or review reference.
 * @return `UMI_STATUS_OK` on success or a validation, capacity or refresh status.
 */
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
