/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/stash_catalogue.c
 *
 * PURPOSE:
 *   Maintain a bounded, ordered stash catalogue for Source Control Centre.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/stash_catalogue.h"
#include <string.h>
void umi_vcs_advanced_stash_catalogue_init(UmiVcsAdvancedStashCatalogue *catalogue)
{
    if (catalogue == NULL) return;
    (void)memset(catalogue, 0, sizeof(*catalogue));
    catalogue->struct_size = (uint32_t)sizeof(*catalogue);
    catalogue->api_version = UMI_VCS_ADVANCED_API_VERSION;
    catalogue->revision = 1U;
}
const UmiVcsAdvancedStashEntry *umi_vcs_advanced_stash_catalogue_find_ref(
    const UmiVcsAdvancedStashCatalogue *catalogue, const char *stash_ref)
{
    size_t i;
    if (catalogue == NULL || stash_ref == NULL) return NULL;
    for (i = 0U; i < catalogue->count; ++i)
        if (umi_vcs_advanced_text_equal(catalogue->entries[i].stash_ref, stash_ref)) return &catalogue->entries[i];
    return NULL;
}
UmiStatus umi_vcs_advanced_stash_catalogue_add(UmiVcsAdvancedStashCatalogue *catalogue,
                                                const UmiVcsAdvancedStashEntry *entry)
{
    if (catalogue == NULL || umi_vcs_advanced_stash_entry_validate(entry) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_vcs_advanced_stash_catalogue_find_ref(catalogue, entry->stash_ref) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (catalogue->count >= UMI_VCS_ADVANCED_LIST_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->entries[catalogue->count++] = *entry;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}
const UmiVcsAdvancedStashEntry *umi_vcs_advanced_stash_catalogue_at(
    const UmiVcsAdvancedStashCatalogue *catalogue, size_t index)
{
    return catalogue != NULL && index < catalogue->count ? &catalogue->entries[index] : NULL;
}
