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

typedef enum UmiDistributionNotificationKind {
    UMI_DISTRIBUTION_NOTIFICATION_UPDATE = 1,
    UMI_DISTRIBUTION_NOTIFICATION_SECURITY = 2,
    UMI_DISTRIBUTION_NOTIFICATION_TRANSACTION = 3,
    UMI_DISTRIBUTION_NOTIFICATION_ROLLBACK = 4
} UmiDistributionNotificationKind;

typedef enum UmiDistributionNotificationSeverity {
    UMI_DISTRIBUTION_NOTICE_INFO = 1,
    UMI_DISTRIBUTION_NOTICE_ACTION = 2,
    UMI_DISTRIBUTION_NOTICE_URGENT = 3
} UmiDistributionNotificationSeverity;

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

typedef struct UmiDistributionNotificationCentre UmiDistributionNotificationCentre;

UmiStatus umi_distribution_notification_centre_create(
    UmiDistributionNotificationCentre **out_centre);
void umi_distribution_notification_centre_destroy(
    UmiDistributionNotificationCentre *centre);
UmiStatus umi_distribution_notification_centre_upsert(
    UmiDistributionNotificationCentre *centre,
    const UmiDistributionNotification *notification);
UmiStatus umi_distribution_notification_centre_at(
    const UmiDistributionNotificationCentre *centre,
    size_t index,
    UmiDistributionNotification *out_notification);
UmiStatus umi_distribution_notification_centre_mark_read(
    UmiDistributionNotificationCentre *centre,
    const char *notification_id,
    int read);
size_t umi_distribution_notification_centre_count(
    const UmiDistributionNotificationCentre *centre);
size_t umi_distribution_notification_centre_unread_count(
    const UmiDistributionNotificationCentre *centre);

#ifdef __cplusplus
}
#endif

#endif
