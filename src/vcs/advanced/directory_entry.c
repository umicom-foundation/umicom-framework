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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/directory_entry.h"

#include <string.h>

/*
 * Initialise vcs advanced directory entry from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_directory_entry_init(UmiVcsAdvancedDirectoryEntry *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

/*
 * Check that vcs advanced directory entry satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_directory_entry_validate(const UmiVcsAdvancedDirectoryEntry *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->relative_path))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Copy vcs advanced directory entry into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_directory_entry_set(UmiVcsAdvancedDirectoryEntry *value,
                                                 const char *relative_path,
                                                 uint64_t size_bytes,
                                                 uint64_t content_fingerprint,
                                                 int directory)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->relative_path, sizeof(value->relative_path), relative_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->size_bytes = size_bytes;
    value->content_fingerprint = content_fingerprint;
    value->directory = directory != 0;
    return umi_vcs_advanced_directory_entry_validate(value);
}
