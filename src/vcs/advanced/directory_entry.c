/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/directory_entry.c
 *
 * PURPOSE:
 *   Implement one filesystem entry for deterministic directory comparison.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/directory_entry.h"

#include <string.h>

void umi_vcs_advanced_directory_entry_init(UmiVcsAdvancedDirectoryEntry *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_directory_entry_validate(const UmiVcsAdvancedDirectoryEntry *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->relative_path))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_directory_entry_set(UmiVcsAdvancedDirectoryEntry *value,
                                                 const char *relative_path,
                                                 uint64_t size_bytes,
                                                 uint64_t content_fingerprint,
                                                 int directory)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->relative_path, sizeof(value->relative_path), relative_path);
    if (status != UMI_STATUS_OK) return status;
    value->size_bytes = size_bytes;
    value->content_fingerprint = content_fingerprint;
    value->directory = directory != 0;
    return umi_vcs_advanced_directory_entry_validate(value);
}
