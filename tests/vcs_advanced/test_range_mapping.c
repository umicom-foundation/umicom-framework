/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_range_mapping.c
 *
 * PURPOSE:
 *   Validate map source ranges to destination ranges after edits or diff alignment.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/range_mapping.h"

int main(void)
{
    UmiVcsAdvancedRangeMapping value;
    umi_vcs_advanced_range_mapping_init(&value);
    if (umi_vcs_advanced_range_mapping_validate(&value) == UMI_STATUS_OK) return 1;
    value.source_start = 10U; value.source_count = 5U; value.target_start = 12U; value.target_count = 5U; value.confidence_percent = 100U;
    if (umi_vcs_advanced_range_mapping_validate(&value) != UMI_STATUS_OK) return 2;
    if (umi_vcs_advanced_range_mapping_delta(&value) != 2LL) return 3;
    return 0;
}
