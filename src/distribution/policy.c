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

/* Provide the channel allowed operation used by this module and its client applications. */
static int channel_allowed(UmiReleaseChannel candidate,
                           const UmiDistributionPolicy *policy)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (!policy->allow_prerelease && candidate != UMI_RELEASE_STABLE) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->channel == UMI_RELEASE_STABLE) return candidate == UMI_RELEASE_STABLE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->channel == UMI_RELEASE_BETA) {
        return candidate == UMI_RELEASE_BETA || candidate == UMI_RELEASE_STABLE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->channel == UMI_RELEASE_NIGHTLY) {
        return candidate == UMI_RELEASE_NIGHTLY || candidate == UMI_RELEASE_BETA ||
               candidate == UMI_RELEASE_STABLE;
    }
    return 1;
}

/* Provide the decide operation used by this module and its client applications. */
static void decide(UmiDistributionDecision *decision,
                   const char *reason,
                   int eligible)
{
    decision->eligible = eligible;
    (void)snprintf(decision->reason, sizeof(decision->reason), "%s", reason);
}

/*
 * Provide the distribution policy default operation used by this module and its client
 * applications.
 */
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

/*
 * Provide the distribution policy evaluate operation used by this module and its client
 * applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || candidate == NULL || evidence == NULL ||
        out_decision == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_decision, 0, sizeof(*out_decision));
    out_decision->installed_version = installed_version;
    out_decision->candidate_version = candidate->version;
    out_decision->requires_restart =
        candidate->kind == UMI_DISTRIBUTION_APPLICATION ||
        candidate->kind == UMI_DISTRIBUTION_RUNTIME;
    /* Apply this branch only when its contract condition is satisfied. */
    if (candidate->deprecated) {
        decide(out_decision, "candidate release is deprecated", 0);
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->require_trusted && !candidate->trusted) {
        decide(out_decision, "candidate provider is not trusted", 0);
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!candidate->compatible) {
        decide(out_decision, "candidate is incompatible with this runtime", 0);
        return UMI_STATUS_OK;
    }
    /* Apply this operation only while the related capability or state is available. */
    if (!channel_allowed(candidate->channel, policy)) {
        decide(out_decision, "candidate is outside the selected release channel", 0);
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->security_only && !candidate->security_update) {
        decide(out_decision, "policy permits security updates only", 0);
        return UMI_STATUS_OK;
    }
    evidence_status = umi_distribution_evidence_validate(
        evidence, policy->require_signature, policy->require_sbom,
        policy->require_provenance, evidence_reason, sizeof(evidence_reason));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (evidence_status != UMI_STATUS_OK) {
        decide(out_decision, evidence_reason, 0);
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!installed) {
        out_decision->installation_required = 1;
        decide(out_decision, "verified package is eligible for installation", 1);
        return UMI_STATUS_OK;
    }
    comparison = umi_distribution_version_compare(candidate->version, installed_version);
    /* Apply this branch only when its contract condition is satisfied. */
    if (comparison == 0) {
        decide(out_decision, "selected release is already installed", 0);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (comparison > 0) {
        out_decision->update_available = 1;
        decide(out_decision, "verified package update is available", 1);
    } else /* Apply this branch only when its contract condition is satisfied. */ if (!policy->allow_downgrade) {
        decide(out_decision, "downgrade is blocked by update policy", 0);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        out_decision->downgrade = 1;
        decide(out_decision, "verified downgrade is explicitly permitted", 1);
    }
    return UMI_STATUS_OK;
}
