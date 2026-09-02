/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/service.h
 *
 * PURPOSE:
 *   Coordinate the existing Product Centre and Delivery contracts with release
 *   repositories, verification evidence, transaction history and notifications.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/

/*
 * This service is the reusable distribution boundary.  Studio supplies product
 * identity and UI composition; Framework owns every generic decision and model.
 */
#ifndef UMICOM_DISTRIBUTION_SERVICE_H
#define UMICOM_DISTRIBUTION_SERVICE_H

#include "umicom/distribution/notification.h"
#include "umicom/distribution/repository.h"
#include "umicom/product/centre.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the distribution service data shared with callers of this public contract.
 */
typedef struct UmiDistributionService UmiDistributionService;

/**
 * Represent the distribution service snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDistributionServiceSnapshot {
    uint64_t revision;
    size_t marketplace_releases;
    size_t products;
    size_t installations;
    size_t policies;
    size_t evidence_records;
    size_t transactions;
    size_t notifications;
    size_t unread_notifications;
    size_t available_updates;
} UmiDistributionServiceSnapshot;

/**
 * Initialise distribution service from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_distribution_service_create(
    UmiProductCentre *product_centre,
    UmiDistributionService **out_service);
/**
 * Release or reset state held by distribution service so the same storage can be reused
 * safely.
 */
void umi_distribution_service_destroy(UmiDistributionService *service);
/**
 * Provide the distribution service products operation used by this module and its client
 * applications.
 */
UmiProductCentre *umi_distribution_service_products(UmiDistributionService *service);
/**
 * Provide the distribution service repository operation used by this module and its client
 * applications.
 */
UmiDistributionRepository *umi_distribution_service_repository(
    UmiDistributionService *service);
/**
 * Provide the distribution service evidence operation used by this module and its client
 * applications.
 */
UmiDistributionEvidenceRegistry *umi_distribution_service_evidence(
    UmiDistributionService *service);
/**
 * Provide the distribution service transactions operation used by this module and its
 * client applications.
 */
UmiDistributionTransactionLog *umi_distribution_service_transactions(
    UmiDistributionService *service);
/**
 * Provide the distribution service notifications operation used by this module and its
 * client applications.
 */
UmiDistributionNotificationCentre *umi_distribution_service_notifications(
    UmiDistributionService *service);
/**
 * Provide the distribution service publish operation used by this module and its client
 * applications.
 */
UmiStatus umi_distribution_service_publish(
    UmiDistributionService *service,
    const UmiDistributionPackage *package);
/**
 * Provide the distribution service record installation operation used by this module and
 * its client applications.
 */
UmiStatus umi_distribution_service_record_installation(
    UmiDistributionService *service,
    const char *product_id,
    UmiVersion version,
    const char *install_root,
    UmiReleaseChannel channel,
    uint64_t installed_at_ms);
/**
 * Provide the distribution service set policy operation used by this module and its client
 * applications.
 */
UmiStatus umi_distribution_service_set_policy(
    UmiDistributionService *service,
    const char *product_id,
    const UmiDistributionPolicy *policy);
/**
 * Provide the distribution service scan updates operation used by this module and its
 * client applications.
 */
UmiStatus umi_distribution_service_scan_updates(
    UmiDistributionService *service,
    uint32_t framework_abi,
    uint64_t timestamp_ms,
    size_t *out_available_updates);
/**
 * Release or reset state held by distribution service plan so the same storage can be
 * reused safely.
 */
UmiStatus umi_distribution_service_plan_release(
    UmiDistributionService *service,
    const char *transaction_id,
    const char *release_id,
    const char *install_root,
    uint64_t timestamp_ms,
    UmiDistributionDecision *out_decision,
    UmiDistributionTransaction *out_transaction);
/**
 * Provide the distribution service snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_distribution_service_snapshot(
    const UmiDistributionService *service,
    UmiDistributionServiceSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif
