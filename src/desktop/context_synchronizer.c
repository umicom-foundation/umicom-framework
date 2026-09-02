/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/desktop/context_synchronizer.c
 *
 * PURPOSE:
 *   Implement acknowledged window-level context delivery on top of the
 *   Framework Application Context Hub and desktop link groups.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop/context_synchronizer.h"

#include <stdlib.h>
#include <string.h>

struct UmiDesktopContextSynchronizer {
    UmiDesktopRuntime *desktop;
    UmiDesktopContentRuntime *content;
    UmiDesktopContextSubscription
        subscriptions[UMI_DESKTOP_CONTEXT_SUBSCRIPTION_MAX];
    UmiDesktopContextDelivery deliveries[UMI_DESKTOP_CONTEXT_DELIVERY_MAX];
    size_t subscription_count;
    size_t delivery_count;
    uint64_t last_sequence;
    uint64_t revision;
};

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(
    char *destination,
    size_t capacity,
    const char *source,
    int required)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL ||
        (required && source[0] == '\0')) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the find subscription operation used by this module and its client applications. */
static size_t find_subscription(
    const UmiDesktopContextSynchronizer *synchronizer,
    const char *window_id,
    const char *context_type)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (synchronizer == NULL || window_id == NULL || context_type == NULL)
        return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < synchronizer->subscription_count; ++index) {
        const UmiDesktopContextSubscription *subscription =
            &synchronizer->subscriptions[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(subscription->window_id, window_id) == 0 &&
            strcmp(subscription->context_type, context_type) == 0)
            return index;
    }
    return SIZE_MAX;
}

/*
 * Provide the matching subscription operation used by this module and its client
 * applications.
 */
static const UmiDesktopContextSubscription *matching_subscription(
    const UmiDesktopContextSynchronizer *synchronizer,
    const char *window_id,
    const char *context_type)
{
    size_t index = find_subscription(synchronizer, window_id, context_type);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index != SIZE_MAX) return &synchronizer->subscriptions[index];
    index = find_subscription(synchronizer, window_id, "*");
    return index != SIZE_MAX ? &synchronizer->subscriptions[index] : NULL;
}

/*
 * Initialise desktop context synchronizer from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_desktop_context_synchronizer_create(
    UmiDesktopRuntime *desktop,
    UmiDesktopContentRuntime *content,
    UmiDesktopContextSynchronizer **out_synchronizer)
{
    UmiDesktopContextSynchronizer *synchronizer;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (desktop == NULL || content == NULL || out_synchronizer == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_synchronizer = NULL;
    synchronizer = (UmiDesktopContextSynchronizer *)calloc(
        1U, sizeof(*synchronizer));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (synchronizer == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    synchronizer->desktop = desktop;
    synchronizer->content = content;
    synchronizer->revision = 1U;
    *out_synchronizer = synchronizer;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by desktop context synchronizer so the same storage can be
 * reused safely.
 */
void umi_desktop_context_synchronizer_destroy(
    UmiDesktopContextSynchronizer *synchronizer)
{
    free(synchronizer);
}

