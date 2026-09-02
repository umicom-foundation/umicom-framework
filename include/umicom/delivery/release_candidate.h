/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/release_candidate.h
 *
 * PURPOSE:
 *   Combine build evidence, artifacts, verification and policy into one release-candidate record.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/**
 * Represent the release candidate data shared with callers of this public contract.
 */
typedef struct UmiReleaseCandidate {
    UmiDeliveryManifest manifest;
    UmiBuildEvidence build;
    UmiArtifactSet artifacts;
    UmiVerificationReport verification;
} UmiReleaseCandidate;

/**
 * Initialise release candidate from caller-provided values so later operations receive a
 * known state.
 */
void umi_release_candidate_init(UmiReleaseCandidate *candidate);
/**
 * Provide the release candidate ready operation used by this module and its client
 * applications.
 */
int umi_release_candidate_ready(const UmiReleaseCandidate *candidate);

#ifdef __cplusplus
}
#endif

#endif
