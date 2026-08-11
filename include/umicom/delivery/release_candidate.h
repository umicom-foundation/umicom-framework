/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/release_candidate.h
 *
 * PURPOSE:
 *   Combine build evidence, artifacts, verification and policy into one release-candidate record.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A candidate is the object reviewed before publishing; it is separate from a release that has already been promoted.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_RELEASE_CANDIDATE_H
#define INCLUDE_UMICOM_DELIVERY_RELEASE_CANDIDATE_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"
#include "umicom/delivery/artifact_set.h"
#include "umicom/delivery/build_evidence.h"
#include "umicom/delivery/manifest.h"
#include "umicom/delivery/verification.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReleaseCandidate {
    UmiDeliveryManifest manifest;
    UmiBuildEvidence build;
    UmiArtifactSet artifacts;
    UmiVerificationReport verification;
} UmiReleaseCandidate;

void umi_release_candidate_init(UmiReleaseCandidate *candidate);
int umi_release_candidate_ready(const UmiReleaseCandidate *candidate);

#ifdef __cplusplus
}
#endif

#endif