/*
 * Provide the desktop context synchronizer subscribe operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_context_synchronizer_subscribe(
    UmiDesktopContextSynchronizer *synchronizer,
    const UmiDesktopContextSubscription *subscription)
{
    UmiDesktopContentSnapshot content;
    UmiDesktopContextSubscription *stored;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (synchronizer == NULL || subscription == NULL ||
        subscription->structure_size < sizeof(*subscription) ||
        subscription->window_id[0] == '\0' ||
        subscription->context_type[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_subscription(synchronizer, subscription->window_id,
                          subscription->context_type) != SIZE_MAX)
        return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (synchronizer->subscription_count >=
        UMI_DESKTOP_CONTEXT_SUBSCRIPTION_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_desktop_content_runtime_snapshot(
        synchronizer->content, subscription->window_id, &content);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)content;
    stored = &synchronizer->subscriptions[synchronizer->subscription_count];
    *stored = *subscription;
    stored->structure_size = (uint32_t)sizeof(*stored);
    stored->refresh_on_delivery = subscription->refresh_on_delivery != 0;
    synchronizer->subscription_count += 1U;
    synchronizer->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop context synchronizer unsubscribe operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_context_synchronizer_unsubscribe(
    UmiDesktopContextSynchronizer *synchronizer,
    const char *window_id,
    const char *context_type)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (synchronizer == NULL || window_id == NULL || context_type == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    index = find_subscription(synchronizer, window_id, context_type);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < synchronizer->subscription_count) {
        (void)memmove(&synchronizer->subscriptions[index],
                      &synchronizer->subscriptions[index + 1U],
                      (synchronizer->subscription_count - index - 1U) *
                          sizeof(synchronizer->subscriptions[0]));
    }
    synchronizer->subscription_count -= 1U;
    (void)memset(
        &synchronizer->subscriptions[synchronizer->subscription_count],
        0, sizeof(synchronizer->subscriptions[0]));
    synchronizer->revision += 1U;
    return UMI_STATUS_OK;
}

/* Provide the append delivery operation used by this module and its client applications. */
static UmiStatus append_delivery(
    UmiDesktopContextSynchronizer *synchronizer,
    const UmiDesktopWindow *source,
    const UmiDesktopWindow *destination,
    const UmiDesktopContextSubscription *subscription,
    const char *context_type,
    const char *subject_id,
    const char *correlation_id)
{
    UmiDesktopContextDelivery *delivery;
    UmiStatus status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (synchronizer->delivery_count >= UMI_DESKTOP_CONTEXT_DELIVERY_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    delivery = &synchronizer->deliveries[synchronizer->delivery_count];
    (void)memset(delivery, 0, sizeof(*delivery));
    delivery->structure_size = (uint32_t)sizeof(*delivery);
    delivery->sequence = synchronizer->last_sequence + 1U;
    status = copy_text(delivery->group_id, sizeof(delivery->group_id),
                       source->context_group_id, 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(delivery->source_window_id,
                           sizeof(delivery->source_window_id),
                           source->window_id, 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(delivery->source_application_id,
                           sizeof(delivery->source_application_id),
                           source->owner_application_id, 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(delivery->destination_window_id,
                           sizeof(delivery->destination_window_id),
                           destination->window_id, 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(delivery->destination_application_id,
                           sizeof(delivery->destination_application_id),
                           destination->owner_application_id, 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(delivery->context_type,
                           sizeof(delivery->context_type), context_type, 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(delivery->subject_id,
                           sizeof(delivery->subject_id), subject_id, 1);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(delivery->correlation_id,
                           sizeof(delivery->correlation_id),
                           correlation_id != NULL ? correlation_id : "", 0);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    delivery->refresh_status = subscription->refresh_on_delivery
        ? umi_desktop_content_runtime_refresh(
              synchronizer->content, destination->window_id)
        : UMI_STATUS_OK;
    delivery->revision = 1U;
    synchronizer->last_sequence = delivery->sequence;
    synchronizer->delivery_count += 1U;
    synchronizer->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop context synchronizer publish operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_context_synchronizer_publish(
    UmiDesktopContextSynchronizer *synchronizer,
    const char *source_window_id,
    const char *context_type,
    const char *subject_id,
    const char *correlation_id)
{
    const UmiDesktopWindow *source;
    const char *destinations[UMI_UI_WINDOW_GROUP_MAX_MEMBERS];
    size_t count;
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (synchronizer == NULL || source_window_id == NULL ||
        context_type == NULL || context_type[0] == '\0' ||
        subject_id == NULL || subject_id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    source = umi_desktop_window_manager_find(
        umi_desktop_runtime_windows(synchronizer->desktop), source_window_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (source == NULL) return UMI_STATUS_NOT_FOUND;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (source->context_group_id[0] == '\0')
        return UMI_STATUS_INVALID_STATE;
    status = umi_desktop_content_runtime_publish_context(
        synchronizer->content, source_window_id, context_type, subject_id,
        correlation_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    count = umi_desktop_context_links_route(
        umi_desktop_runtime_context_links(synchronizer->desktop),
        source->context_group_id, source_window_id, destinations,
        UMI_UI_WINDOW_GROUP_MAX_MEMBERS);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_UI_WINDOW_GROUP_MAX_MEMBERS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        const UmiDesktopWindow *destination;
        const UmiDesktopContextSubscription *subscription =
            matching_subscription(
                synchronizer, destinations[index], context_type);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (subscription == NULL) continue;
        destination = umi_desktop_window_manager_find(
            umi_desktop_runtime_windows(synchronizer->desktop),
            destinations[index]);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (destination == NULL) return UMI_STATUS_INVALID_STATE;
        status = append_delivery(
            synchronizer, source, destination, subscription, context_type,
            subject_id, correlation_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop context synchronizer next operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_context_synchronizer_next(
    const UmiDesktopContextSynchronizer *synchronizer,
    const char *destination_window_id,
    UmiDesktopContextDelivery *out_delivery)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (synchronizer == NULL || destination_window_id == NULL ||
        out_delivery == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < synchronizer->delivery_count; ++index) {
        const UmiDesktopContextDelivery *delivery =
            &synchronizer->deliveries[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!delivery->acknowledged &&
            strcmp(delivery->destination_window_id,
                   destination_window_id) == 0) {
            *out_delivery = *delivery;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the desktop context synchronizer acknowledge operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_context_synchronizer_acknowledge(
    UmiDesktopContextSynchronizer *synchronizer,
    const char *destination_window_id,
    uint64_t sequence)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (synchronizer == NULL || destination_window_id == NULL || sequence == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < synchronizer->delivery_count; ++index) {
        UmiDesktopContextDelivery *delivery = &synchronizer->deliveries[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (delivery->sequence == sequence &&
            strcmp(delivery->destination_window_id,
                   destination_window_id) == 0) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (delivery->acknowledged) return UMI_STATUS_INVALID_STATE;
            delivery->acknowledged = 1;
            delivery->revision += 1U;
            synchronizer->revision += 1U;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find desktop context synchronizer delivery while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_desktop_context_synchronizer_delivery_at(
    const UmiDesktopContextSynchronizer *synchronizer,
    size_t index,
    UmiDesktopContextDelivery *out_delivery)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (synchronizer == NULL || out_delivery == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= synchronizer->delivery_count) return UMI_STATUS_NOT_FOUND;
    *out_delivery = synchronizer->deliveries[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the desktop context synchronizer snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_context_synchronizer_snapshot(
    const UmiDesktopContextSynchronizer *synchronizer,
    UmiDesktopContextSynchronizerSnapshot *out_snapshot)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (synchronizer == NULL || out_snapshot == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->structure_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->subscription_count = synchronizer->subscription_count;
    out_snapshot->delivery_count = synchronizer->delivery_count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < synchronizer->delivery_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!synchronizer->deliveries[index].acknowledged)
            out_snapshot->pending_count += 1U;
    }
    out_snapshot->last_sequence = synchronizer->last_sequence;
    out_snapshot->revision = synchronizer->revision;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by desktop context synchronizer subscription
 * without changing their state.
 */
size_t umi_desktop_context_synchronizer_subscription_count(
    const UmiDesktopContextSynchronizer *synchronizer)
{
    return synchronizer != NULL ? synchronizer->subscription_count : 0U;
}
