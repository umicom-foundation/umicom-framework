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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/diff_document.h"

#include <string.h>

void umi_vcs_advanced_diff_document_init(UmiVcsAdvancedDiffDocument *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_diff_document_validate(const UmiVcsAdvancedDiffDocument *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->left_path) || !umi_vcs_advanced_text_present(value->right_path))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_diff_document_set_paths(UmiVcsAdvancedDiffDocument *value,
                                                     const char *left_path,
                                                     const char *right_path,
                                                     UmiVcsCompareMode mode)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->left_path, sizeof(value->left_path), left_path);
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->right_path, sizeof(value->right_path), right_path);
    if (status != UMI_STATUS_OK) return status;
    value->mode = mode;
    return umi_vcs_advanced_diff_document_validate(value);
}
size_t umi_vcs_advanced_diff_document_change_count(const UmiVcsAdvancedDiffDocument *value)
{
    if (value == NULL) return 0U;
    return value->added_lines + value->deleted_lines + value->modified_lines;
}
