/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/range_mapping.c
 *
 * PURPOSE:
 *   Map source ranges to destination ranges after edits or diff alignment.
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
#include "umicom/vcs/advanced/range_mapping.h"

#include <string.h>

void umi_vcs_advanced_range_mapping_init(UmiVcsAdvancedRangeMapping *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

UmiStatus umi_vcs_advanced_range_mapping_validate(const UmiVcsAdvancedRangeMapping *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->source_count == 0U || value->target_count == 0U || value->confidence_percent > 100U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

long long umi_vcs_advanced_range_mapping_delta(const UmiVcsAdvancedRangeMapping *value)
{
    if (umi_vcs_advanced_range_mapping_validate(value) != UMI_STATUS_OK) return 0LL;
    return (long long)value->target_start - (long long)value->source_start;
}
int umi_vcs_advanced_range_mapping_contains_source(const UmiVcsAdvancedRangeMapping *value,
                                                     size_t line)
{
    if (umi_vcs_advanced_range_mapping_validate(value) != UMI_STATUS_OK) return 0;
    return line >= value->source_start && line < value->source_start + value->source_count;
}
