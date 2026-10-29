/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/platform/recent_items_pin.inc
 *
 * PURPOSE:
 *   Commit a recent-item pin decision without exposing a partially saved model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
UmiStatus UmiRecentItemsSetPinnedLocal(UmiRecentItemRegistry *registry,
    const char *id, uint64_t expectedItemRevision, int pinned, const char *metadataPath)
{
    if (registry == NULL || id == NULL || id[0] == '\0' ||
        expectedItemRevision == 0U || metadataPath == NULL ||
        !umi_fs_is_absolute(metadataPath)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = find_index(registry, id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (registry->items[index].revision != expectedItemRevision)
        return UMI_STATUS_INVALID_STATE;
    if (registry->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Persist the complete candidate so other pending history changes are
     * retained. Borrowers keep the live registry address and see the new pin
     * only after the replacement succeeds. Resource URIs are never opened. */
    UmiRecentItemRegistry *candidate = malloc(sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *candidate = *registry;
    UmiStatus status = umi_platform_recent_items_registry_set_pinned(candidate, id, pinned);
    if (status == UMI_STATUS_OK) status = UmiRecentItemsSaveLocal(candidate, metadataPath);
    if (status == UMI_STATUS_OK) *registry = *candidate;
    free(candidate);
    return status;
}
