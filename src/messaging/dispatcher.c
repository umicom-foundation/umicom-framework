/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/messaging/dispatcher.c
 *
 * PURPOSE:
 *   Implement schema-aware typed handler registration, unsubscription, matching dispatch and operational counters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/messaging/dispatcher.h"

#include <stdlib.h>
#include <string.h>

#include "include/messaging_internal.h"

#define UMI_DISPATCHER_CAPACITY 256U

typedef struct UmiHandlerEntry {
    uint64_t id;
    UmiSubscription subscription;
    char *name;
    char *schema;
    char *source;
    char *partition;
    UmiMessageHandler handler;
    void *user_data;
    int active;
} UmiHandlerEntry;

struct UmiDispatcher {
    UmiSpinLock lock;
    UmiSchemaRegistry *schemas;
    UmiHandlerEntry entries[UMI_DISPATCHER_CAPACITY];
    size_t count;
    uint64_t next_id;
    UmiDispatcherStats stats;
};

UmiStatus umi_dispatcher_create(UmiSchemaRegistry *schemas,
                                UmiDispatcher **out_dispatcher)
{
    UmiDispatcher *dispatcher;
    if (out_dispatcher == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_dispatcher = NULL;
    dispatcher = (UmiDispatcher *)calloc(1U, sizeof(*dispatcher));
    if (dispatcher == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_spin_init(&dispatcher->lock);
    dispatcher->schemas = schemas;
    dispatcher->next_id = 1U;
    *out_dispatcher = dispatcher;
    return UMI_STATUS_OK;
}

static void entry_dispose(UmiHandlerEntry *entry)
{
    free(entry->name);
    free(entry->schema);
    free(entry->source);
    free(entry->partition);
    (void)memset(entry, 0, sizeof(*entry));
}

void umi_dispatcher_destroy(UmiDispatcher *dispatcher)
{
    size_t index;
    if (dispatcher == NULL) return;
    for (index = 0U; index < dispatcher->count; ++index) {
        entry_dispose(&dispatcher->entries[index]);
    }
    free(dispatcher);
}

static UmiStatus duplicate_optional(const char *value, char **out_value)
{
    if (value == NULL || value[0] == '\0') {
        *out_value = NULL;
        return UMI_STATUS_OK;
    }
    *out_value = umi_message_strdup(value);
    return *out_value != NULL ? UMI_STATUS_OK : UMI_STATUS_OUT_OF_MEMORY;
}

UmiStatus umi_dispatcher_subscribe(UmiDispatcher *dispatcher,
                                   const UmiSubscription *subscription,
                                   UmiMessageHandler handler,
                                   void *user_data,
                                   uint64_t *out_subscription_id)
{
    UmiHandlerEntry entry;
    UmiStatus status;
    if (dispatcher == NULL || subscription == NULL || handler == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(&entry, 0, sizeof(entry));
    entry.subscription = *subscription;
    entry.handler = handler;
    entry.user_data = user_data;
    entry.active = 1;
#define DUP(value, storage)                                                  \
    do {                                                                     \
        status = duplicate_optional((value), &(storage));                    \
        if (status != UMI_STATUS_OK) {                                       \
            entry_dispose(&entry);                                           \
            return status;                                                   \
        }                                                                    \
    } while (0)
    DUP(subscription->name, entry.name);
    DUP(subscription->schema_id, entry.schema);
    DUP(subscription->source, entry.source);
    DUP(subscription->partition_key, entry.partition);
#undef DUP
    entry.subscription.name = entry.name;
    entry.subscription.schema_id = entry.schema;
    entry.subscription.source = entry.source;
    entry.subscription.partition_key = entry.partition;

    umi_spin_lock(&dispatcher->lock);
    if (dispatcher->count >= UMI_DISPATCHER_CAPACITY) {
        umi_spin_unlock(&dispatcher->lock);
        entry_dispose(&entry);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    entry.id = dispatcher->next_id++;
    dispatcher->entries[dispatcher->count++] = entry;
    dispatcher->stats.handlers++;
    umi_spin_unlock(&dispatcher->lock);
    if (out_subscription_id != NULL) *out_subscription_id = entry.id;
    return UMI_STATUS_OK;
}

UmiStatus umi_dispatcher_unsubscribe(UmiDispatcher *dispatcher,
                                     uint64_t subscription_id)
{
    size_t index;
    if (dispatcher == NULL || subscription_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_spin_lock(&dispatcher->lock);
    for (index = 0U; index < dispatcher->count; ++index) {
        if (dispatcher->entries[index].active &&
            dispatcher->entries[index].id == subscription_id) {
            dispatcher->entries[index].active = 0;
            dispatcher->stats.handlers--;
            umi_spin_unlock(&dispatcher->lock);
            return UMI_STATUS_OK;
        }
    }
    umi_spin_unlock(&dispatcher->lock);
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_dispatcher_dispatch(UmiDispatcher *dispatcher,
                                  const UmiMessageEnvelope *message,
                                  size_t *out_delivery_count)
{
    size_t index;
    size_t delivered = 0U;
    UmiStatus first_failure = UMI_STATUS_OK;
    UmiStatus status;
    if (dispatcher == NULL || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (dispatcher->schemas != NULL && message->schema_id != NULL &&
        message->schema_id[0] != '\0') {
        status = umi_schema_registry_validate(dispatcher->schemas, message);
    } else {
        status = umi_message_validate(message);
    }
    dispatcher->stats.dispatched++;
    if (status != UMI_STATUS_OK) {
        dispatcher->stats.rejected++;
        return status;
    }
    for (index = 0U; index < dispatcher->count; ++index) {
        UmiHandlerEntry *entry = &dispatcher->entries[index];
        if (!entry->active ||
            !umi_subscription_matches(&entry->subscription, message)) {
            continue;
        }
        status = entry->handler(message, entry->user_data);
        delivered++;
        if (status != UMI_STATUS_OK && first_failure == UMI_STATUS_OK) {
            first_failure = status;
            dispatcher->stats.failures++;
        }
    }
    dispatcher->stats.delivered += (uint64_t)delivered;
    if (out_delivery_count != NULL) *out_delivery_count = delivered;
    return first_failure;
}

UmiDispatcherStats umi_dispatcher_stats(const UmiDispatcher *dispatcher)
{
    UmiDispatcherStats stats;
    (void)memset(&stats, 0, sizeof(stats));
    return dispatcher != NULL ? dispatcher->stats : stats;
}
