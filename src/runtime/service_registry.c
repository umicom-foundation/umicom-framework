/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/service_registry.c
 *
 * PURPOSE:
 *   Implement deterministic shared-service registration, lookup, replacement,
 *   unregistration and optional owned-service destruction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/service_registry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"

typedef struct UmiServiceEntry {
    UmiServiceDescriptor descriptor;
    char service_id[UMI_SERVICE_ID_CAPACITY];
    char provider_module_id[UMI_SERVICE_PROVIDER_CAPACITY];
} UmiServiceEntry;

struct UmiServiceRegistry {
    UmiServiceEntry entries[UMI_SERVICE_REGISTRY_MAX];
    size_t count;
    UmiMutex *mutex;
};

/* Provide the service matches operation used by this module and its client applications. */
static int umi_service_matches(const UmiServiceEntry *entry,
                               const char *service_id,
                               const char *provider_module_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL || service_id == NULL ||
        strcmp(entry->service_id, service_id) != 0) {
        return 0;
    }
    return provider_module_id == NULL ||
        strcmp(entry->provider_module_id, provider_module_id) == 0;
}

/* Release or reset state held by service entry so the same storage can be reused safely. */
static void umi_service_entry_destroy(UmiServiceEntry *entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry != NULL &&
        (entry->descriptor.flags & UMI_SERVICE_OWNED) != 0U &&
        entry->descriptor.destroy != NULL &&
        entry->descriptor.service != NULL) {
        entry->descriptor.destroy(entry->descriptor.service,
                                  entry->descriptor.destroy_user_data);
    }
}

/*
 * Initialise service registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_service_registry_create(UmiServiceRegistry **out_registry)
{
    UmiServiceRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiServiceRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_mutex_create(&registry->mutex) != UMI_STATUS_OK) {
        free(registry);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by service registry so the same storage can be reused
 * safely.
 */
void umi_service_registry_destroy(UmiServiceRegistry *registry)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = registry->count; index > 0U; --index) {
        umi_service_entry_destroy(&registry->entries[index - 1U]);
    }
    umi_mutex_destroy(registry->mutex);
    free(registry);
}

/* Add service registry only after its inputs and available capacity have been checked. */
UmiStatus umi_service_registry_register(UmiServiceRegistry *registry,
                                        const UmiServiceDescriptor *descriptor)
{
    size_t index;
    UmiServiceEntry *entry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || descriptor == NULL ||
        descriptor->structure_size < sizeof(*descriptor) ||
        descriptor->service_id == NULL || descriptor->service_id[0] == '\0' ||
        descriptor->provider_module_id == NULL ||
        descriptor->provider_module_id[0] == '\0' ||
        descriptor->service == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(descriptor->service_id) >= UMI_SERVICE_ID_CAPACITY ||
        strlen(descriptor->provider_module_id) >= UMI_SERVICE_PROVIDER_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)umi_mutex_lock(registry->mutex);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_service_matches(&registry->entries[index],
                                descriptor->service_id,
                                descriptor->provider_module_id)) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if ((registry->entries[index].descriptor.flags &
                 UMI_SERVICE_REPLACEABLE) == 0U) {
                (void)umi_mutex_unlock(registry->mutex);
                return UMI_STATUS_ALREADY_EXISTS;
            }
            umi_service_entry_destroy(&registry->entries[index]);
            entry = &registry->entries[index];
            goto store;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((descriptor->flags & UMI_SERVICE_SINGLETON) != 0U &&
            strcmp(registry->entries[index].service_id,
                   descriptor->service_id) == 0) {
            (void)umi_mutex_unlock(registry->mutex);
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_SERVICE_REGISTRY_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    entry = &registry->entries[registry->count++];

store:
    (void)memset(entry, 0, sizeof(*entry));
    (void)snprintf(entry->service_id,
                   sizeof(entry->service_id),
                   "%s",
                   descriptor->service_id);
    (void)snprintf(entry->provider_module_id,
                   sizeof(entry->provider_module_id),
                   "%s",
                   descriptor->provider_module_id);
    entry->descriptor = *descriptor;
    entry->descriptor.service_id = entry->service_id;
    entry->descriptor.provider_module_id = entry->provider_module_id;
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

/*
 * Remove service registry while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_service_registry_unregister(UmiServiceRegistry *registry,
                                          const char *service_id,
                                          const char *provider_module_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || service_id == NULL || provider_module_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)umi_mutex_lock(registry->mutex);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_service_matches(&registry->entries[index],
                                service_id,
                                provider_module_id)) {
            size_t move_count;
            umi_service_entry_destroy(&registry->entries[index]);
            move_count = registry->count - index - 1U;
            /* Apply this branch only when its contract condition is satisfied. */
            if (move_count > 0U) {
                (void)memmove(&registry->entries[index],
                              &registry->entries[index + 1U],
                              move_count * sizeof(registry->entries[0]));
            }
            registry->count -= 1U;
            (void)umi_mutex_unlock(registry->mutex);
            return UMI_STATUS_OK;
        }
    }
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find service registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiServiceDescriptor *umi_service_registry_find(
    const UmiServiceRegistry *registry,
    const char *service_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || service_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->entries[index].service_id, service_id) == 0) {
            return &registry->entries[index].descriptor;
        }
    }
    return NULL;
}

/*
 * Provide the service registry find provider operation used by this module and its client
 * applications.
 */
const UmiServiceDescriptor *umi_service_registry_find_provider(
    const UmiServiceRegistry *registry,
    const char *service_id,
    const char *provider_module_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || service_id == NULL || provider_module_id == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_service_matches(&registry->entries[index],
                                service_id,
                                provider_module_id)) {
            return &registry->entries[index].descriptor;
        }
    }
    return NULL;
}

/*
 * Return the number of records represented by service registry without changing their
 * state.
 */
size_t umi_service_registry_count(const UmiServiceRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Find service registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiServiceDescriptor *umi_service_registry_at(
    const UmiServiceRegistry *registry,
    size_t index)
{
    return registry != NULL && index < registry->count
        ? &registry->entries[index].descriptor
        : NULL;
}
