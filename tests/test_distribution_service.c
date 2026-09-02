/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_distribution_service.c
 *
 * PURPOSE:
 *   Verify the reusable Product/Marketplace/Update service from publication
 *   through notification and recoverable transaction planning.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>

#include "distribution_fixture.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDistributionService *service = NULL;
    UmiDistributionPackage package = distribution_test_package(
        "studio-0.17.0", "org.umicom.studio", (UmiVersion){0U, 17U, 0U},
        UMI_RELEASE_STABLE);
    UmiDistributionEvidence evidence = distribution_test_evidence(package.release_id);
    UmiDistributionPolicy policy = umi_distribution_policy_default();
    UmiDistributionServiceSnapshot snapshot;
    UmiDistributionDecision decision;
    UmiDistributionTransaction transaction;
    size_t updates = 0U;
    assert(umi_distribution_service_create(NULL, &service) == UMI_STATUS_OK);
    assert(umi_distribution_service_publish(service, &package) == UMI_STATUS_OK);
    assert(umi_distribution_evidence_registry_upsert(
        umi_distribution_service_evidence(service), &evidence) == UMI_STATUS_OK);
    assert(umi_distribution_service_record_installation(
        service, package.product_id, (UmiVersion){0U, 16U, 0U},
        "C:/Program Files/Umicom Studio", UMI_RELEASE_STABLE, 50U) ==
        UMI_STATUS_OK);
    assert(umi_distribution_service_set_policy(
        service, package.product_id, &policy) == UMI_STATUS_OK);
    assert(umi_distribution_service_scan_updates(
        service, UMICOM_FRAMEWORK_ABI_VERSION, 100U, &updates) == UMI_STATUS_OK);
    assert(updates == 1U);
    assert(umi_distribution_service_plan_release(
        service, "txn-studio-17", package.release_id, "", 200U,
        &decision, &transaction) == UMI_STATUS_OK);
    assert(transaction.action == UMI_DISTRIBUTION_UPDATE);
    assert(umi_distribution_service_snapshot(service, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.products == 1U);
    assert(snapshot.available_updates == 1U);
    assert(snapshot.transactions == 1U);
    assert(snapshot.notifications == 2U);
    umi_distribution_service_destroy(service);
    return 0;
}
