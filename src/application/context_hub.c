/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/context_hub.c
 *
 * PURPOSE:
 *   Implement Framework-owned linked context groups for independent windows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/context_hub.h"

#include <stdlib.h>
#include <string.h>

typedef struct UmiContextSubscription {
    char application_id[UMI_CONTEXT_HUB_TEXT_CAPACITY];
    char group_id[UMI_CONTEXT_HUB_TEXT_CAPACITY];
    UmiApplicationContextListener listener;
    void *listener_context;
} UmiContextSubscription;

struct UmiApplicationContextHub {
    UmiApplicationContext groups[UMI_CONTEXT_HUB_MAX_GROUPS];
    UmiContextSubscription subscribers[UMI_CONTEXT_HUB_MAX_SUBSCRIBERS];
    size_t group_count;
    size_t subscriber_count;
};

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL ||
        source[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the find group operation used by this module and its client applications. */
static size_t find_group(const UmiApplicationContextHub *hub,
                         const char *group_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hub == NULL || group_id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < hub->group_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(hub->groups[index].group_id, group_id) == 0) return index;
    }
    return SIZE_MAX;
}

/*
 * Initialise application context hub from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_context_hub_create(
    UmiApplicationContextHub **out_hub)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_hub == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_hub = (UmiApplicationContextHub *)calloc(1U, sizeof(**out_hub));
    return *out_hub != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

/*
 * Release or reset state held by application context hub so the same storage can be reused
 * safely.
 */
void umi_application_context_hub_destroy(UmiApplicationContextHub *hub)
{
    free(hub);
}

/*
 * Provide the application context hub subscribe operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_context_hub_subscribe(
    UmiApplicationContextHub *hub,
    const char *application_id,
    const char *group_id,
    UmiApplicationContextListener listener,
    void *listener_context)
{
    UmiContextSubscription *subscription;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hub == NULL || listener == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (hub->subscriber_count >= UMI_CONTEXT_HUB_MAX_SUBSCRIBERS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    subscription = &hub->subscribers[hub->subscriber_count];
    status = copy_text(subscription->application_id,
                       sizeof(subscription->application_id), application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(subscription->group_id,
                           sizeof(subscription->group_id), group_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    subscription->listener = listener;
    subscription->listener_context = listener_context;
    hub->subscriber_count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the application context hub publish operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_context_hub_publish(
    UmiApplicationContextHub *hub,
    const UmiApplicationContext *context)
{
    size_t group_index;
    size_t index;
    UmiApplicationContext stored;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hub == NULL || context == NULL || context->group_id[0] == '\0' ||
        context->source_application_id[0] == '\0' ||
        context->context_type[0] == '\0' || context->subject_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    group_index = find_group(hub, context->group_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (group_index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (hub->group_count >= UMI_CONTEXT_HUB_MAX_GROUPS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        group_index = hub->group_count++;
    }
    stored = *context;
    stored.revision = hub->groups[group_index].revision + 1U;
    hub->groups[group_index] = stored;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < hub->subscriber_count; ++index) {
        const UmiContextSubscription *subscription = &hub->subscribers[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(subscription->group_id, stored.group_id) == 0 &&
            strcmp(subscription->application_id,
                   stored.source_application_id) != 0) {
            subscription->listener(subscription->listener_context, &stored);
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the application context hub snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_context_hub_snapshot(
    const UmiApplicationContextHub *hub,
    const char *group_id,
    UmiApplicationContext *out_context)
{
    const size_t index = find_group(hub, group_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (hub == NULL || group_id == NULL || out_context == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_context = hub->groups[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by application context hub group without
 * changing their state.
 */
size_t umi_application_context_hub_group_count(
    const UmiApplicationContextHub *hub)
{
    return hub != NULL ? hub->group_count : 0U;
}
