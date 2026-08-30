/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/compare_side.c
 *
 * PURPOSE:
 *   Implement one immutable or editable side of a professional comparison session.
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
#include "umicom/vcs/advanced/compare_side.h"

#include <string.h>

void umi_vcs_advanced_compare_side_init(UmiVcsAdvancedCompareSide *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_compare_side_validate(const UmiVcsAdvancedCompareSide *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->path))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_compare_side_set(UmiVcsAdvancedCompareSide *value,
                                              const char *path,
                                              const char *revision,
                                              const char *label,
                                              int read_only)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->path, sizeof(value->path), path);
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->revision, sizeof(value->revision), revision);
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->label, sizeof(value->label), label);
    if (status != UMI_STATUS_OK) return status;
    value->read_only = read_only != 0;
    value->exists = 1;
    value->fingerprint = umi_vcs_advanced_hash_text(path) ^ umi_vcs_advanced_hash_text(revision);
    return umi_vcs_advanced_compare_side_validate(value);
}
