/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/release_evidence_v2.c
 *
 * PURPOSE:
 *   Aggregate checksums, signatures, licences, SBOM and provenance evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/delivery/release_evidence_v2.h"
#include <string.h>

void umi_release_evidence_v2_init(UmiReleaseEvidenceV2 *evidence)
{
    if (evidence != NULL) (void)memset(evidence, 0, sizeof(*evidence));
}

UmiStatus umi_release_evidence_v2_set(UmiReleaseEvidenceV2 *evidence,
                                      const char *check_id,
                                      UmiEvidenceStatus status)
{
    UmiEvidenceStatus *destination = NULL;
    if (evidence == NULL || check_id == NULL ||
        status < UMI_EVIDENCE_UNKNOWN || status > UMI_EVIDENCE_SKIP) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(check_id, "tests") == 0) destination = &evidence->tests;
    else if (strcmp(check_id, "checksums") == 0) destination = &evidence->checksums;
    else if (strcmp(check_id, "signatures") == 0) destination = &evidence->signatures;
    else if (strcmp(check_id, "licences") == 0) destination = &evidence->licence_inventory;
    else if (strcmp(check_id, "sbom") == 0) destination = &evidence->sbom;
    else if (strcmp(check_id, "provenance") == 0) destination = &evidence->provenance;
    else return UMI_STATUS_NOT_FOUND;
    *destination = status;
    return UMI_STATUS_OK;
}

size_t umi_release_evidence_v2_blocker_count(const UmiReleaseEvidenceV2 *evidence,
                                              UmiReleaseChannel channel)
{
    size_t blockers = 0U;
    if (evidence == NULL) return 6U;
    if (evidence->tests != UMI_EVIDENCE_PASS) ++blockers;
    if (evidence->checksums != UMI_EVIDENCE_PASS) ++blockers;
    if (evidence->licence_inventory != UMI_EVIDENCE_PASS) ++blockers;
    if (evidence->sbom != UMI_EVIDENCE_PASS) ++blockers;
    if (evidence->provenance != UMI_EVIDENCE_PASS) ++blockers;
    if (channel == UMI_RELEASE_STABLE) {
        if (evidence->signatures != UMI_EVIDENCE_PASS) ++blockers;
    } else if (evidence->signatures == UMI_EVIDENCE_FAIL) {
        ++blockers;
    }
    return blockers;
}

int umi_release_evidence_v2_ready(const UmiReleaseEvidenceV2 *evidence,
                                  UmiReleaseChannel channel)
{
    return umi_release_evidence_v2_blocker_count(evidence, channel) == 0U;
}
