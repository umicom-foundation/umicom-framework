/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_distribution_evidence_policy.c
 *
 * PURPOSE:
 *   Verify supply-chain evidence gates and update-channel policy decisions.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "distribution_fixture.h"

int main(void)
{
    UmiDistributionPackage package = distribution_test_package(
        "studio-0.17.0", "org.umicom.studio", (UmiVersion){0U, 17U, 0U},
        UMI_RELEASE_STABLE);
    UmiDistributionEvidence evidence = distribution_test_evidence(package.release_id);
    UmiDistributionEvidenceRegistry *registry = NULL;
    UmiDistributionPolicy policy = umi_distribution_policy_default();
    UmiDistributionDecision decision;
    char reason[256];
    assert(umi_distribution_evidence_registry_create(&registry) == UMI_STATUS_OK);
    assert(umi_distribution_evidence_registry_upsert(registry, &evidence) ==
           UMI_STATUS_OK);
    assert(umi_distribution_evidence_validate(
        &evidence, 1, 1, 1, reason, sizeof(reason)) == UMI_STATUS_OK);
    assert(umi_distribution_policy_evaluate(
        &policy, 1, (UmiVersion){0U, 16U, 0U}, &package, &evidence,
        &decision) == UMI_STATUS_OK);
    assert(decision.eligible);
    assert(decision.update_available);
    evidence.signature_verified = 0;
    assert(umi_distribution_policy_evaluate(
        &policy, 1, (UmiVersion){0U, 16U, 0U}, &package, &evidence,
        &decision) == UMI_STATUS_OK);
    assert(!decision.eligible);
    assert(strstr(decision.reason, "signature") != NULL);
    evidence.signature_verified = 1;
    package.version = (UmiVersion){0U, 15U, 0U};
    policy.allow_downgrade = 1;
    assert(umi_distribution_policy_evaluate(
        &policy, 1, (UmiVersion){0U, 16U, 0U}, &package, &evidence,
        &decision) == UMI_STATUS_OK);
    assert(decision.eligible);
    assert(decision.downgrade);
    umi_distribution_evidence_registry_destroy(registry);
    return 0;
}
