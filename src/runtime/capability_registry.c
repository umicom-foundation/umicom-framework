/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/capability_registry.c
 *
 * PURPOSE:
 *   Implement deterministic capability registration, lookup, dependency
 *   validation, and provider ownership without general runtime reflection or
 *   cross-module access to private implementation details.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/capability_registry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#include <windows.h>
typedef CRITICAL_SECTION UmiCapabilityLock;
/*
 * Initialise capability lock from caller-provided values so later operations receive a
 * known state.
 */
static void umi_capability_lock_init(UmiCapabilityLock *lock)
{
    InitializeCriticalSection(lock);
}
/* Release or reset state held by capability lock so the same storage can be reused safely. */
static void umi_capability_lock_dispose(UmiCapabilityLock *lock)
{
    DeleteCriticalSection(lock);
}
/*
 * Provide the capability lock enter operation used by this module and its client
 * applications.
 */
static void umi_capability_lock_enter(UmiCapabilityLock *lock)
{
    EnterCriticalSection(lock);
}
/*
 * Provide the capability lock leave operation used by this module and its client
 * applications.
 */
static void umi_capability_lock_leave(UmiCapabilityLock *lock)
{
    LeaveCriticalSection(lock);
}
#else
#include <pthread.h>
typedef pthread_mutex_t UmiCapabilityLock;
/*
 * Initialise capability lock from caller-provided values so later operations receive a
 * known state.
 */
static void umi_capability_lock_init(UmiCapabilityLock *lock)
{
    (void)pthread_mutex_init(lock, NULL);
}
/* Release or reset state held by capability lock so the same storage can be reused safely. */
static void umi_capability_lock_dispose(UmiCapabilityLock *lock)
{
    (void)pthread_mutex_destroy(lock);
}
/*
 * Provide the capability lock enter operation used by this module and its client
 * applications.
 */
static void umi_capability_lock_enter(UmiCapabilityLock *lock)
{
    (void)pthread_mutex_lock(lock);
}
/*
 * Provide the capability lock leave operation used by this module and its client
 * applications.
 */
static void umi_capability_lock_leave(UmiCapabilityLock *lock)
{
    (void)pthread_mutex_unlock(lock);
}
#endif

typedef struct UmiCapabilityEntry {
    UmiCapabilityDescriptor descriptor;
    char capability_id[UMI_CAPABILITY_ID_CAPACITY];
    char provider_module_id[UMI_CAPABILITY_PROVIDER_CAPACITY];
} UmiCapabilityEntry;

struct UmiCapabilityRegistry {
    UmiCapabilityEntry entries[UMI_CAPABILITY_REGISTRY_MAX];
    size_t count;
    UmiCapabilityLock lock;
};

/*
 * Provide the capability matches operation used by this module and its client
 * applications.
 */
static int umi_capability_matches(const UmiCapabilityEntry *entry,
                                  const char *capability_id,
                                  const char *provider_module_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL || capability_id == NULL ||
        strcmp(entry->capability_id, capability_id) != 0) {
        return 0;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider_module_id == NULL) {
        return 1;
    }
    return strcmp(entry->provider_module_id, provider_module_id) == 0;
}

/*
 * Initialise capability registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_capability_registry_create(
    UmiCapabilityRegistry **out_registry)
{
    UmiCapabilityRegistry *registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_registry = NULL;

    registry = (UmiCapabilityRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    umi_capability_lock_init(&registry->lock);
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by capability registry so the same storage can be reused
 * safely.
 */
void umi_capability_registry_destroy(UmiCapabilityRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) {
        return;
    }
    umi_capability_lock_dispose(&registry->lock);
    free(registry);
}

