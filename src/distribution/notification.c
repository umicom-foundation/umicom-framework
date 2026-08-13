/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/distribution/notification.c
 *
 * PURPOSE:
 *   Implement bounded, toolkit-neutral distribution notifications.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include "umicom/distribution/notification.h"

#include <stdlib.h>
#include <string.h>

struct UmiDistributionNotificationCentre {
    UmiDistributionNotification items[UMI_DISTRIBUTION_NOTIFICATION_CAPACITY];
    size_t count;
    uint64_t revision;
};

static size_t find_index(const UmiDistributionNotificationCentre *centre,
                         const char *notification_id)
{
    size_t index;
    if (centre == NULL || notification_id == NULL) return SIZE_MAX;
    for (index = 0U; index < centre->count; ++index) {
        if (strcmp(centre->items[index].notification_id, notification_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

UmiStatus umi_distribution_notification_centre_create(
    UmiDistributionNotificationCentre **out_centre)
{
    UmiDistributionNotificationCentre *centre;
    if (out_centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_centre = NULL;
    centre = (UmiDistributionNotificationCentre *)calloc(1U, sizeof(*centre));
    if (centre == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    centre->revision = 1U;
    *out_centre = centre;
    return UMI_STATUS_OK;
}

void umi_distribution_notification_centre_destroy(
    UmiDistributionNotificationCentre *centre)
{
    free(centre);
}

UmiStatus umi_distribution_notification_centre_upsert(
    UmiDistributionNotificationCentre *centre,
    const UmiDistributionNotification *notification)
{
    size_t index;
    if (centre == NULL || notification == NULL ||
        notification->notification_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(centre, notification->notification_id);
    if (index == SIZE_MAX) {
        if (centre->count >= UMI_DISTRIBUTION_NOTIFICATION_CAPACITY) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = centre->count++;
    }
    centre->items[index] = *notification;
    ++centre->revision;
    centre->items[index].revision = centre->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_distribution_notification_centre_at(
    const UmiDistributionNotificationCentre *centre,
    size_t index,
    UmiDistributionNotification *out_notification)
{
    if (centre == NULL || out_notification == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= centre->count) return UMI_STATUS_NOT_FOUND;
    *out_notification = centre->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_distribution_notification_centre_mark_read(
    UmiDistributionNotificationCentre *centre,
    const char *notification_id,
    int read)
{
    size_t index;
    if (centre == NULL || notification_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(centre, notification_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    centre->items[index].read = read != 0;
    ++centre->revision;
    centre->items[index].revision = centre->revision;
    return UMI_STATUS_OK;
}

size_t umi_distribution_notification_centre_count(
    const UmiDistributionNotificationCentre *centre)
{
    return centre != NULL ? centre->count : 0U;
}

size_t umi_distribution_notification_centre_unread_count(
    const UmiDistributionNotificationCentre *centre)
{
    size_t index;
    size_t count = 0U;
    if (centre == NULL) return 0U;
    for (index = 0U; index < centre->count; ++index) {
        if (!centre->items[index].read) ++count;
    }
    return count;
}
