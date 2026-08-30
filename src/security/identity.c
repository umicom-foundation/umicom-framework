/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/identity.c
 *
 * PURPOSE:
 *   Implement thread-safe identity registration, lookup and enablement state
 *   without retaining caller-owned strings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/identity.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/platform/threading.h"
struct UmiIdentityRegistry {
    UmiIdentityRecord items[UMI_IDENTITY_REGISTRY_MAX];
    size_t count;
    uint64_t next_revision;
    UmiMutex *mutex;
};
static size_t identity_find(const UmiIdentityRegistry *registry,
                            const char *identity_id)
{
    size_t index;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].identity_id, identity_id) == 0) {
            return index;
        }
    }
    return SIZE_MAX;
}
UmiStatus umi_identity_registry_create(UmiIdentityRegistry **out_registry)
{
    UmiIdentityRegistry *registry;
    UmiStatus status;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiIdentityRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&registry->mutex);
    if (status != UMI_STATUS_OK) { free(registry); return status; }
    registry->next_revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}
void umi_identity_registry_destroy(UmiIdentityRegistry *registry)
{
    if (registry == NULL) return;
    umi_mutex_destroy(registry->mutex);
    free(registry);
}
UmiStatus umi_identity_registry_register(UmiIdentityRegistry *registry,
                                         const UmiIdentityRecord *identity)
{
    UmiIdentityRecord *stored;
    UmiStatus status;
    if (registry == NULL || identity == NULL || identity->identity_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strlen(identity->identity_id) >= UMI_IDENTITY_ID_CAPACITY ||
        strlen(identity->display_name) >= UMI_IDENTITY_DISPLAY_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) return status;
    if (identity_find(registry, identity->identity_id) != SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (registry->count >= UMI_IDENTITY_REGISTRY_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    stored = &registry->items[registry->count++];
    *stored = *identity;
    stored->enabled = identity->enabled != 0;
    stored->revision = registry->next_revision++;
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}
UmiStatus umi_identity_registry_get(const UmiIdentityRegistry *registry,
                                    const char *identity_id,
                                    UmiIdentityRecord *out_identity)
{
    size_t index;
    UmiStatus status;
    if (registry == NULL || identity_id == NULL || out_identity == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) return status;
    index = identity_find(registry, identity_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_identity = registry->items[index];
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}
UmiStatus umi_identity_registry_set_enabled(UmiIdentityRegistry *registry,
                                            const char *identity_id,
                                            int enabled)
{
    size_t index;
    UmiStatus status;
    if (registry == NULL || identity_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) return status;
    index = identity_find(registry, identity_id);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    registry->items[index].enabled = enabled != 0;
    registry->items[index].revision = registry->next_revision++;
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}
size_t umi_identity_registry_count(const UmiIdentityRegistry *registry)
{
    size_t count = 0U;
    if (registry == NULL) return 0U;
    if (umi_mutex_lock(registry->mutex) == UMI_STATUS_OK) {
        count = registry->count;
        (void)umi_mutex_unlock(registry->mutex);
    }
    return count;
}
UmiStatus umi_identity_registry_at(const UmiIdentityRegistry *registry,
                                   size_t index,
                                   UmiIdentityRecord *out_identity)
{
    UmiStatus status;
    if (registry == NULL || out_identity == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) return status;
    if (index >= registry->count) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    *out_identity = registry->items[index];
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_OK;
}
