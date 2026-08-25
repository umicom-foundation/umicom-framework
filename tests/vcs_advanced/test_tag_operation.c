/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_tag_operation.c
 *
 * PURPOSE:
 *   Validate plan tag creation, deletion or verification with explicit signing intent.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/tag_operation.h"

int main(void)
{
    UmiVcsAdvancedTagOperation value;
    umi_vcs_advanced_tag_operation_init(&value);
    if (umi_vcs_advanced_tag_operation_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_tag_operation_create(&value, "v1.0", "HEAD", "release", 1) != UMI_STATUS_OK) return 2;
    if (value.sign == 0) return 3;
    return 0;
}
