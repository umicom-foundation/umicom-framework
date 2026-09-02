/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/service.c
 *
 * PURPOSE:
 *   Coordinate Product Centre registries with distribution repositories,
 *   release evidence, update decisions, transactions and notifications.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/distribution/service.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UMI_DISTRIBUTION_POLICY_CAPACITY 256U

typedef struct UmiDistributionPolicyEntry {
    char product_id[UMI_DISTRIBUTION_ID_CAPACITY];
    UmiDistributionPolicy policy;
} UmiDistributionPolicyEntry;

struct UmiDistributionService {
    UmiProductCentre *products;
    UmiDistributionRepository *repository;
    UmiDistributionEvidenceRegistry *evidence;
    UmiDistributionTransactionLog *transactions;
    UmiDistributionNotificationCentre *notifications;
    UmiDistributionPolicyEntry policies[UMI_DISTRIBUTION_POLICY_CAPACITY];
    size_t policy_count;
    size_t available_updates;
    uint64_t revision;
    int owns_products;
};

/* Provide the format version operation used by this module and its client applications. */
static void format_version(UmiVersion version, char *buffer, size_t capacity)
{
    (void)snprintf(buffer, capacity, "%u.%u.%u",
                   (unsigned int)version.major,
                   (unsigned int)version.minor,
                   (unsigned int)version.patch);
}

/* Provide the parse version operation used by this module and its client applications. */
static UmiVersion parse_version(const char *text)
{
    UmiVersion version = {0U, 0U, 0U};
    unsigned int major = 0U;
    unsigned int minor = 0U;
    unsigned int patch = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text != NULL && sscanf(text, "%u.%u.%u", &major, &minor, &patch) == 3 &&
        major <= UINT16_MAX && minor <= UINT16_MAX && patch <= UINT16_MAX) {
        version.major = (uint16_t)major;
        version.minor = (uint16_t)minor;
        version.patch = (uint16_t)patch;
    }
    return version;
}

/* Provide the find policy operation used by this module and its client applications. */
static UmiDistributionPolicy *find_policy(UmiDistributionService *service,
                                          const char *product_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < service->policy_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(service->policies[index].product_id, product_id) == 0) {
            return &service->policies[index].policy;
        }
    }
    return NULL;
}

/*
 * Initialise distribution service from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_distribution_service_create(
    UmiProductCentre *product_centre,
    UmiDistributionService **out_service)
{
    UmiDistributionService *service;
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiDistributionService *)calloc(1U, sizeof(*service));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    service->products = product_centre;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service->products == NULL) {
        status = umi_product_centre_create(&service->products);
        service->owns_products = status == UMI_STATUS_OK;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_distribution_repository_create(
            UMI_DISTRIBUTION_PACKAGE_CAPACITY, &service->repository);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_distribution_evidence_registry_create(&service->evidence);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_distribution_transaction_log_create(&service->transactions);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_distribution_notification_centre_create(&service->notifications);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_distribution_service_destroy(service);
        return status;
    }
    service->revision = 1U;
    *out_service = service;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by distribution service so the same storage can be reused
 * safely.
 */
void umi_distribution_service_destroy(UmiDistributionService *service)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL) return;
    umi_distribution_notification_centre_destroy(service->notifications);
    umi_distribution_transaction_log_destroy(service->transactions);
    umi_distribution_evidence_registry_destroy(service->evidence);
    umi_distribution_repository_destroy(service->repository);
    /* Apply this branch only when its contract condition is satisfied. */
    if (service->owns_products) umi_product_centre_destroy(service->products);
    free(service);
}

/*
 * Provide the distribution service products operation used by this module and its client
 * applications.
 */
UmiProductCentre *umi_distribution_service_products(UmiDistributionService *service)
{
    return service != NULL ? service->products : NULL;
}

/*
 * Provide the distribution service repository operation used by this module and its client
 * applications.
 */
UmiDistributionRepository *umi_distribution_service_repository(
    UmiDistributionService *service)
{
    return service != NULL ? service->repository : NULL;
}

/*
 * Provide the distribution service evidence operation used by this module and its client
 * applications.
 */
UmiDistributionEvidenceRegistry *umi_distribution_service_evidence(
    UmiDistributionService *service)
{
    return service != NULL ? service->evidence : NULL;
}

/*
 * Provide the distribution service transactions operation used by this module and its
 * client applications.
 */
UmiDistributionTransactionLog *umi_distribution_service_transactions(
    UmiDistributionService *service)
{
    return service != NULL ? service->transactions : NULL;
}

