/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/recent_items_remove.inc
 *
 * PURPOSE:
 *   Persist a bounded recent-item removal before changing the live catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
UmiStatus UmiRecentItemsRemoveLocal(UmiRecentItemRegistry *registry,
    const char *id, uint64_t expectedItemRevision, const char *metadataPath)
{
    if (registry == NULL || id == NULL || id[0] == '\0' ||
        expectedItemRevision == 0U || metadataPath == NULL ||
        !umi_fs_is_absolute(metadataPath)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (registry->items[index].revision != expectedItemRevision)
        return UMI_STATUS_INVALID_STATE;
    if (registry->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Borrowers keep the same live registry address. The complete bounded
     * candidate exists only for this transaction, so a failed save cannot
     * remove an item from memory while leaving it in the preferences file. */
    UmiRecentItemRegistry *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *registry;
    UmiStatus status = umi_platform_recent_items_registry_remove(candidate, id);
    if (status == UMI_STATUS_OK) status = UmiRecentItemsSaveLocal(candidate, metadataPath);
    if (status == UMI_STATUS_OK) *registry = *candidate;
    free(candidate);
    return status;
}
