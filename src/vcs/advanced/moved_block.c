/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/moved_block.c
 *
 * PURPOSE:
 *   Capture identical or near-identical blocks moved within a document.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/moved_block.h"

#include <string.h>

void umi_vcs_advanced_moved_block_init(UmiVcsAdvancedMovedBlock *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_moved_block_validate(const UmiVcsAdvancedMovedBlock *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->line_count == 0U || value->confidence_percent > 100U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

int umi_vcs_advanced_moved_block_is_significant(const UmiVcsAdvancedMovedBlock *value,
                                                   size_t minimum_lines,
                                                   uint32_t minimum_confidence)
{
    if (umi_vcs_advanced_moved_block_validate(value) != UMI_STATUS_OK) return 0;
    return value->line_count >= minimum_lines &&
           value->confidence_percent >= minimum_confidence &&
           value->old_start != value->new_start;
}
