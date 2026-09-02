/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/evidence.h
 *
 * PURPOSE:
 *   Record bounded implementation, adoption, test and acceptance evidence for
 *   the assets declared by Framework application experiences.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EVIDENCE_H
#define UMICOM_APPLICATION_PRODUCTISATION_EVIDENCE_H

#include "umicom/application/productisation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the productisation evidence data shared with callers of this public contract.
 */
typedef struct UmiProductisationEvidence {
    char application_id[UMI_PRODUCTISATION_ID_CAPACITY];
    UmiProductisationAssetKind asset_kind;
    char asset_id[UMI_PRODUCTISATION_ID_CAPACITY];
    char frontend_id[UMI_PRODUCTISATION_ID_CAPACITY];
    UmiProductisationEvidenceState state;
    char reference[UMI_PRODUCTISATION_REFERENCE_CAPACITY];
    uint64_t revision;
} UmiProductisationEvidence;

/**
 * Represent the productisation evidence ledger data shared with callers of this public
 * contract.
 */
typedef struct UmiProductisationEvidenceLedger {
    UmiProductisationEvidence entries[UMI_PRODUCTISATION_MAX_EVIDENCE];
    size_t count;
    uint64_t next_revision;
} UmiProductisationEvidenceLedger;

/**
 * Reset an evidence ledger to an empty, ready-to-use state.
 *
 * @param ledger Ledger storage supplied and retained by the caller.
 */
void umi_productisation_evidence_ledger_init(
    UmiProductisationEvidenceLedger *ledger);

/**
 * Add or update evidence for one application asset and frontend.
 *
 * Identifiers and the reference are copied into bounded ledger storage. The
 * function does not keep the supplied string pointers.
 *
 * @param ledger Ledger that receives the evidence.
 * @param application_id Stable application identifier.
 * @param asset_kind Kind of feature, panel, layout, component or capability.
 * @param asset_id Stable identifier of the recorded asset.
 * @param frontend_id Frontend identifier, or an empty string for shared proof.
 * @param state Highest evidence state supported by `reference`.
 * @param reference Human-readable test, file or review reference.
 * @return `UMI_STATUS_OK` on success, or a validation or capacity status.
 */
UmiStatus umi_productisation_evidence_record(
    UmiProductisationEvidenceLedger *ledger,
    const char *application_id,
    UmiProductisationAssetKind asset_kind,
    const char *asset_id,
    const char *frontend_id,
    UmiProductisationEvidenceState state,
    const char *reference);

/**
 * Find the latest evidence for an exact application asset and frontend.
 *
 * @param ledger Ledger to search.
 * @param application_id Stable application identifier.
 * @param asset_kind Kind of asset to match.
 * @param asset_id Stable asset identifier.
 * @param frontend_id Frontend identifier used when the evidence was recorded.
 * @return Pointer owned by `ledger`, or `NULL` when no entry matches.
 */
const UmiProductisationEvidence *umi_productisation_evidence_find(
    const UmiProductisationEvidenceLedger *ledger,
    const char *application_id,
    UmiProductisationAssetKind asset_kind,
    const char *asset_id,
    const char *frontend_id);

/**
 * Resolve the effective evidence state for an application asset.
 *
 * The recorded state is returned when matching evidence exists. Otherwise the
 * supplied declaration or catalogue state is returned unchanged.
 *
 * @param ledger Ledger to search; may be empty but must be valid.
 * @param application_id Stable application identifier.
 * @param asset_kind Kind of asset to match.
 * @param asset_id Stable asset identifier.
 * @param fallback_state State to use when the ledger has no matching entry.
 * @return Recorded or fallback evidence state.
 */
UmiProductisationEvidenceState umi_productisation_evidence_effective_state(
    const UmiProductisationEvidenceLedger *ledger,
    const char *application_id,
    UmiProductisationAssetKind asset_kind,
    const char *asset_id,
    UmiProductisationEvidenceState fallback_state);

#ifdef __cplusplus
}
#endif

#endif
