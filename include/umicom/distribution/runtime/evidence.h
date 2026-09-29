/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/runtime/evidence.h
 *
 * PURPOSE:
 *   Assess explicit release evidence without manufacturing a product release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_RUNTIME_EVIDENCE_H
#define UMICOM_DISTRIBUTION_RUNTIME_EVIDENCE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/distribution/runtime/release_gate.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_RELEASE_EVIDENCE_LIMIT 128U
#define UMI_RELEASE_TEXT_LIMIT 524288U
#define UMI_RELEASE_TOKEN_CAPACITY 64U
#define UMI_RELEASE_TITLE_CAPACITY 256U

typedef struct UmiReleaseContract UmiReleaseContract;
typedef struct UmiReleaseEvidence UmiReleaseEvidence;
typedef enum UmiReleaseEvidenceCategory {
    UMI_RELEASE_SIGNATURE, UMI_RELEASE_CHECKSUM, UMI_RELEASE_COMPATIBILITY,
    UMI_RELEASE_TESTS, UMI_RELEASE_FRONTEND, UMI_RELEASE_OTHER
} UmiReleaseEvidenceCategory;
typedef struct UmiReleaseRequirement {
    char id[UMI_RELEASE_TOKEN_CAPACITY];
    UmiReleaseEvidenceCategory category;
    char profile[UMI_RELEASE_TOKEN_CAPACITY];
    char configuration[UMI_RELEASE_TOKEN_CAPACITY];
    char kind[UMI_RELEASE_TOKEN_CAPACITY];
    char ownerBatch[UMI_RELEASE_TOKEN_CAPACITY];
    char title[UMI_RELEASE_TITLE_CAPACITY];
} UmiReleaseRequirement;
typedef enum UmiReleaseEvidenceReason {
    UMI_RELEASE_EVIDENCE_COMPLETE = 0U,
    UMI_RELEASE_EVIDENCE_MISSING = 1U,
    UMI_RELEASE_EVIDENCE_CONTEXT = 2U,
    UMI_RELEASE_EVIDENCE_OUTCOME = 4U,
    UMI_RELEASE_EVIDENCE_COUNTS = 8U,
    UMI_RELEASE_EVIDENCE_REFERENCE = 16U
} UmiReleaseEvidenceReason;
typedef struct UmiReleaseEvidenceAssessment {
    size_t required;
    size_t reportedComplete;
    size_t missing;
    size_t rejected;
    unsigned reasons[UMI_RELEASE_EVIDENCE_LIMIT];
    UmiDrReleaseGateInput distributionInput;
    bool readyForOwnerReview;
} UmiReleaseEvidenceAssessment;

/* Parse bounded ASCII TSV, not executable instructions. Text is copied; it may
 * be freed after success. *out must be NULL. Failure leaves *out unchanged.
 * Unknown fields, duplicate IDs, embedded NUL and overflow are errors.
 * Contracts must contain each of the existing distribution gate's five kinds.
 * This evidence catalogue is NOT the runtime capability/provider registry. */
UmiStatus UmiReleaseContractParse(const char *text, size_t length,
    UmiReleaseContract **outContract);
UmiStatus UmiReleaseEvidenceParse(const char *text, size_t length,
    UmiReleaseEvidence **outEvidence);
void UmiReleaseContractDestroy(UmiReleaseContract *contract);
void UmiReleaseEvidenceDestroy(UmiReleaseEvidence *evidence);
size_t UmiReleaseContractCount(const UmiReleaseContract *contract);
const UmiReleaseRequirement *UmiReleaseContractAt(
    const UmiReleaseContract *contract, size_t index);
const char *UmiReleaseContractCandidate(const UmiReleaseContract *contract);
const char *UmiReleaseContractSource(const UmiReleaseContract *contract);
/* Immutable inputs support concurrent readers with independently owned outputs.
 * Destroy must not race readers. Unknown evidence IDs are input errors. Every
 * declared requirement is mandatory; no percentage, optional flag or waiver can
 * quietly remove one. Missing/skipped/not-run/failed or mismatched evidence
 * blocks owner review. The existing distribution gate makes the final boolean.
 *
 * IMPORTANT: records are caller-supplied assertions. This function does not
 * authenticate a signer, read logs, check digest bytes, execute tests, compare
 * test inventories, inspect a Git checkout or authorise publishing. A complete
 * report is a request for human review, NEVER a certified stable release.
 * Artefact digest is a reference to inspect separately, not proof of origin.
 * On error, the complete previous assessment remains untouched. */
UmiStatus UmiReleaseEvidenceAssess(const UmiReleaseContract *contract,
    const UmiReleaseEvidence *evidence, UmiReleaseEvidenceAssessment *outAssessment);
#ifdef __cplusplus
}
#endif
#endif
