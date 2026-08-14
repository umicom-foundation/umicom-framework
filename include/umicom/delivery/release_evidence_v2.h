/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/release_evidence_v2.h
 *
 * PURPOSE:
 *   Aggregate checksums, signatures, licences, SBOM and provenance evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_RELEASE_EVIDENCE_V2_H
#define UMICOM_DELIVERY_RELEASE_EVIDENCE_V2_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReleaseEvidenceV2 {
    UmiEvidenceStatus tests;
    UmiEvidenceStatus checksums;
    UmiEvidenceStatus signatures;
    UmiEvidenceStatus licence_inventory;
    UmiEvidenceStatus sbom;
    UmiEvidenceStatus provenance;
} UmiReleaseEvidenceV2;

void umi_release_evidence_v2_init(UmiReleaseEvidenceV2 *evidence);
UmiStatus umi_release_evidence_v2_set(UmiReleaseEvidenceV2 *evidence,
                                      const char *check_id,
                                      UmiEvidenceStatus status);
size_t umi_release_evidence_v2_blocker_count(const UmiReleaseEvidenceV2 *evidence,
                                              UmiReleaseChannel channel);
int umi_release_evidence_v2_ready(const UmiReleaseEvidenceV2 *evidence,
                                  UmiReleaseChannel channel);

#ifdef __cplusplus
}
#endif
#endif
