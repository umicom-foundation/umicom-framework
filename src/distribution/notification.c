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

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(const UmiDistributionNotificationCentre *centre,
                         const char *notification_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL || notification_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < centre->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(centre->items[index].notification_id, notification_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/*
 * Initialise distribution notification centre from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_distribution_notification_centre_create(
    UmiDistributionNotificationCentre **out_centre)
{
    UmiDistributionNotificationCentre *centre;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_centre == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_centre = NULL;
    centre = (UmiDistributionNotificationCentre *)calloc(1U, sizeof(*centre));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    centre->revision = 1U;
    *out_centre = centre;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by distribution notification centre so the same storage can
 * be reused safely.
 */
void umi_distribution_notification_centre_destroy(
    UmiDistributionNotificationCentre *centre)
{
    free(centre);
}

/*
 * Provide the distribution notification centre upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_distribution_notification_centre_upsert(
    UmiDistributionNotificationCentre *centre,
    const UmiDistributionNotification *notification)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL || notification == NULL ||
        notification->notification_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(centre, notification->notification_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Find distribution notification centre while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_distribution_notification_centre_at(
    const UmiDistributionNotificationCentre *centre,
    size_t index,
    UmiDistributionNotification *out_notification)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL || out_notification == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= centre->count) return UMI_STATUS_NOT_FOUND;
    *out_notification = centre->items[index];
    return UMI_STATUS_OK;
}

/*
 * Read distribution notification centre mark into validated module state and return a
 * status when input cannot be used.
 */
UmiStatus umi_distribution_notification_centre_mark_read(
    UmiDistributionNotificationCentre *centre,
    const char *notification_id,
    int read)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL || notification_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    index = find_index(centre, notification_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    centre->items[index].read = read != 0;
    ++centre->revision;
    centre->items[index].revision = centre->revision;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by distribution notification centre without
 * changing their state.
 */
size_t umi_distribution_notification_centre_count(
    const UmiDistributionNotificationCentre *centre)
{
    return centre != NULL ? centre->count : 0U;
}

/*
 * Return the number of records represented by distribution notification centre unread
 * without changing their state.
 */
size_t umi_distribution_notification_centre_unread_count(
    const UmiDistributionNotificationCentre *centre)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < centre->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!centre->items[index].read) ++count;
    }
    return count;
}
