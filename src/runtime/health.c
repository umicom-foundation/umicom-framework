/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/health.c
 *
 * PURPOSE:
 *   Implement thread-safe component-health updates, snapshots and aggregate
 *   health calculation for Framework supervisors and product workbenches.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

static size_t umi_health_find_index(const UmiHealthRegistry *registry,
                                    const char *component_id)
{
    size_t index;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->entries[index].component_id,
                   component_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}

UmiStatus umi_health_registry_create(UmiHealthRegistry **out_registry)
{
    UmiHealthRegistry *registry;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiHealthRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (umi_mutex_create(&registry->mutex) != UMI_STATUS_OK) {
        free(registry);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_health_registry_destroy(UmiHealthRegistry *registry)
{
    if (registry == NULL) return;
    umi_mutex_destroy(registry->mutex);
    free(registry);
}

UmiStatus umi_health_registry_update(UmiHealthRegistry *registry,
                                     const char *component_id,
                                     UmiHealthState state,
                                     const char *message,
                                     uint64_t updated_at_ns)
{
    size_t index;
    UmiHealthSnapshot *entry;
    if (registry == NULL || component_id == NULL || component_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strlen(component_id) >= UMI_HEALTH_COMPONENT_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)umi_mutex_lock(registry->mutex);
    index = umi_health_find_index(registry, component_id);
    if (index == SIZE_MAX) {
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

UmiStatus umi_health_registry_get(const UmiHealthRegistry *registry,
                                  const char *component_id,
                                  UmiHealthSnapshot *out_snapshot)
{
    size_t index;
    UmiHealthRegistry *mutable_registry;

    if (registry == NULL || component_id == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    mutable_registry = (UmiHealthRegistry *)registry;
    (void)umi_mutex_lock(mutable_registry->mutex);
    index = umi_health_find_index(registry, component_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(mutable_registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_snapshot = registry->entries[index];
    (void)umi_mutex_unlock(mutable_registry->mutex);
    return UMI_STATUS_OK;
}

size_t umi_health_registry_count(const UmiHealthRegistry *registry)
{
    size_t count;
    UmiHealthRegistry *mutable_registry;

    if (registry == NULL) {
        return 0U;
    }
    mutable_registry = (UmiHealthRegistry *)registry;
    (void)umi_mutex_lock(mutable_registry->mutex);
    count = registry->count;
    (void)umi_mutex_unlock(mutable_registry->mutex);
    return count;
}

UmiStatus umi_health_registry_at(const UmiHealthRegistry *registry,
                                 size_t index,
                                 UmiHealthSnapshot *out_snapshot)
{
    UmiHealthRegistry *mutable_registry;

    if (registry == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    mutable_registry = (UmiHealthRegistry *)registry;
    (void)umi_mutex_lock(mutable_registry->mutex);
    if (index >= registry->count) {
        (void)umi_mutex_unlock(mutable_registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_snapshot = registry->entries[index];
    (void)umi_mutex_unlock(mutable_registry->mutex);
    return UMI_STATUS_OK;
}

UmiHealthState umi_health_registry_overall(const UmiHealthRegistry *registry)
{
    size_t index;
    UmiHealthState overall = UMI_HEALTH_UNKNOWN;
    UmiHealthRegistry *mutable_registry;

    if (registry == NULL) {
        return overall;
    }
    mutable_registry = (UmiHealthRegistry *)registry;
    (void)umi_mutex_lock(mutable_registry->mutex);
    if (registry->count == 0U) {
        (void)umi_mutex_unlock(mutable_registry->mutex);
        return overall;
    }

    overall = UMI_HEALTH_READY;
    for (index = 0U; index < registry->count; ++index) {
        UmiHealthState state = registry->entries[index].state;
        if (state == UMI_HEALTH_FAILED) {
            overall = UMI_HEALTH_FAILED;
            break;
        }
        if (state == UMI_HEALTH_DEGRADED) {
            overall = UMI_HEALTH_DEGRADED;
        } else if ((state == UMI_HEALTH_STARTING ||
                    state == UMI_HEALTH_STOPPING) &&
                   overall == UMI_HEALTH_READY) {
            overall = state;
        } else if (state == UMI_HEALTH_UNKNOWN) {
            overall = UMI_HEALTH_UNKNOWN;
        }
    }
    (void)umi_mutex_unlock(mutable_registry->mutex);
    return overall;
}

const char *umi_health_state_text(UmiHealthState state)
{
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
