/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/distribution/notification.h
 *
 * PURPOSE:
 *   Retain actionable update, security, transaction and rollback notifications
 *   for product frontends without exposing a toolkit notification object.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#ifndef UMICOM_DISTRIBUTION_NOTIFICATION_H
#define UMICOM_DISTRIBUTION_NOTIFICATION_H

#include "umicom/distribution/transaction.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DISTRIBUTION_NOTIFICATION_CAPACITY 512U

/**
 * List the named distribution notification kind values accepted by this public contract.
 */
typedef enum UmiDistributionNotificationKind {
    UMI_DISTRIBUTION_NOTIFICATION_UPDATE = 1,
    UMI_DISTRIBUTION_NOTIFICATION_SECURITY = 2,
    UMI_DISTRIBUTION_NOTIFICATION_TRANSACTION = 3,
    UMI_DISTRIBUTION_NOTIFICATION_ROLLBACK = 4
} UmiDistributionNotificationKind;

/**
 * List the named distribution notification severity values accepted by this public
 * contract.
 */
typedef enum UmiDistributionNotificationSeverity {
    UMI_DISTRIBUTION_NOTICE_INFO = 1,
    UMI_DISTRIBUTION_NOTICE_ACTION = 2,
    UMI_DISTRIBUTION_NOTICE_URGENT = 3
} UmiDistributionNotificationSeverity;

/**
 * Represent the distribution notification data shared with callers of this public
 * contract.
 */
typedef struct UmiDistributionNotification {
    char notification_id[UMI_DISTRIBUTION_ID_CAPACITY];
    char product_id[UMI_DISTRIBUTION_ID_CAPACITY];
    char release_id[UMI_DISTRIBUTION_ID_CAPACITY];
    char title[UMI_DISTRIBUTION_NAME_CAPACITY];
    char message[UMI_DISTRIBUTION_TEXT_CAPACITY];
    char action_id[UMI_DISTRIBUTION_ID_CAPACITY];
    UmiDistributionNotificationKind kind;
    UmiDistributionNotificationSeverity severity;
    uint64_t created_at_ms;
    int read;
    uint64_t revision;
} UmiDistributionNotification;

/**
 * Represent the distribution notification centre data shared with callers of this public
 * contract.
 */
typedef struct UmiDistributionNotificationCentre UmiDistributionNotificationCentre;

/**
 * Initialise distribution notification centre from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_distribution_notification_centre_create(
    UmiDistributionNotificationCentre **out_centre);
/**
 * Release or reset state held by distribution notification centre so the same storage can
 * be reused safely.
 */
void umi_distribution_notification_centre_destroy(
    UmiDistributionNotificationCentre *centre);
/**
 * Provide the distribution notification centre upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_distribution_notification_centre_upsert(
    UmiDistributionNotificationCentre *centre,
    const UmiDistributionNotification *notification);
/**
 * Find distribution notification centre while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_distribution_notification_centre_at(
    const UmiDistributionNotificationCentre *centre,
    size_t index,
    UmiDistributionNotification *out_notification);
/**
 * Read distribution notification centre mark into validated module state and return a
 * status when input cannot be used.
 */
UmiStatus umi_distribution_notification_centre_mark_read(
    UmiDistributionNotificationCentre *centre,
    const char *notification_id,
    int read);
/**
 * Return the number of records represented by distribution notification centre without
 * changing their state.
 */
size_t umi_distribution_notification_centre_count(
    const UmiDistributionNotificationCentre *centre);
/**
 * Return the number of records represented by distribution notification centre unread
 * without changing their state.
 */
size_t umi_distribution_notification_centre_unread_count(
    const UmiDistributionNotificationCentre *centre);

#ifdef __cplusplus
}
#endif

#endif
