/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/service_registry_ownership.inc
 *
 * PURPOSE:
 *   Preserve owned service lifetimes across replacement, removal and callbacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* A copied entry contains pointers into its former storage. Always bind the
 * public descriptor to the identifier arrays in the copy being published. */
static void ServiceRebind(UmiServiceEntry *entry)
{
    entry->descriptor.service_id = entry->service_id;
    entry->descriptor.provider_module_id = entry->provider_module_id;
}

static int ServiceHasDestructor(const UmiServiceEntry *entry)
{
    return (entry->descriptor.flags & UMI_SERVICE_OWNED) != 0U &&
        entry->descriptor.destroy != NULL && entry->descriptor.service != NULL;
}

/* Destructors execute after releasing the write mutex. The atomic flag rejects
 * mutation/release during that interval, including reentry from the destructor.
 * Read-only callbacks still require the same external ownership discipline as
 * the public borrowed-pointer lookup APIs. */
static void ServiceDestroyRetired(UmiServiceRegistry *registry,
                                  UmiServiceEntry *retired)
{
    umi_service_entry_destroy(retired);
    atomic_store_explicit(&registry->callback_active, 0, memory_order_release);
}

UmiStatus umi_service_registry_register(UmiServiceRegistry *registry,
                                        const UmiServiceDescriptor *descriptor)
{
    if (registry == NULL || descriptor == NULL ||
        descriptor->structure_size < sizeof(*descriptor) ||
        descriptor->service_id == NULL || descriptor->service_id[0] == '\0' ||
        descriptor->provider_module_id == NULL || descriptor->provider_module_id[0] == '\0' ||
        descriptor->service == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (strlen(descriptor->service_id) >= UMI_SERVICE_ID_CAPACITY ||
        strlen(descriptor->provider_module_id) >= UMI_SERVICE_PROVIDER_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Snapshot before changing or destroying an existing entry. The input may
     * be a descriptor borrowed from this registry or strings owned by the
     * retiring object. The stored entry never points at these stack arrays. */
    UmiServiceEntry incoming = {0};
    incoming.descriptor = *descriptor;
    (void)snprintf(incoming.service_id, sizeof(incoming.service_id),
                   "%s", descriptor->service_id);
    (void)snprintf(incoming.provider_module_id, sizeof(incoming.provider_module_id),
                   "%s", descriptor->provider_module_id);
    ServiceRebind(&incoming);

    UmiStatus status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) return status;
    if (atomic_load_explicit(&registry->callback_active, memory_order_acquire)) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    size_t replacement = SIZE_MAX;
    for (size_t index = 0U; index < registry->count; ++index) {
        UmiServiceEntry *existing = &registry->entries[index];
        if (strcmp(existing->service_id, incoming.service_id) != 0) continue;
        if (strcmp(existing->provider_module_id, incoming.provider_module_id) == 0) {
            if ((existing->descriptor.flags & UMI_SERVICE_REPLACEABLE) == 0U) {
                (void)umi_mutex_unlock(registry->mutex);
                return UMI_STATUS_ALREADY_EXISTS;
            }
            replacement = index;
        } else if (((existing->descriptor.flags | incoming.descriptor.flags) &
                    UMI_SERVICE_SINGLETON) != 0U) {
            (void)umi_mutex_unlock(registry->mutex);
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (replacement == SIZE_MAX && registry->count >= UMI_SERVICE_REGISTRY_MAX) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    UmiServiceEntry retired = {0};
    UmiServiceEntry *destination;
    int destroy_retired = 0;
    if (replacement != SIZE_MAX) {
        destination = &registry->entries[replacement];
        retired = *destination;
        ServiceRebind(&retired);
        /* Replacing metadata for the same object does not end that object's
         * lifetime. New ownership flags and destructor apply to its next removal. */
        destroy_retired = retired.descriptor.service != incoming.descriptor.service &&
                          ServiceHasDestructor(&retired);
    } else destination = &registry->entries[registry->count++];
    *destination = incoming;
    ServiceRebind(destination);
    if (destroy_retired)
        atomic_store_explicit(&registry->callback_active, 1, memory_order_release);
    (void)umi_mutex_unlock(registry->mutex);
    if (destroy_retired) ServiceDestroyRetired(registry, &retired);
    return UMI_STATUS_OK;
}

UmiStatus umi_service_registry_unregister(UmiServiceRegistry *registry,
                                          const char *service_id,
                                          const char *provider_module_id)
{
    if (registry == NULL || service_id == NULL || provider_module_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) return status;
    if (atomic_load_explicit(&registry->callback_active, memory_order_acquire)) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    for (size_t index = 0U; index < registry->count; ++index) {
        if (!umi_service_matches(&registry->entries[index], service_id, provider_module_id))
            continue;
        UmiServiceEntry retired = registry->entries[index];
        ServiceRebind(&retired);
        size_t remaining = registry->count - index - 1U;
        if (remaining != 0U)
            (void)memmove(&registry->entries[index], &registry->entries[index + 1U],
                          remaining * sizeof(registry->entries[0]));
        --registry->count;
        for (size_t moved = index; moved < registry->count; ++moved)
            ServiceRebind(&registry->entries[moved]);
        (void)memset(&registry->entries[registry->count], 0, sizeof(registry->entries[0]));
        int destroy_retired = ServiceHasDestructor(&retired);
        if (destroy_retired)
            atomic_store_explicit(&registry->callback_active, 1, memory_order_release);
        (void)umi_mutex_unlock(registry->mutex);
        if (destroy_retired) ServiceDestroyRetired(registry, &retired);
        return UMI_STATUS_OK;
    }
    (void)umi_mutex_unlock(registry->mutex);
    return UMI_STATUS_NOT_FOUND;
}

UmiStatus UmiServiceRegistryRelease(UmiServiceRegistry **inOutRegistry)
{
    if (inOutRegistry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiServiceRegistry *registry = *inOutRegistry;
    if (registry == NULL) return UMI_STATUS_OK;
    UmiStatus status = umi_mutex_lock(registry->mutex);
    if (status != UMI_STATUS_OK) return status;
    if (atomic_load_explicit(&registry->callback_active, memory_order_acquire)) {
        (void)umi_mutex_unlock(registry->mutex);
        return UMI_STATUS_INVALID_STATE;
    }
    atomic_store_explicit(&registry->callback_active, 1, memory_order_release);
    (void)umi_mutex_unlock(registry->mutex);
    /* The owner has excluded other concurrent operations for release. Detach each service
     * before its callback, so callback discovery cannot return the retiring one.
     * Keep the guard set for the entire reverse-order destruction sequence. */
    while (registry->count != 0U) {
        UmiServiceEntry retired = registry->entries[--registry->count];
        ServiceRebind(&retired);
        (void)memset(&registry->entries[registry->count], 0, sizeof(registry->entries[0]));
        umi_service_entry_destroy(&retired);
    }
    umi_mutex_destroy(registry->mutex);
    free(registry);
    *inOutRegistry = NULL;
    return UMI_STATUS_OK;
}

void umi_service_registry_destroy(UmiServiceRegistry *registry)
{
    /* Existing callers retain the registry when a destructor reenters release. */
    (void)UmiServiceRegistryRelease(&registry);
}

int UmiServiceRegistryIsInCallback(const UmiServiceRegistry *registry)
{
    return registry != NULL && atomic_load_explicit(
        &registry->callback_active, memory_order_acquire) != 0;
}
