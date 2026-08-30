/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/directory_diff.c
 *
 * PURPOSE:
 *   Implement deterministic directory-entry comparison state.
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

#include "umicom/vcs/advanced/directory_diff.h"

#include <string.h>

UmiStatus umi_vcs_advanced_directory_diff_compare(
    const UmiVcsAdvancedDirectoryEntry *left,
    const UmiVcsAdvancedDirectoryEntry *right,
    UmiVcsAdvancedDirectoryDiff *out_diff)
{
    const char *path;
    UmiStatus status;

    if (out_diff == NULL || (left == NULL && right == NULL)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_diff, 0, sizeof(*out_diff));
    out_diff->struct_size = (uint32_t)sizeof(*out_diff);
    out_diff->api_version = UMI_VCS_ADVANCED_API_VERSION;

    if (left != NULL &&
        umi_vcs_advanced_directory_entry_validate(left) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (right != NULL &&
        umi_vcs_advanced_directory_entry_validate(right) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    path = left != NULL ? left->relative_path : right->relative_path;
    status = umi_vcs_advanced_copy_text(
        out_diff->relative_path, sizeof(out_diff->relative_path), path);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    if (left != NULL) {
        out_diff->left = *left;
        out_diff->has_left = 1;
    }
    if (right != NULL) {
        out_diff->right = *right;
        out_diff->has_right = 1;
    }

    if (left == NULL) {
        out_diff->state = UMI_VCS_DIRECTORY_RIGHT_ONLY;
    } else if (right == NULL) {
        out_diff->state = UMI_VCS_DIRECTORY_LEFT_ONLY;
    } else if (left->directory != right->directory ||
               left->symlink != right->symlink) {
        out_diff->state = UMI_VCS_DIRECTORY_TYPE_CHANGED;
    } else if (left->size_bytes == right->size_bytes &&
               left->content_fingerprint == right->content_fingerprint) {
        out_diff->state = UMI_VCS_DIRECTORY_EQUAL;
    } else {
        out_diff->state = UMI_VCS_DIRECTORY_DIFFERENT;
    }
    return UMI_STATUS_OK;
}
