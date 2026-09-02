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

/*
 * Policy evaluation is a pure decision.  A successful decision still does not
 * alter installed files; it only authorises creation of a visible plan.
 */
#ifndef UMICOM_DISTRIBUTION_POLICY_H
#define UMICOM_DISTRIBUTION_POLICY_H

#include "umicom/distribution/evidence.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the distribution policy data shared with callers of this public contract.
 */
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

/**
 * Represent the distribution decision data shared with callers of this public contract.
 */
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

/**
 * Provide the distribution policy default operation used by this module and its client
 * applications.
 */
UmiDistributionPolicy umi_distribution_policy_default(void);
/**
 * Provide the distribution policy evaluate operation used by this module and its client
 * applications.
 */
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
