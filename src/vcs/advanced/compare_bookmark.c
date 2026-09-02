/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/compare_bookmark.c
 *
 * PURPOSE:
 *   Persist named source locations inside long comparison sessions.
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
#include "umicom/vcs/advanced/compare_bookmark.h"

#include <string.h>

/*
 * Initialise vcs advanced compare bookmark from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_compare_bookmark_init(UmiVcsAdvancedCompareBookmark *value)
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
 * Check that vcs advanced compare bookmark satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_compare_bookmark_validate(const UmiVcsAdvancedCompareBookmark *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->path) || value->line_number == 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Copy vcs advanced compare bookmark into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiStatus umi_vcs_advanced_compare_bookmark_set(UmiVcsAdvancedCompareBookmark *value,
                                                  const char *path,
                                                  const char *label,
                                                  size_t line_number,
                                                  size_t change_index)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || line_number == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->path, sizeof(value->path), path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->label, sizeof(value->label), label);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->line_number = line_number;
    value->change_index = change_index;
    value->fingerprint = umi_vcs_advanced_hash_text(path) ^ (uint64_t)line_number;
    return umi_vcs_advanced_compare_bookmark_validate(value);
}