/*
 * Provide the distribution service notifications operation used by this module and its
 * client applications.
 */
UmiDistributionNotificationCentre *umi_distribution_service_notifications(
    UmiDistributionService *service)
{
    return service != NULL ? service->notifications : NULL;
}

/*
 * Provide the distribution service publish operation used by this module and its client
 * applications.
 */
UmiStatus umi_distribution_service_publish(
    UmiDistributionService *service,
    const UmiDistributionPackage *package)
{
    UmiProductMarketplaceItemSnapshot item;
    UmiProductInstallationSnapshot installation;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || package == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_distribution_repository_upsert(service->repository, package);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&item, 0, sizeof(item));
    (void)snprintf(item.id, sizeof(item.id), "%s", package->release_id);
    (void)snprintf(item.provider_id, sizeof(item.provider_id), "%s",
                   package->provider_id);
    (void)snprintf(item.name, sizeof(item.name), "%s", package->display_name);
    (void)snprintf(item.summary, sizeof(item.summary), "%s", package->summary);
    format_version(package->version, item.version, sizeof(item.version));
    (void)snprintf(item.category, sizeof(item.category), "%s", package->category);
    (void)snprintf(item.licence, sizeof(item.licence), "%s", package->licence_id);
    item.trusted = package->trusted;
    item.compatible = package->compatible;
    item.rank = package->security_update ? 1000 : 100;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_product_installation_state_registry_find(
            umi_product_centre_installations(service->products),
            package->product_id, &installation) == UMI_STATUS_OK) {
        item.installed = 1;
        item.update_available = umi_distribution_version_compare(
            package->version, parse_version(installation.version)) > 0;
    }
    status = umi_product_marketplace_registry_upsert(
        umi_product_centre_marketplace(service->products), &item);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

/*
 * Provide the distribution service record installation operation used by this module and
 * its client applications.
 */
