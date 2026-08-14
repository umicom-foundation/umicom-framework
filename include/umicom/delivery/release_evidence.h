/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/release_evidence.h
 *
 * PURPOSE:
 *   Aggregate checksums, signatures, licences, SBOM and provenance evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_RELEASE_EVIDENCE_H
#define UMICOM_DELIVERY_RELEASE_EVIDENCE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReleaseEvidence {
    UmiEvidenceStatus tests;
    UmiEvidenceStatus checksums;
    UmiEvidenceStatus signatures;
    UmiEvidenceStatus licence_inventory;
    UmiEvidenceStatus sbom;
    UmiEvidenceStatus provenance;
} UmiReleaseEvidence;

void umi_release_evidence_init(UmiReleaseEvidence *evidence);
UmiStatus umi_release_evidence_set(UmiReleaseEvidence *evidence,
                                      const char *check_id,
                                      UmiEvidenceStatus status);
size_t umi_release_evidence_blocker_count(const UmiReleaseEvidence *evidence,
                                              UmiReleaseChannel channel);
int umi_release_evidence_ready(const UmiReleaseEvidence *evidence,
                                  UmiReleaseChannel channel);

#ifdef __cplusplus
}
#endif
#endif
