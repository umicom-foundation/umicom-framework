/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/staging_selection.c
 *
 * PURPOSE:
 *   Represent file, hunk or line scope for partial staging operations.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/staging_selection.h"

#include <string.h>

void umi_vcs_advanced_staging_selection_init(UmiVcsAdvancedStagingSelection *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_staging_selection_validate(const UmiVcsAdvancedStagingSelection *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->path) || (!value->entire_file && value->last_line < value->first_line))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_staging_selection_file(UmiVcsAdvancedStagingSelection *value,
                                                    const char *path,
                                                    int staged_side)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->path, sizeof(value->path), path);
    if (status != UMI_STATUS_OK) return status;
    value->entire_file = 1;
    value->staged_side = staged_side != 0;
    return umi_vcs_advanced_staging_selection_validate(value);
}
UmiStatus umi_vcs_advanced_staging_selection_lines(UmiVcsAdvancedStagingSelection *value,
                                                     const char *path,
                                                     size_t hunk_index,
                                                     size_t first_line,
                                                     size_t last_line)
{
    UmiStatus status;
    if (value == NULL || first_line == 0U || last_line < first_line) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->path, sizeof(value->path), path);
    if (status != UMI_STATUS_OK) return status;
    value->hunk_index = hunk_index;
    value->first_line = first_line;
    value->last_line = last_line;
    value->entire_file = 0;
    return umi_vcs_advanced_staging_selection_validate(value);
}
