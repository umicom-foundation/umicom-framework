/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/context_hub.c
 *
 * PURPOSE:
 *   Implement Framework-owned linked context groups for independent windows.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL ||
        source[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

static size_t find_group(const UmiApplicationContextHub *hub,
                         const char *group_id)
{
    size_t index;
    if (hub == NULL || group_id == NULL) return SIZE_MAX;
    for (index = 0U; index < hub->group_count; ++index) {
        if (strcmp(hub->groups[index].group_id, group_id) == 0) return index;
    }
    return SIZE_MAX;
}

UmiStatus umi_application_context_hub_create(
    UmiApplicationContextHub **out_hub)
{
    if (out_hub == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_hub = (UmiApplicationContextHub *)calloc(1U, sizeof(**out_hub));
    return *out_hub != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

void umi_application_context_hub_destroy(UmiApplicationContextHub *hub)
{
    free(hub);
}

UmiStatus umi_application_context_hub_subscribe(
    UmiApplicationContextHub *hub,
    const char *application_id,
    const char *group_id,
    UmiApplicationContextListener listener,
    void *listener_context)
{
    UmiContextSubscription *subscription;
    UmiStatus status;
    if (hub == NULL || listener == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (hub->subscriber_count >= UMI_CONTEXT_HUB_MAX_SUBSCRIBERS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    subscription = &hub->subscribers[hub->subscriber_count];
    status = copy_text(subscription->application_id,
                       sizeof(subscription->application_id), application_id);
    if (status == UMI_STATUS_OK)
        status = copy_text(subscription->group_id,
                           sizeof(subscription->group_id), group_id);
    if (status != UMI_STATUS_OK) return status;
    subscription->listener = listener;
    subscription->listener_context = listener_context;
    hub->subscriber_count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_context_hub_publish(
    UmiApplicationContextHub *hub,
    const UmiApplicationContext *context)
{
    size_t group_index;
    size_t index;
    UmiApplicationContext stored;
    if (hub == NULL || context == NULL || context->group_id[0] == '\0' ||
        context->source_application_id[0] == '\0' ||
        context->context_type[0] == '\0' || context->subject_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    group_index = find_group(hub, context->group_id);
    if (group_index == SIZE_MAX) {
        if (hub->group_count >= UMI_CONTEXT_HUB_MAX_GROUPS)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        group_index = hub->group_count++;
    }
    stored = *context;
    stored.revision = hub->groups[group_index].revision + 1U;
    hub->groups[group_index] = stored;
    for (index = 0U; index < hub->subscriber_count; ++index) {
        const UmiContextSubscription *subscription = &hub->subscribers[index];
        if (strcmp(subscription->group_id, stored.group_id) == 0 &&
            strcmp(subscription->application_id,
                   stored.source_application_id) != 0) {
            subscription->listener(subscription->listener_context, &stored);
        }
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_application_context_hub_snapshot(
    const UmiApplicationContextHub *hub,
    const char *group_id,
    UmiApplicationContext *out_context)
{
    const size_t index = find_group(hub, group_id);
    if (hub == NULL || group_id == NULL || out_context == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    *out_context = hub->groups[index];
    return UMI_STATUS_OK;
}

size_t umi_application_context_hub_group_count(
    const UmiApplicationContextHub *hub)
{
    return hub != NULL ? hub->group_count : 0U;
}
