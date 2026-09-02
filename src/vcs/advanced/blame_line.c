/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/blame_line.c
 *
 * PURPOSE:
 *   Implement line-level provenance without leaking Git parser details into applications.
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
#include "umicom/vcs/advanced/blame_line.h"

#include <string.h>

/*
 * Initialise vcs advanced blame line from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_blame_line_init(UmiVcsAdvancedBlameLine *value)
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
 * Check that vcs advanced blame line satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_blame_line_validate(const UmiVcsAdvancedBlameLine *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->line_number == 0U || !umi_vcs_advanced_text_present(value->commit_oid))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Copy vcs advanced blame line into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_blame_line_set(UmiVcsAdvancedBlameLine *value,
                                            size_t line_number,
                                            const char *commit_oid,
                                            const char *author,
                                            const char *source_path,
                                            const char *content)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || line_number == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    value->line_number = line_number;
    value->original_line_number = line_number;
    status = umi_vcs_advanced_copy_text(value->commit_oid, sizeof(value->commit_oid), commit_oid);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->author, sizeof(value->author), author);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->source_path, sizeof(value->source_path), source_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->content_fingerprint = umi_vcs_advanced_hash_text(content);
    return umi_vcs_advanced_blame_line_validate(value);
}