UmiStatus umi_distribution_service_record_installation(
    UmiDistributionService *service,
    const char *product_id,
    UmiVersion version,
    const char *install_root,
    UmiReleaseChannel channel,
    uint64_t installed_at_ms)
{
    UmiProductInstallationSnapshot installation;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || product_id == NULL || install_root == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&installation, 0, sizeof(installation));
    (void)snprintf(installation.id, sizeof(installation.id), "%s", product_id);
    (void)snprintf(installation.product_id, sizeof(installation.product_id), "%s",
                   product_id);
    format_version(version, installation.version, sizeof(installation.version));
    (void)snprintf(installation.install_root, sizeof(installation.install_root),
                   "%s", install_root);
    (void)snprintf(installation.channel, sizeof(installation.channel), "%s",
                   umi_release_channel_text(channel));
    installation.installed_at = installed_at_ms;
    installation.state = 1;
    installation.verified = 1;
    installation.rollback_available = 1;
    status = umi_product_installation_state_registry_upsert(
        umi_product_centre_installations(service->products), &installation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

/*
 * Provide the distribution service set policy operation used by this module and its client
 * applications.
 */
UmiStatus umi_distribution_service_set_policy(
    UmiDistributionService *service,
    const char *product_id,
    const UmiDistributionPolicy *policy)
{
    UmiDistributionPolicy *existing;
    UmiProductUpdatePolicySnapshot product_policy;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || product_id == NULL || policy == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    existing = find_policy(service, product_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (existing == NULL) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (service->policy_count >= UMI_DISTRIBUTION_POLICY_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        existing = &service->policies[service->policy_count].policy;
        (void)snprintf(service->policies[service->policy_count].product_id,
                       sizeof(service->policies[0].product_id), "%s", product_id);
        ++service->policy_count;
    }
    *existing = *policy;
    (void)memset(&product_policy, 0, sizeof(product_policy));
    (void)snprintf(product_policy.id, sizeof(product_policy.id), "%s", product_id);
    (void)snprintf(product_policy.product_id, sizeof(product_policy.product_id),
                   "%s", product_id);
    (void)snprintf(product_policy.channel, sizeof(product_policy.channel), "%s",
                   umi_release_channel_text(policy->channel));
    (void)snprintf(product_policy.allowed_range,
                   sizeof(product_policy.allowed_range), "%s", ">= installed");
    product_policy.automatic = policy->automatic_updates;
    product_policy.security_only = policy->security_only;
    product_policy.allow_prerelease = policy->allow_prerelease;
    product_policy.require_signature = policy->require_signature;
    status = umi_product_update_policy_registry_upsert(
        umi_product_centre_policies(service->products), &product_policy);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

/*
 * Provide the update marketplace status operation used by this module and its client
 * applications.
 */
static UmiStatus update_marketplace_status(UmiDistributionService *service)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_distribution_repository_count(service->repository);
         ++index) {
        UmiDistributionPackage package;
        UmiProductMarketplaceItemSnapshot item;
        UmiProductInstallationSnapshot installation;
        UmiStatus status = umi_distribution_repository_at(
            service->repository, index, &package);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_product_marketplace_registry_find(
            umi_product_centre_marketplace(service->products),
            package.release_id, &item);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_product_installation_state_registry_find(
            umi_product_centre_installations(service->products),
            package.product_id, &installation);
        item.installed = status == UMI_STATUS_OK;
        item.update_available = item.installed &&
            umi_distribution_version_compare(
                package.version, parse_version(installation.version)) > 0;
        status = umi_product_marketplace_registry_upsert(
            umi_product_centre_marketplace(service->products), &item);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the distribution service scan updates operation used by this module and its
 * client applications.
 */
UmiStatus umi_distribution_service_scan_updates(
    UmiDistributionService *service,
    uint32_t framework_abi,
    uint64_t timestamp_ms,
    size_t *out_available_updates)
{
    UmiProductInstallationRegistry *installations;
    size_t index;
    size_t available = 0U;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_available_updates == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    installations = umi_product_centre_installations(service->products);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_product_installation_state_registry_count(installations);
         ++index) {
        UmiProductInstallationSnapshot installation;
        UmiDistributionPackage package;
        UmiDistributionEvidence evidence;
        UmiDistributionDecision decision;
        UmiDistributionPolicy fallback;
        UmiDistributionPolicy *policy;
        UmiDistributionNotification notification;
        status = umi_product_installation_state_registry_at(
            installations, index, &installation);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        fallback = umi_distribution_policy_default();
        policy = find_policy(service, installation.product_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (policy == NULL) policy = &fallback;
        status = umi_distribution_repository_latest(
            service->repository, installation.product_id, policy->channel,
            framework_abi, &package);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_NOT_FOUND) continue;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        (void)memset(&evidence, 0, sizeof(evidence));
        (void)umi_distribution_evidence_registry_find(
            service->evidence, package.release_id, &evidence);
        status = umi_distribution_policy_evaluate(
            policy, 1, parse_version(installation.version), &package,
            &evidence, &decision);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!decision.eligible || !decision.update_available) continue;
        (void)memset(&notification, 0, sizeof(notification));
        (void)snprintf(notification.notification_id,
                       sizeof(notification.notification_id), "update.%.120s",
                       package.release_id);
        (void)snprintf(notification.product_id, sizeof(notification.product_id),
                       "%s", package.product_id);
        (void)snprintf(notification.release_id, sizeof(notification.release_id),
                       "%s", package.release_id);
        (void)snprintf(notification.title, sizeof(notification.title),
                       "%.220s update available", package.display_name);
        (void)snprintf(notification.message, sizeof(notification.message),
                       "Version %u.%u.%u passed channel, compatibility and supply-chain policy.",
                       (unsigned int)package.version.major,
                       (unsigned int)package.version.minor,
                       (unsigned int)package.version.patch);
        (void)snprintf(notification.action_id, sizeof(notification.action_id),
                       "%s", "studio.action.marketplace.plan-update");
        notification.kind = package.security_update
            ? UMI_DISTRIBUTION_NOTIFICATION_SECURITY
            : UMI_DISTRIBUTION_NOTIFICATION_UPDATE;
        notification.severity = package.security_update
            ? UMI_DISTRIBUTION_NOTICE_URGENT
            : UMI_DISTRIBUTION_NOTICE_ACTION;
        notification.created_at_ms = timestamp_ms;
        status = umi_distribution_notification_centre_upsert(
            service->notifications, &notification);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        ++available;
    }
    status = update_marketplace_status(service);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    service->available_updates = available;
    ++service->revision;
    *out_available_updates = available;
    return UMI_STATUS_OK;
}

/*
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
    UmiDistributionTransaction *out_transaction)
{
    UmiDistributionPackage package;
    UmiDistributionEvidence evidence;
    UmiDistributionPolicy fallback;
    UmiDistributionPolicy *policy;
    UmiProductInstallationSnapshot installation;
    UmiDistributionTransactionAction action;
    UmiDistributionNotification notification;
    UmiVersion installed_version = {0U, 0U, 0U};
    const char *root = install_root;
    int installed = 0;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || transaction_id == NULL || release_id == NULL ||
        install_root == NULL || out_decision == NULL || out_transaction == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_distribution_repository_find(
        service->repository, release_id, &package);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&evidence, 0, sizeof(evidence));
    (void)umi_distribution_evidence_registry_find(
        service->evidence, release_id, &evidence);
    status = umi_product_installation_state_registry_find(
        umi_product_centre_installations(service->products),
        package.product_id, &installation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        installed = 1;
        installed_version = parse_version(installation.version);
        /* Apply this branch only when its contract condition is satisfied. */
        if (root[0] == '\0') root = installation.install_root;
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_NOT_FOUND) {
        return status;
    }
    fallback = umi_distribution_policy_default();
    policy = find_policy(service, package.product_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) policy = &fallback;
    status = umi_distribution_policy_evaluate(
        policy, installed, installed_version, &package, &evidence, out_decision);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!out_decision->eligible) return UMI_STATUS_PERMISSION_DENIED;
    action = !installed ? UMI_DISTRIBUTION_INSTALL
        : (out_decision->downgrade ? UMI_DISTRIBUTION_ROLLBACK
                                   : UMI_DISTRIBUTION_UPDATE);
    status = umi_distribution_transaction_plan(
        out_transaction, transaction_id, action, package.product_id,
        package.release_id, installed_version, package.version, root, timestamp_ms);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_transaction->requires_restart = out_decision->requires_restart;
    status = umi_distribution_transaction_log_upsert(
        service->transactions, out_transaction);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(&notification, 0, sizeof(notification));
    (void)snprintf(notification.notification_id,
                   sizeof(notification.notification_id), "transaction.%.115s",
                   transaction_id);
    (void)snprintf(notification.product_id, sizeof(notification.product_id),
                   "%s", package.product_id);
    (void)snprintf(notification.release_id, sizeof(notification.release_id),
                   "%s", package.release_id);
    (void)snprintf(notification.title, sizeof(notification.title),
                   "%.230s plan ready", package.display_name);
    (void)snprintf(notification.message, sizeof(notification.message),
                   "%s", out_transaction->message);
    (void)snprintf(notification.action_id, sizeof(notification.action_id),
                   "%s", "studio.action.pane.product-transactions");
    notification.kind = UMI_DISTRIBUTION_NOTIFICATION_TRANSACTION;
    notification.severity = UMI_DISTRIBUTION_NOTICE_ACTION;
    notification.created_at_ms = timestamp_ms;
    status = umi_distribution_notification_centre_upsert(
        service->notifications, &notification);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) ++service->revision;
    return status;
}

