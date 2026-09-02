/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/release_evidence.h
 *
 * PURPOSE:
 *   Aggregate checksums, signatures, licences, SBOM and provenance evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DELIVERY_RELEASE_EVIDENCE_H
#define UMICOM_DELIVERY_RELEASE_EVIDENCE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the release evidence data shared with callers of this public contract.
 */
typedef struct UmiReleaseEvidence {
    UmiEvidenceStatus tests;
    UmiEvidenceStatus checksums;
    UmiEvidenceStatus signatures;
    UmiEvidenceStatus licence_inventory;
    UmiEvidenceStatus sbom;
    UmiEvidenceStatus provenance;
} UmiReleaseEvidence;

/**
 * Initialise release evidence from caller-provided values so later operations receive a
 * known state.
 */
void umi_release_evidence_init(UmiReleaseEvidence *evidence);
/**
 * Copy release evidence into module-owned storage so callers keep ownership of their input
 * values.
 */
UmiStatus umi_release_evidence_set(UmiReleaseEvidence *evidence,
                                      const char *check_id,
                                      UmiEvidenceStatus status);
/**
 * Return the number of records represented by release evidence blocker without changing
 * their state.
 */
size_t umi_release_evidence_blocker_count(const UmiReleaseEvidence *evidence,
                                              UmiReleaseChannel channel);
/**
 * Provide the release evidence ready operation used by this module and its client
 * applications.
 */
int umi_release_evidence_ready(const UmiReleaseEvidence *evidence,
                                  UmiReleaseChannel channel);

#ifdef __cplusplus
}
#endif
#endif
