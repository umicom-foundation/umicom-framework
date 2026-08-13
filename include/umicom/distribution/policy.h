/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/policy.h
 *
 * PURPOSE:
 *   Evaluate channel, trust, compatibility, evidence and version policy before
 *   an install or update transaction can be planned.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Policy evaluation is a pure decision.  A successful decision still does not
 * alter installed files; it only authorises creation of a visible plan.
 */
#ifndef UMICOM_DISTRIBUTION_POLICY_H
#define UMICOM_DISTRIBUTION_POLICY_H

#include "umicom/distribution/evidence.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDistributionPolicy {
    UmiReleaseChannel channel;
    int automatic_updates;
    int security_only;
    int allow_prerelease;
    int allow_downgrade;
    int require_trusted;
    int require_signature;
    int require_sbom;
    int require_provenance;
} UmiDistributionPolicy;

typedef struct UmiDistributionDecision {
    int eligible;
    int installation_required;
    int update_available;
    int downgrade;
    int requires_restart;
    UmiVersion installed_version;
    UmiVersion candidate_version;
    char reason[256];
} UmiDistributionDecision;

UmiDistributionPolicy umi_distribution_policy_default(void);
UmiStatus umi_distribution_policy_evaluate(
    const UmiDistributionPolicy *policy,
    int installed,
    UmiVersion installed_version,
    const UmiDistributionPackage *candidate,
    const UmiDistributionEvidence *evidence,
    UmiDistributionDecision *out_decision);

#ifdef __cplusplus
}
#endif

#endif