/*
 * Return the number of records represented by distinct product without changing their
 * state.
 */
static size_t distinct_product_count(const UmiDistributionRepository *repository)
{
    size_t index;
    size_t count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_distribution_repository_count(repository); ++index) {
        UmiDistributionPackage current;
        size_t prior;
        int seen = 0;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_distribution_repository_at(repository, index, &current) !=
            UMI_STATUS_OK) continue;
        /* Visit each bounded item once so every record receives the same rule. */
        for (prior = 0U; prior < index; ++prior) {
            UmiDistributionPackage previous;
            /* Apply this branch only when its contract condition is satisfied. */
            if (umi_distribution_repository_at(repository, prior, &previous) ==
                    UMI_STATUS_OK &&
                strcmp(previous.product_id, current.product_id) == 0) {
                seen = 1;
                break;
            }
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!seen) ++count;
    }
    return count;
}

/*
 * Provide the distribution service snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_distribution_service_snapshot(
    const UmiDistributionService *service,
    UmiDistributionServiceSnapshot *out_snapshot)
{
    UmiProductCentreSnapshot products;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (service == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    status = umi_product_centre_snapshot(service->products, &products);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->revision = service->revision;
    out_snapshot->marketplace_releases =
        umi_distribution_repository_count(service->repository);
    out_snapshot->products = distinct_product_count(service->repository);
    out_snapshot->installations = products.installations_count;
    out_snapshot->policies = service->policy_count;
    out_snapshot->evidence_records =
        umi_distribution_evidence_registry_count(service->evidence);
    out_snapshot->transactions =
        umi_distribution_transaction_log_count(service->transactions);
    out_snapshot->notifications =
        umi_distribution_notification_centre_count(service->notifications);
    out_snapshot->unread_notifications =
        umi_distribution_notification_centre_unread_count(service->notifications);
    out_snapshot->available_updates = service->available_updates;
    return UMI_STATUS_OK;
}
