/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/stash_catalogue.h
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
#ifndef UMICOM_VCS_ADVANCED_STASH_CATALOGUE_H
#define UMICOM_VCS_ADVANCED_STASH_CATALOGUE_H
#include "umicom/vcs/advanced/stash_entry.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs advanced stash catalogue data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedStashCatalogue {
    uint32_t struct_size;
    uint32_t api_version;
    UmiVcsAdvancedStashEntry entries[UMI_VCS_ADVANCED_LIST_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiVcsAdvancedStashCatalogue;
/**
 * Initialise vcs advanced stash catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_stash_catalogue_init(UmiVcsAdvancedStashCatalogue *catalogue);
/**
 * Add vcs advanced stash catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_vcs_advanced_stash_catalogue_add(UmiVcsAdvancedStashCatalogue *catalogue,
                                                const UmiVcsAdvancedStashEntry *entry);
/**
 * Find vcs advanced stash catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiVcsAdvancedStashEntry *umi_vcs_advanced_stash_catalogue_at(
    const UmiVcsAdvancedStashCatalogue *catalogue, size_t index);
/**
 * Provide the vcs advanced stash catalogue find ref operation used by this module and its
 * client applications.
 */
const UmiVcsAdvancedStashEntry *umi_vcs_advanced_stash_catalogue_find_ref(
    const UmiVcsAdvancedStashCatalogue *catalogue, const char *stash_ref);
#ifdef __cplusplus
}
#endif
#endif