/* Add capability registry only after its inputs and available capacity have been checked. */
UmiStatus umi_capability_registry_register(
    UmiCapabilityRegistry *registry,
    const UmiCapabilityDescriptor *descriptor)
{
    size_t index;
    UmiCapabilityEntry *entry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || descriptor == NULL ||
        descriptor->structure_size < sizeof(*descriptor) ||
        descriptor->capability_id == NULL ||
        descriptor->capability_id[0] == '\0' ||
        descriptor->provider_module_id == NULL ||
        descriptor->provider_module_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(descriptor->capability_id) >= UMI_CAPABILITY_ID_CAPACITY ||
        strlen(descriptor->provider_module_id) >=
            UMI_CAPABILITY_PROVIDER_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    umi_capability_lock_enter(&registry->lock);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_capability_matches(&registry->entries[index],
                                   descriptor->capability_id,
                                   descriptor->provider_module_id)) {
            umi_capability_lock_leave(&registry->lock);
            return UMI_STATUS_ALREADY_EXISTS;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if ((descriptor->flags & UMI_CAPABILITY_SINGLETON) != 0U &&
            strcmp(registry->entries[index].capability_id,
                   descriptor->capability_id) == 0) {
            umi_capability_lock_leave(&registry->lock);
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_CAPABILITY_REGISTRY_MAX) {
        umi_capability_lock_leave(&registry->lock);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    entry = &registry->entries[registry->count];
    (void)memset(entry, 0, sizeof(*entry));
    (void)snprintf(entry->capability_id,
                   sizeof(entry->capability_id),
                   "%s",
                   descriptor->capability_id);
    (void)snprintf(entry->provider_module_id,
                   sizeof(entry->provider_module_id),
                   "%s",
                   descriptor->provider_module_id);
    entry->descriptor = *descriptor;
    entry->descriptor.capability_id = entry->capability_id;
    entry->descriptor.provider_module_id = entry->provider_module_id;
    registry->count += 1U;
    umi_capability_lock_leave(&registry->lock);
    return UMI_STATUS_OK;
}

/*
 * Remove capability registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_capability_registry_unregister(
    UmiCapabilityRegistry *registry,
    const char *capability_id,
    const char *provider_module_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || capability_id == NULL ||
        provider_module_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_capability_lock_enter(&registry->lock);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_capability_matches(&registry->entries[index],
                                   capability_id,
                                   provider_module_id)) {
            size_t move_count = registry->count - index - 1U;
            /* Apply this branch only when its contract condition is satisfied. */
            if (move_count > 0U) {
                (void)memmove(&registry->entries[index],
                              &registry->entries[index + 1U],
                              move_count * sizeof(registry->entries[0]));
            }
            registry->count -= 1U;
            umi_capability_lock_leave(&registry->lock);
            return UMI_STATUS_OK;
        }
    }
    umi_capability_lock_leave(&registry->lock);
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find capability registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiCapabilityDescriptor *umi_capability_registry_find(
    const UmiCapabilityRegistry *registry,
    const char *capability_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || capability_id == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->entries[index].capability_id,
                   capability_id) == 0) {
            return &registry->entries[index].descriptor;
        }
    }
    return NULL;
}

/*
 * Provide the capability registry find provider operation used by this module and its
 * client applications.
 */
const UmiCapabilityDescriptor *umi_capability_registry_find_provider(
    const UmiCapabilityRegistry *registry,
    const char *capability_id,
    const char *provider_module_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || capability_id == NULL ||
        provider_module_id == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (umi_capability_matches(&registry->entries[index],
                                   capability_id,
                                   provider_module_id)) {
            return &registry->entries[index].descriptor;
        }
    }
    return NULL;
}

/*
 * Provide the capability registry require operation used by this module and its client
 * applications.
 */
UmiStatus umi_capability_registry_require(
    const UmiCapabilityRegistry *registry,
    const char *const *required_capabilities,
    const char **out_missing_capability)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_missing_capability != NULL) {
        *out_missing_capability = NULL;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (required_capabilities == NULL) {
        return UMI_STATUS_OK;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; required_capabilities[index] != NULL; ++index) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (umi_capability_registry_find(registry,
                                         required_capabilities[index]) == NULL) {
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (out_missing_capability != NULL) {
                *out_missing_capability = required_capabilities[index];
            }
            return UMI_STATUS_NOT_FOUND;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by capability registry without changing their
 * state.
 */
size_t umi_capability_registry_count(
    const UmiCapabilityRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Find capability registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiCapabilityDescriptor *umi_capability_registry_at(
    const UmiCapabilityRegistry *registry,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || index >= registry->count) {
        return NULL;
    }
    return &registry->entries[index].descriptor;
}
