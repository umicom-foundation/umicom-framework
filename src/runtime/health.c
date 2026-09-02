/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/health.c
 *
 * PURPOSE:
 *   Implement thread-safe component-health updates, snapshots and aggregate
 *   health calculation for Framework supervisors and product workbenches.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/health.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/platform/threading.h"

struct UmiHealthRegistry {
    UmiHealthSnapshot entries[UMI_HEALTH_REGISTRY_MAX];
    size_t count;
    uint64_t sequence;
    UmiMutex *mutex;
};

/* Provide the health find index operation used by this module and its client applications. */
static size_t umi_health_find_index(const UmiHealthRegistry *registry,
                                    const char *component_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->entries[index].component_id,
                   component_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

/*
 * Initialise health registry from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_health_registry_create(UmiHealthRegistry **out_registry)
{
    UmiHealthRegistry *registry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiHealthRegistry *)calloc(1U, sizeof(*registry));
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

/* Release or reset state held by health registry so the same storage can be reused safely. */
void umi_health_registry_destroy(UmiHealthRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    umi_mutex_destroy(registry->mutex);
    free(registry);
}

/*
 * Provide the health registry update operation used by this module and its client
 * applications.
 */
UmiStatus umi_health_registry_update(UmiHealthRegistry *registry,
                                     const char *component_id,
                                     UmiHealthState state,
                                     const char *message,
                                     uint64_t updated_at_ns)
{
    size_t index;
    UmiHealthSnapshot *entry;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || component_id == NULL || component_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(component_id) >= UMI_HEALTH_COMPONENT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)umi_mutex_lock(registry->mutex);
    index = umi_health_find_index(registry, component_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->count >= UMI_HEALTH_REGISTRY_MAX) {
            (void)umi_mutex_unlock(registry->mutex);
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = registry->count++;
    }
    entry = &registry->entries[index];
    (void)memset(entry, 0, sizeof(*entry));
    (void)snprintf(entry->component_id,
                   sizeof(entry->component_id),
                   "%s",
                   component_id);
    entry->state = state;
    (void)snprintf(entry->message,
                   sizeof(entry->message),
                   "%s",
                   message != NULL ? message : "");
    entry->updated_at_ns = updated_at_ns;
    entry->sequence = ++registry->sequence;
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the health registry get operation used by this module and its client
 * applications.
 */
UmiStatus umi_health_registry_get(const UmiHealthRegistry *registry,
                                  const char *component_id,
                                  UmiHealthSnapshot *out_snapshot)
{
    size_t index;
    UmiHealthRegistry *mutable_registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || component_id == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    mutable_registry = (UmiHealthRegistry *)registry;
    (void)umi_mutex_lock(mutable_registry->mutex);
    index = umi_health_find_index(registry, component_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(mutable_registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_snapshot = registry->entries[index];
    (void)umi_mutex_unlock(mutable_registry->mutex);
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by health registry without changing their
 * state.
 */
size_t umi_health_registry_count(const UmiHealthRegistry *registry)
{
    size_t count;
    UmiHealthRegistry *mutable_registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) {
        return 0U;
    }
    mutable_registry = (UmiHealthRegistry *)registry;
    (void)umi_mutex_lock(mutable_registry->mutex);
    count = registry->count;
    (void)umi_mutex_unlock(mutable_registry->mutex);
    return count;
}

/*
 * Find health registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_health_registry_at(const UmiHealthRegistry *registry,
                                 size_t index,
                                 UmiHealthSnapshot *out_snapshot)
{
    UmiHealthRegistry *mutable_registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    mutable_registry = (UmiHealthRegistry *)registry;
    (void)umi_mutex_lock(mutable_registry->mutex);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) {
        (void)umi_mutex_unlock(mutable_registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_snapshot = registry->entries[index];
    (void)umi_mutex_unlock(mutable_registry->mutex);
    return UMI_STATUS_OK;
}

/*
 * Provide the health registry overall operation used by this module and its client
 * applications.
 */
UmiHealthState umi_health_registry_overall(const UmiHealthRegistry *registry)
{
    size_t index;
    UmiHealthState overall = UMI_HEALTH_UNKNOWN;
    UmiHealthRegistry *mutable_registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) {
        return overall;
    }
    mutable_registry = (UmiHealthRegistry *)registry;
    (void)umi_mutex_lock(mutable_registry->mutex);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count == 0U) {
        (void)umi_mutex_unlock(mutable_registry->mutex);
        return overall;
    }

    overall = UMI_HEALTH_READY;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        UmiHealthState state = registry->entries[index].state;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (state == UMI_HEALTH_FAILED) {
            overall = UMI_HEALTH_FAILED;
            break;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (state == UMI_HEALTH_DEGRADED) {
            overall = UMI_HEALTH_DEGRADED;
        } else /* Apply this branch only when its contract condition is satisfied. */ if ((state == UMI_HEALTH_STARTING ||
                    state == UMI_HEALTH_STOPPING) &&
                   overall == UMI_HEALTH_READY) {
            overall = state;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (state == UMI_HEALTH_UNKNOWN) {
            overall = UMI_HEALTH_UNKNOWN;
        }
    }
    (void)umi_mutex_unlock(mutable_registry->mutex);
    return overall;
}

/* Provide the health state text operation used by this module and its client applications. */
const char *umi_health_state_text(UmiHealthState state)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (state) {
        case UMI_HEALTH_UNKNOWN: return "unknown";
        case UMI_HEALTH_STARTING: return "starting";
        case UMI_HEALTH_READY: return "ready";
        case UMI_HEALTH_DEGRADED: return "degraded";
        case UMI_HEALTH_STOPPING: return "stopping";
        case UMI_HEALTH_STOPPED: return "stopped";
        case UMI_HEALTH_FAILED: return "failed";
        default: return "invalid";
    }
}
