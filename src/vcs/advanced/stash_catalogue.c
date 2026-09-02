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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/stash_catalogue.h"
#include <string.h>
/*
 * Initialise vcs advanced stash catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_stash_catalogue_init(UmiVcsAdvancedStashCatalogue *catalogue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return;
    (void)memset(catalogue, 0, sizeof(*catalogue));
    catalogue->struct_size = (uint32_t)sizeof(*catalogue);
    catalogue->api_version = UMI_VCS_ADVANCED_API_VERSION;
    catalogue->revision = 1U;
}
/*
 * Provide the vcs advanced stash catalogue find ref operation used by this module and its
 * client applications.
 */
const UmiVcsAdvancedStashEntry *umi_vcs_advanced_stash_catalogue_find_ref(
    const UmiVcsAdvancedStashCatalogue *catalogue, const char *stash_ref)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || stash_ref == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < catalogue->count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_vcs_advanced_text_equal(catalogue->entries[i].stash_ref, stash_ref)) return &catalogue->entries[i];
    return NULL;
}
/*
 * Add vcs advanced stash catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_vcs_advanced_stash_catalogue_add(UmiVcsAdvancedStashCatalogue *catalogue,
                                                const UmiVcsAdvancedStashEntry *entry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || umi_vcs_advanced_stash_entry_validate(entry) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_vcs_advanced_stash_catalogue_find_ref(catalogue, entry->stash_ref) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count >= UMI_VCS_ADVANCED_LIST_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    catalogue->entries[catalogue->count++] = *entry;
    catalogue->revision += 1U;
    return UMI_STATUS_OK;
}
/*
 * Find vcs advanced stash catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiVcsAdvancedStashEntry *umi_vcs_advanced_stash_catalogue_at(
    const UmiVcsAdvancedStashCatalogue *catalogue, size_t index)
{
    return catalogue != NULL && index < catalogue->count ? &catalogue->entries[index] : NULL;
}
