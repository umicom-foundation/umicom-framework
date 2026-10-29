/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/service_registry_snapshots.inc
 *
 * PURPOSE:
 *   Copy service metadata for diagnostics without exposing borrowed storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
static void ServiceCopyMetadata(const UmiServiceEntry *entry,
                                UmiServiceSnapshot *out_snapshot)
{
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)memcpy(out_snapshot->service_id, entry->service_id,
                 sizeof(out_snapshot->service_id));
    (void)memcpy(out_snapshot->provider_module_id, entry->provider_module_id,
                 sizeof(out_snapshot->provider_module_id));
    out_snapshot->service_version = entry->descriptor.service_version;
    out_snapshot->flags = entry->descriptor.flags;
}

UmiStatus UmiServiceRegistryCopySnapshot(const UmiServiceRegistry *registry,
    const char *service_id, const char *provider_module_id,
    UmiServiceSnapshot *out_snapshot)
{
    if (registry == NULL || service_id == NULL || service_id[0] == '\0' ||
        (provider_module_id != NULL && provider_module_id[0] == '\0') ||
        out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiServiceRegistry *mutable_registry = (UmiServiceRegistry *)registry;
    UmiStatus status = umi_mutex_lock(mutable_registry->mutex);
    if (status != UMI_STATUS_OK) return status;
    status = UMI_STATUS_NOT_FOUND;
    for (size_t index = 0U; index < registry->count; ++index) {
        if (!umi_service_matches(&registry->entries[index], service_id, provider_module_id))
            continue;
        ServiceCopyMetadata(&registry->entries[index], out_snapshot);
        status = UMI_STATUS_OK;
        break;
    }
    (void)umi_mutex_unlock(mutable_registry->mutex);
    return status;
}

UmiStatus UmiServiceRegistryCopySnapshots(const UmiServiceRegistry *registry,
    UmiServiceSnapshot *out_snapshots, size_t capacity, size_t *out_total)
{
    if (out_total != NULL) *out_total = 0U;
    if (registry == NULL || out_total == NULL ||
        (capacity != 0U && out_snapshots == NULL)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiServiceRegistry *mutable_registry = (UmiServiceRegistry *)registry;
    UmiStatus status = umi_mutex_lock(mutable_registry->mutex);
    if (status != UMI_STATUS_OK) return status;
    /* One lock covers both list and count. There is no raw descriptor pointer
     * for a caller to retain while a writer compacts or replaces the registry. */
    size_t count = registry->count;
    size_t copied = capacity < count ? capacity : count;
    if (out_snapshots != NULL) {
        for (size_t index = 0U; index < copied; ++index)
            ServiceCopyMetadata(&registry->entries[index], &out_snapshots[index]);
        if (capacity < count) status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    *out_total = count;
    (void)umi_mutex_unlock(mutable_registry->mutex);
    return status;
}

UmiStatus UmiServiceRegistryCopySummary(const UmiServiceRegistry *registry,
    UmiServiceRegistrySummary *out_summary)
{
    if (registry == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiServiceRegistry *mutable_registry = (UmiServiceRegistry *)registry;
    UmiStatus status = umi_mutex_lock(mutable_registry->mutex);
    if (status != UMI_STATUS_OK) return status;
    UmiServiceRegistrySummary summary = {0};
    summary.service_count = registry->count;
    for (size_t index = 0U; index < registry->count; ++index) {
        uint32_t flags = registry->entries[index].descriptor.flags;
        if ((flags & UMI_SERVICE_SINGLETON) != 0U) ++summary.singleton_service_count;
        if ((flags & UMI_SERVICE_OWNED) != 0U) ++summary.owned_service_count;
        if ((flags & UMI_SERVICE_THREAD_SAFE) != 0U) ++summary.thread_safe_service_count;
        if ((flags & UMI_SERVICE_REPLACEABLE) != 0U) ++summary.replaceable_service_count;
    }
    *out_summary = summary;
    (void)umi_mutex_unlock(mutable_registry->mutex);
    return UMI_STATUS_OK;
}

UmiStatus UmiServiceRegistryCountPrefix(const UmiServiceRegistry *registry,
    const char *prefix, size_t *out_count)
{
    if (out_count != NULL) *out_count = 0U;
    if (registry == NULL || prefix == NULL || out_count == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t length = strlen(prefix);
    UmiServiceRegistry *mutable_registry = (UmiServiceRegistry *)registry;
    UmiStatus status = umi_mutex_lock(mutable_registry->mutex);
    if (status != UMI_STATUS_OK) return status;
    size_t count = 0U;
    for (size_t index = 0U; index < registry->count; ++index) {
        if (strncmp(registry->entries[index].service_id, prefix, length) == 0) ++count;
    }
    *out_count = count;
    (void)umi_mutex_unlock(mutable_registry->mutex);
    return UMI_STATUS_OK;
}
