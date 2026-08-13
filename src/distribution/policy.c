/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/policy.c
 *
 * PURPOSE:
 *   Implement pure distribution-policy decisions before transaction planning.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/distribution/policy.h"

#include <stdio.h>
#include <string.h>

static int channel_allowed(UmiReleaseChannel candidate,
                           const UmiDistributionPolicy *policy)
{
    if (!policy->allow_prerelease && candidate != UMI_RELEASE_STABLE) return 0;
    if (policy->channel == UMI_RELEASE_STABLE) return candidate == UMI_RELEASE_STABLE;
    if (policy->channel == UMI_RELEASE_BETA) {
        return candidate == UMI_RELEASE_BETA || candidate == UMI_RELEASE_STABLE;
    }
    if (policy->channel == UMI_RELEASE_NIGHTLY) {
        return candidate == UMI_RELEASE_NIGHTLY || candidate == UMI_RELEASE_BETA ||
               candidate == UMI_RELEASE_STABLE;
    }
    return 1;
}

static void decide(UmiDistributionDecision *decision,
                   const char *reason,
                   int eligible)
{
    decision->eligible = eligible;
    (void)snprintf(decision->reason, sizeof(decision->reason), "%s", reason);
}

UmiDistributionPolicy umi_distribution_policy_default(void)
{
    UmiDistributionPolicy policy;
    (void)memset(&policy, 0, sizeof(policy));
    policy.channel = UMI_RELEASE_STABLE;
    policy.require_trusted = 1;
    policy.require_signature = 1;
    policy.require_sbom = 1;
    policy.require_provenance = 1;
    return policy;
}

UmiStatus umi_distribution_policy_evaluate(
    const UmiDistributionPolicy *policy,
    int installed,
    UmiVersion installed_version,
    const UmiDistributionPackage *candidate,
    const UmiDistributionEvidence *evidence,
    UmiDistributionDecision *out_decision)
{
    char evidence_reason[256];
    int comparison;
    UmiStatus evidence_status;
    if (policy == NULL || candidate == NULL || evidence == NULL ||
        out_decision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_decision, 0, sizeof(*out_decision));
    out_decision->installed_version = installed_version;
    out_decision->candidate_version = candidate->version;
    out_decision->requires_restart =
        candidate->kind == UMI_DISTRIBUTION_APPLICATION ||
        candidate->kind == UMI_DISTRIBUTION_RUNTIME;
    if (candidate->deprecated) {
        decide(out_decision, "candidate release is deprecated", 0);
        return UMI_STATUS_OK;
    }
    if (policy->require_trusted && !candidate->trusted) {
        decide(out_decision, "candidate provider is not trusted", 0);
        return UMI_STATUS_OK;
    }
    if (!candidate->compatible) {
        decide(out_decision, "candidate is incompatible with this runtime", 0);
        return UMI_STATUS_OK;
    }
    if (!channel_allowed(candidate->channel, policy)) {
        decide(out_decision, "candidate is outside the selected release channel", 0);
        return UMI_STATUS_OK;
    }
    if (policy->security_only && !candidate->security_update) {
        decide(out_decision, "policy permits security updates only", 0);
        return UMI_STATUS_OK;
    }
    evidence_status = umi_distribution_evidence_validate(
        evidence, policy->require_signature, policy->require_sbom,
        policy->require_provenance, evidence_reason, sizeof(evidence_reason));
    if (evidence_status != UMI_STATUS_OK) {
        decide(out_decision, evidence_reason, 0);
        return UMI_STATUS_OK;
    }
    if (!installed) {
        out_decision->installation_required = 1;
        decide(out_decision, "verified package is eligible for installation", 1);
        return UMI_STATUS_OK;
    }
    comparison = umi_distribution_version_compare(candidate->version, installed_version);
    if (comparison == 0) {
        decide(out_decision, "selected release is already installed", 0);
    } else if (comparison > 0) {
        out_decision->update_available = 1;
        decide(out_decision, "verified package update is available", 1);
    } else if (!policy->allow_downgrade) {
        decide(out_decision, "downgrade is blocked by update policy", 0);
    } else {
        out_decision->downgrade = 1;
        decide(out_decision, "verified downgrade is explicitly permitted", 1);
    }
    return UMI_STATUS_OK;
}
