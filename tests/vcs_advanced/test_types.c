/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_types.c
 *
 * PURPOSE:
 *   Exercise stable VCS advanced vocabulary and helper contracts.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/types.h"

#include <string.h>

int main(void)
{
    char text[16];
    if (umi_vcs_advanced_copy_text(text, sizeof(text), "merge") != UMI_STATUS_OK) return 1;
    if (strcmp(text, "merge") != 0) return 2;
    if (!umi_vcs_advanced_text_present(text)) return 3;
    if (!umi_vcs_advanced_text_equal(text, "merge")) return 4;
    if (umi_vcs_advanced_hash_text("a") == umi_vcs_advanced_hash_text("b")) return 5;
    if (strcmp(umi_vcs_advanced_operation_text(UMI_VCS_ADVANCED_OPERATION_REBASE), "rebase") != 0) return 6;
    if (strcmp(umi_vcs_compare_mode_text(UMI_VCS_COMPARE_THREE_WAY), "three-way") != 0) return 7;
    return 0;
}
