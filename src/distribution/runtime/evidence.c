/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/runtime/evidence.c
 *
 * PURPOSE:
 *   Translate named evidence assertions into the existing distribution release gate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "evidence_internal.h"
#include <stdlib.h>
#include <string.h>

void UmiReleaseContractDestroy(UmiReleaseContract *contract) { free(contract); }
void UmiReleaseEvidenceDestroy(UmiReleaseEvidence *evidence) { free(evidence); }
size_t UmiReleaseContractCount(const UmiReleaseContract *contract)
{ return contract != NULL ? contract->count : 0U; }
const UmiReleaseRequirement *UmiReleaseContractAt(const UmiReleaseContract *contract, size_t index)
{ return contract != NULL && index < contract->count ? &contract->rows[index] : NULL; }
const char *UmiReleaseContractCandidate(const UmiReleaseContract *contract)
{ return contract != NULL ? contract->candidate : ""; }
const char *UmiReleaseContractSource(const UmiReleaseContract *contract)
{ return contract != NULL ? contract->source : ""; }

UmiStatus UmiReleaseEvidenceAssess(const UmiReleaseContract *contract,
    const UmiReleaseEvidence *evidence, UmiReleaseEvidenceAssessment *outAssessment)
{
    UmiReleaseEvidenceAssessment result = {0};
    bool categoryComplete[6] = {true, true, true, true, true, true};
    if (contract == NULL || evidence == NULL || outAssessment == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Typos or surplus unrelated evidence must not be silently ignored. */
    for (size_t i = 0U; i < evidence->count; ++i) {
        bool found = false;
        for (size_t j = 0U; j < contract->count; ++j)
            if (strcmp(evidence->rows[i].id, contract->rows[j].id) == 0) found = true;
        if (!found) return UMI_STATUS_NOT_FOUND;
    }
    result.required = contract->count;
    for (size_t i = 0U; i < contract->count; ++i) {
        const UmiReleaseRequirement *requirement = &contract->rows[i];
        const UmiReleaseObservation *observation = NULL;
        unsigned reason = 0U;
        for (size_t j = 0U; j < evidence->count; ++j)
            if (strcmp(requirement->id, evidence->rows[j].id) == 0) observation = &evidence->rows[j];
        if (observation == NULL) {
            reason = UMI_RELEASE_EVIDENCE_MISSING;
            ++result.missing;
        } else {
            if (strcmp(contract->candidate, observation->candidate) != 0 ||
                strcmp(contract->source, observation->source) != 0 ||
                strcmp(requirement->profile, observation->profile) != 0 ||
                strcmp(requirement->configuration, observation->configuration) != 0 ||
                strcmp(requirement->kind, observation->kind) != 0)
                reason |= UMI_RELEASE_EVIDENCE_CONTEXT;
            if (strcmp(observation->outcome, "passed") != 0)
                reason |= UMI_RELEASE_EVIDENCE_OUTCOME;
            if (observation->total == 0U || observation->passed != observation->total ||
                observation->failed != 0U || observation->skipped != 0U || observation->notRun != 0U)
                reason |= UMI_RELEASE_EVIDENCE_COUNTS;
            if (strcmp(observation->digest, "-") == 0 || strcmp(observation->reference, "-") == 0)
                reason |= UMI_RELEASE_EVIDENCE_REFERENCE;
            if (reason != 0U) ++result.rejected;
        }
        result.reasons[i] = reason;
        if (reason == 0U) ++result.reportedComplete;
        else categoryComplete[(size_t)requirement->category] = false;
    }
    result.distributionInput.signatures_ok = categoryComplete[UMI_RELEASE_SIGNATURE];
    result.distributionInput.checksums_ok = categoryComplete[UMI_RELEASE_CHECKSUM];
    result.distributionInput.compatibility_ok = categoryComplete[UMI_RELEASE_COMPATIBILITY];
    result.distributionInput.tests_ok = categoryComplete[UMI_RELEASE_TESTS];
    result.distributionInput.frontend_conformance_ok = categoryComplete[UMI_RELEASE_FRONTEND];
    result.distributionInput.blockers = result.missing + result.rejected;
    /* Reuse the existing release decision; do not create an application-local
     * policy or treat source presence as a completed native user journey. */
    result.readyForOwnerReview = umi_dr_release_gate_pass(&result.distributionInput);
    *outAssessment = result;
    return UMI_STATUS_OK;
}
