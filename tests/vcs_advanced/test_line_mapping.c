/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_line_mapping.c
 *
 * PURPOSE:
 *   Validate map individual lines between comparison sides with confidence evidence.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/line_mapping.h"

int main(void)
{
    UmiVcsAdvancedLineMapping value;
    umi_vcs_advanced_line_mapping_init(&value);
    if (umi_vcs_advanced_line_mapping_validate(&value) == UMI_STATUS_OK) return 1;
    value.left_line = 2U; value.right_line = 2U; value.confidence_percent = 100U; value.kind = UMI_VCS_DIFF_CONTEXT;
    if (!umi_vcs_advanced_line_mapping_exact(&value)) return 2;
    return 0;
}
