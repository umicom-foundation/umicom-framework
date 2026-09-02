/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/diff_document.c
 *
 * PURPOSE:
 *   Implement one compared document and aggregate change statistics.
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
#include "umicom/vcs/advanced/diff_document.h"

#include <string.h>

/*
 * Initialise vcs advanced diff document from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_diff_document_init(UmiVcsAdvancedDiffDocument *value)
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
 * Check that vcs advanced diff document satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_diff_document_validate(const UmiVcsAdvancedDiffDocument *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->left_path) || !umi_vcs_advanced_text_present(value->right_path))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced diff document set paths operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_diff_document_set_paths(UmiVcsAdvancedDiffDocument *value,
                                                     const char *left_path,
                                                     const char *right_path,
                                                     UmiVcsCompareMode mode)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->left_path, sizeof(value->left_path), left_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->right_path, sizeof(value->right_path), right_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->mode = mode;
    return umi_vcs_advanced_diff_document_validate(value);
}
/*
 * Return the number of records represented by vcs advanced diff document change without
 * changing their state.
 */
size_t umi_vcs_advanced_diff_document_change_count(const UmiVcsAdvancedDiffDocument *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return 0U;
    return value->added_lines + value->deleted_lines + value->modified_lines;
}
