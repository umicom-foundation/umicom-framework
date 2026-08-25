/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/directory_entry.h
 *
 * PURPOSE:
 *   Describe one filesystem entry for deterministic directory comparison.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_DIRECTORY_ENTRY_H
#define UMICOM_VCS_ADVANCED_DIRECTORY_ENTRY_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsAdvancedDirectoryEntry {
    uint32_t struct_size;
    uint32_t api_version;
    char relative_path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    uint64_t size_bytes;
    uint64_t content_fingerprint;
    uint64_t modified_seconds;
    int directory;
    int symlink;
} UmiVcsAdvancedDirectoryEntry;

void umi_vcs_advanced_directory_entry_init(UmiVcsAdvancedDirectoryEntry *value);
UmiStatus umi_vcs_advanced_directory_entry_validate(const UmiVcsAdvancedDirectoryEntry *value);
UmiStatus umi_vcs_advanced_directory_entry_set(UmiVcsAdvancedDirectoryEntry *value,
                                                 const char *relative_path,
                                                 uint64_t size_bytes,
                                                 uint64_t content_fingerprint,
                                                 int directory);

#ifdef __cplusplus
}
#endif

#endif
