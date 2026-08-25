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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_STASH_CATALOGUE_H
#define UMICOM_VCS_ADVANCED_STASH_CATALOGUE_H
#include "umicom/vcs/advanced/stash_entry.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsAdvancedStashCatalogue {
    uint32_t struct_size;
    uint32_t api_version;
    UmiVcsAdvancedStashEntry entries[UMI_VCS_ADVANCED_LIST_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiVcsAdvancedStashCatalogue;
void umi_vcs_advanced_stash_catalogue_init(UmiVcsAdvancedStashCatalogue *catalogue);
UmiStatus umi_vcs_advanced_stash_catalogue_add(UmiVcsAdvancedStashCatalogue *catalogue,
                                                const UmiVcsAdvancedStashEntry *entry);
const UmiVcsAdvancedStashEntry *umi_vcs_advanced_stash_catalogue_at(
    const UmiVcsAdvancedStashCatalogue *catalogue, size_t index);
const UmiVcsAdvancedStashEntry *umi_vcs_advanced_stash_catalogue_find_ref(
    const UmiVcsAdvancedStashCatalogue *catalogue, const char *stash_ref);
#ifdef __cplusplus
}
#endif
#endif
